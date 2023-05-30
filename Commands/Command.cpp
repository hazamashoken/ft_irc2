# include "../utils/utils.hpp"
# include "RPL.hpp"
# include "Command.hpp"
# include "../Client.hpp"
# include "../Channel.hpp"
# include "../Server.hpp"
# include "../parsers/Message.hpp"

Command::Command(Client *client, Server *server, Message* message)
: __client(client), __server(server), __message(message)
{
}

Command::~Command()
{
	delete __message;
}

void Command::reply(const std::string& message) const
{
    __client->sendReply(message);
}

void Command::reply(short code, const std::string& message) const
{
	__client->sendReply(code, message);
}

void Command::reply(Client &client, short code, const std::string& message) const
{
	client.sendReply(code, message);
}

void Command::replyClient(short code, const std::string& message) const
{
	replyClient(itostring(code, true)  + " " + __client->getNickname() + " " + message);
}

void Command::replyOther(Client& client, short code, const std::string& message) const
{
	replyOther(client, itostring(code, true)  + " " + client.getNickname() + " " + message);
}

void Command::replyOther(Client& client, const std::string& message) const
{
	client.appendSendBuffer(":" + __client->getPrefix() + " " + message);
}

void Command::replyChannel(Channel& channel, short code, const std::string& message) const
{
	std::map<std::string, Client *>::const_iterator it;
	Client *other;

	for (it = channel.getClients().begin(); it != channel.getClients().end(); it++)
	{
		// send from the client to all other clients in the channel
		other = it->second;
		if (__client != other)
			replyOther(*other, itostring(code, true)  + " " + __client->getNickname() + " " + message);
	}
}

void Command::replyServer(short code, const std::string& message) const
{
	std::map<int, Client *>::const_iterator it;
	Client *other;

	for (it = __server->getClients().begin(); it != __server->getClients().end(); it++)
	{
		// send from the client to all other clients in the server
		other = it->second;
		if (__client != other)
			replyOther(*other, itostring(code, true) + " " + __client->getNickname() + " " + message);
	}
}

void Command::replyClient(const std::string& message) const
{
	__client->sendReply(message);
}

void Command::replyChannel(Channel& channel, const std::string& message) const
{
	std::map<std::string, Client *>::const_iterator it;
	Client *other;

	for (it = channel.getClients().begin(); it != channel.getClients().end(); it++)
	{
		// send from the client to all other clients in the channel
		other = it->second;
		if (__client != other)
			replyOther(*other, message);
	}
}

void Command::replyServer(const std::string& message) const
{
	std::map<int, Client *>::const_iterator it;
	Client *other;

	for (it = __server->getClients().begin(); it != __server->getClients().end(); it++)
	{
		// send from the client to all other clients in the server
		other = it->second;
		if (__client != other)
			replyOther(*other, message);
	}
}

void Command::replyNeedMoreParams(const std::string& message) const
{
    __client->appendSendBuffer(":" + __server->getHostname()
								+ " " + itostring(IRC::ERR_NEEDMOREPARAMS, true)
								+ " " + __client->getNickname() + " "
								+ message + " :Not enough parameters");
}

void Command::replyFromGeneric(const std::string& sender, short code, const std::string& message) const
{
    __client->appendSendBuffer(":" + __server->getHostname()
								+ " " + itostring(code, true)
								+ " " + sender + " "
								+ message);
}

void Command::replyFromGeneric(const std::string& sender, const std::string& message) const
{
    __client->appendSendBuffer(":" + __server->getHostname()
								+ " " + sender + " "
								+ message);
}

void Command::replyFromServer(short code, const std::string& message) const
{
    __client->appendSendBuffer(":" + __server->getHostname()
								+ " " + itostring(code, true)
								+ " " + message + " ");
}

void Command::replyFromServer(const std::string& message) const
{
    __client->appendSendBuffer(":" + __server->getHostname() + " " + message);
}

Client	*Command::getClient(void) const
{
	return __client;
}

Server	*Command::getServer(void) const
{
	return __server;
}

Message *Command::getMessage() const
{
	return __message;
}
