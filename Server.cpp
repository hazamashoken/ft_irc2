# include "Server.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "Commands/Command.hpp"
# include "Status.hpp"
# include "Commands/RPL.hpp"
# include "parsers/Message.hpp"

std::map<std::string, Channel *> Server::__channels;

Server::Server(const char *port, const char *pass)
: __port(std::string(port)), __pass(std::string(pass)),
	 __addrlen(sizeof(__server_addr)), __hostname("localhost"),
	  __version("0.0.1"), __creationDate(std::time(NULL)),
      __availableUserModes("aiorw"), __availableChannelModes("Oovimnptkl")
{
	/// @brief set default operator password
	setOperatorPass(getPass());

	/// @brief setup the commmand pallet
	initializeCommandPallet();

	/// @brief setup the server
	initializeServer();
}

Server::~Server()
{
	{
		std::map<std::string, Channel *>::iterator it = __channels.begin();
		while (it != __channels.end())
		{
			delete it->second;
			++it;
		}
	}
	{
		std::map<int, Client *>::iterator it = __clients.begin();
		while (it != __clients.end())
		{
			delete it->second;
			++it;
		}
	}
	close(__server_fd);
}

/// @brief initialize the server
void Server::initializeServer()
{
	/// @brief create the server socket
	if ((__server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		exit(error("socket() failed", 1));

	/// @brief set the socket options
	int enable = 1;

	/// @brief set the socket to reuse the address
	if (setsockopt(__server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
		exit(error("setsockopt(SO_REUSEADDR) failed", 1));

	/// @brief set the socket to non blocking
	if (fcntl(__server_fd, F_SETFL, O_NONBLOCK) < 0)
		exit(error("fcntl() failed", 1));

	/// @brief set the server address for IPv4 to accept any address at the given port
	__server_addr.sin_family = AF_INET;
	__server_addr.sin_addr.s_addr = INADDR_ANY;
	Debug::debug(std::string("port: ") + __port);
	__server_addr.sin_port = htons(atoi(__port.c_str()));

	/// @brief bind the socket to the address
	if (bind(__server_fd, (struct sockaddr *)&__server_addr, __addrlen) < 0)
		exit(error("bind() failed", 1));

	/// @brief listen for new connections
	if (listen(__server_fd, __server_addr.sin_port) < 0)
		exit(error("listen() failed", 1));

	/// @brief add the server fd to the pollfd vector
	__pfds.push_back((pollfd){__server_fd, POLLIN | POLLHUP, 0});

}

/// @brief accept a new client
/// @return 0 on success, 1 on failure
bool Server::acceptNewClient()
{
	/// @brief set the new client address and size
	int new_client_fd = 0;
	sockaddr_in new_client_addr;
	socklen_t new_client_addr_size = sizeof(new_client_addr);

	/// @brief accept the new client fd
	new_client_fd = accept(__server_fd, (struct sockaddr *)&new_client_addr, &new_client_addr_size);
	if (new_client_fd == -1)
		return(error("accept() failed", 1));

	/// @brief add the new client to the clients map
	__clients.insert(std::pair<int, Client *>(new_client_fd, new Client(new_client_fd, new_client_addr)));

	if (DEBUG)
	{
		std::stringstream ss;
		ss <<  new_client_fd;
		std::string str = ss.str();
		Debug::debug(std::string("new client: ") + str);
	}

	/// @brief add the new client fd to the pollfd vector
	__pfds.push_back((pollfd){new_client_fd, POLLIN | POLLHUP, 0});
	return (0);
}

void Server::processClients()
{
	/// @brief set poll for the fds
	int ret = poll(&__pfds[0], __pfds.size(), -1);
	if (ret < 0) {
		perror("poll failed");
		exit(EXIT_FAILURE);
	}

	/// @brief iterate through the pollfd vector
	for (size_t i = 0; i < __pfds.size(); ++i) {

		/// @brief check for read events on each fd
		if (__pfds[i].revents & POLLIN) {

			/// @brief check if the event is on the server fd
			if (__pfds[i].fd == __server_fd) {

				/// @brief accept the new client
				if (acceptNewClient())
					error("acceptNewClient() failed");

			/// @brief else it is a client fd
			} else if (__clients.count(__pfds[i].fd) != 0) {

				// Debug::debug("client detected");
				/// check if the client is still alive aka recv() returns not 0
				if (__clients[__pfds[i].fd]->isAliveClient() == false)
				{
					/// @brief remove the client from the clients map
					/// @todo move all execution to executeCommand
					__clients[__pfds[i].fd]->setStatus(Status::DELETE);
					Debug::debug("client closed");
					disconnectClient(__clients[__pfds[i].fd]);
					close(__pfds[i].fd);
					__pfds.erase(__pfds.begin() + i);
					continue ;
				}
				// Debug::debug("client handled");

				/// @brief handle the client
				handleClient(__clients[__pfds[i].fd]);
			}
		} else if (__pfds[i].revents & POLLHUP) {
			/// @brief remove the client from the clients map
			/// @todo move all execution to executeCommand
			Debug::debug("client closed");
			if (__clients.count(__pfds[i].fd) != 0)
				disconnectClient(__clients[__pfds[i].fd]);
			close(__pfds[i].fd);
			__pfds.erase(__pfds.begin() + i);
			break ;
		}
	}

	/// @todo move the send() from inside the client to here
	/// @remarks this will allow to send() to all clients at once instead of one by one which cause blocking
	for (std::map<int, Client *>::iterator it = __clients.begin(); it != __clients.end(); ++it)
	{
		if (it->second->getStatus() == Status::DELETE)
		{
			Debug::debug("client closed deleted");
			for (size_t i = 0; i < __pfds.size(); i++)
			{
				if (__pfds[i].fd == it->second->getFd())
				{
					close(__pfds[i].fd);
					__pfds.erase(__pfds.begin() + i);
					break ;
				}
			}
			disconnectClient(it->second);
			break ;
		}
		it->second->flushSendBuffer();
	}
}

void Server::handleClient(Client *client)
{
	std::string command;
	std::vector<std::string> args;
	std::string line;

	/// @brief check if client buffer is empty
	if (client->getReadBuffer().empty())
	{
		Debug::debug("empty read buffer");
		return ;
	}
	line = client->getReadBuffer();
	Debug::debug("line: " + line);
	// if (line.find("\r\n") == std::string::npos)
		// return ;
	// client->setReadBuffer(line.substr(line.find("\r\n") + 2, line.size()));
	while (line.find("\r\n") != std::string::npos)
	{
		command = line.substr(0, line.find("\r\n") + 2);
		line = line.substr(line.find("\r\n") + 2, line.size());
		executeCommand(client, command);
	}
	client->setReadBuffer(line);
	Debug::debug("line: " + line);

	// debugSendToAllClients(line);

}

Client* Server::getClientByNickname(const std::string& nickname)
{
	std::map<int, Client *>::iterator it = __clients.begin();
	while (it != __clients.end())
	{
		if (it->second->getNickname() == nickname)
			return (it->second);
		++it;
	}
	return (NULL);
}

Client* Server::getClientByUsername(const std::string& username)
{
	std::map<int, Client *>::iterator it = __clients.begin();
	while (it != __clients.end())
	{
		if (it->second->getUsername() == username)
			return (it->second);
		++it;
	}
	return (NULL);
}

Channel* Server::getChannelByName(const std::string& channelName)
{
	std::map<std::string, Channel *>::iterator it;

	it = __channels.find(channelName);
	if (it != __channels.end())
		return it->second;
	// std::map<std::string, Channel *>::iterator it = __channels.begin();
	// while (it != __channels.end())
	// {
	// 	if (it->second->getName() == channelName)
	// 		return (it->second);
	// 	++it;
	// }
	return (NULL);
}

void Server::executeCommand(Client* client, std::string& command)
{
	Message *token = new Message;
	Command *cmd = NULL;
	Debug::debug("command: " + command);
	token->setMsg(command);
	if (client->getStatus() == Status::DELETE)
		return ;
	if (client->getStatus() == Status::PASSWORD)
	{
		/// @brief check if the client is trying to send a password
		Debug::debug("password");
		if (token->getMsgTypeCmd() != IRC_PASS)
			return ;
	} else if (client->getStatus() == Status::REGISTER) {
		/// @brief check if the client is trying to register
		Debug::debug("register");
		if (token->getMsgTypeCmd() != IRC_NICK && token->getMsgTypeCmd() != IRC_USER)
			return ;
	}
	if (__commands.count(token->getMsgTypeCmd()))
	{
		cmd = new Command(client, this, token);
		// Debug::debug(Debug::msgType[token->getMsgType()]);
		__commands[token->getMsgTypeCmd()](cmd);
	}
	else
	{
		Debug::debug("command not found: " + token->param(0));
        client->appendSendBuffer("ERROR :Unkonwn command '" + token->param(0) + "'");
		// client->sendReply(IRC::ERR_UNKNOWNCOMMAND, client->getPrefix(), command);
	}

}

/// @brief disconnect a client from all channels and remove it from the server
/// @param client the client to disconnect
void Server::disconnectClient(Client *client)
{
	/// @brief remove the client from all channels
	std::map<std::string, Channel *>::iterator tmp;
	std::map<std::string, Channel *>::iterator it = __channels.begin();
	while (it != __channels.end())
	{
		tmp = it;
		++it;
		tmp->second->removeOperator(client);
		tmp->second->removeClient(client);
	}

	/// @brief remove the client from the server
	__clients.erase(client->getFd());
}

void Server::debugSendToAllClients(const std::string& message)
{
	std::map<int, Client *>::iterator it = __clients.begin();
	while (it != __clients.end())
	{
		send(it->second->getFd(), message.c_str(), message.size(), 0);
		++it;
	}
}

std::string Server::getHostname() const
{
	return __hostname;
}

std::string Server::getVersion() const
{
	return __version;
}

std::string Server::getCreationDate() const
{
	std::string creation = std::ctime(&__creationDate);

	// remove \n at end of string
	if (!creation.empty())
		creation.erase(creation.size() - 1);
	return creation;
}

std::string Server::getUptime() const
{
	std::stringstream uptime;
	long difference;
	long days;
	long hours;
	long minutes;
	long seconds;
	time_t now;

	time(&now);
	difference = static_cast<long>(difftime(now, __creationDate));
	days = difference / 86400L;
	hours = (difference - (days * 86400L)) / 3600L;
	minutes = (difference - (days * 86400L) - (hours * 3600L)) / 60L;
	seconds = difference - (days * 86400L) - (hours * 3600L) - (minutes * 60L);
	uptime << days << " days " << hours
		<< ':' << std::setfill('0') << std::setw(2) << minutes
		<< ':' << std::setfill('0') << std::setw(2) << seconds;
	return std::string(uptime.str());
}

const std::string& Server::getPass() const
{
	return __pass;
}

const std::string& Server::getOperatorPass() const
{
	return __operatorPass;
}

void Server::setOperatorPass(const std::string& pass)
{
	__operatorPass = pass;
}

const std::map<int, Client *>& Server::getClients() const
{
    return __clients;
}

const std::map<std::string, Channel *>& Server::getChannels() const
{
    return __channels;
}

const std::map<int, Client *>& Server::getOperators() const
{
    return __operators;
}

std::string Server::getAvailableUserModes() const
{
    return __availableUserModes;
}

std::string Server::getAvailableChannelModes() const
{
    return __availableChannelModes;
}

void Server::incCommandStats(const std::string &command)
{
	std::map<std::string, int>::iterator it;

	it = __commandStats.find(command);
	if (it != __commandStats.end())
		it->second++;
	else
		__commandStats.insert(std::make_pair(command, 1));
}

const std::map<std::string, int>& Server::getCommandStats() const
{
	return __commandStats;
}

/// @brief initialize the command pallet
void Server::initializeCommandPallet()
{
	/// @brief __commands[IRC::Message::Type] = &Server::command
	__commands[IRC_PASS] = Commands::PASS;
	__commands[IRC_NICK] = Commands::NICK;
	__commands[IRC_USER] = Commands::USER;
	__commands[IRC_OPER] = Commands::OPER;
	__commands[IRC_QUIT] = Commands::QUIT;
	__commands[IRC_SQUIT] = Commands::SQUIT;
    __commands[IRC_MODE] = Commands::MODE;
	__commands[IRC_SERVICE] = Commands::SERVICE;
    __commands[IRC_PING] = Commands::PING;
	__commands[IRC_JOIN] = Commands::JOIN;
	__commands[IRC_PART] = Commands::PART;
	__commands[IRC_CMODE] = Commands::CMODE;
	__commands[IRC_TOPIC] = Commands::TOPIC;
	__commands[IRC_NAMES] = Commands::NAMES;
	__commands[IRC_LIST] = Commands::LIST;
	__commands[IRC_INVITE] = Commands::INVITE;
	__commands[IRC_KICK] = Commands::KICK;
	__commands[IRC_PRIVMSG] = Commands::PRIVMSG;
	__commands[IRC_NOTICE] = Commands::NOTICE;
	__commands[IRC_MOTD] = Commands::MOTD;
	__commands[IRC_LUSERS] = Commands::LUSERS;
	__commands[IRC_VERSION] = Commands::VERSION;
    __commands[IRC_STATS] = Commands::STATS;
    __commands[IRC_LINKS] = Commands::LINKS;
	__commands[IRC_TIME] = Commands::TIME;
	__commands[IRC_CONNECT] = Commands::CONNECT;
	__commands[IRC_TRACE] = Commands::TRACE;
    __commands[IRC_ADMIN] = Commands::ADMIN;
    __commands[IRC_INFO] = Commands::INFO;
	__commands[IRC_SERVLIST] = Commands::SERVLIST;
	__commands[IRC_SQUERY] = Commands::SQUERY;
	__commands[IRC_WHO] = Commands::WHO;
	__commands[IRC_WHOIS] = Commands::WHOIS;
	__commands[IRC_WHOWAS] = Commands::WHOWAS;
	__commands[IRC_KILL] = Commands::KILL;
	__commands[IRC_ERROR] = Commands::ERROR;
	__commands[IRC_AWAY] = Commands::AWAY;
	__commands[IRC_UNSUPPORTED] = Commands::UNSUPPORTED;
}
