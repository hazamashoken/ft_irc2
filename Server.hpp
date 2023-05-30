# pragma once

# include <iostream>
# include <ostream>
# include <sstream>
# include <iomanip>
# include <string>
# include <map>
# include <vector>
# include <cstring>
# include <cstdlib>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <fcntl.h>
# include <poll.h>
# include <ctime>

# include "utils/utils.hpp"
# include "utils/ANSI.hpp"

class Command;
class Client;
class Channel;
class Message;
class Server
{
	public:
		Server(const char *port, const char *pass);
		~Server();

		void initializeServer();
		void initializeCommandPallet();
		bool acceptNewClient();
		void processClients();
		void disconnectClient(Client *client);
		void handleClient(Client *client);
		Client* getClientByNickname(const std::string& nickname);
		Client* getClientByUsername(const std::string& username);
		Channel* getChannelByName(const std::string& channelName);
        const std::map<int, Client *>& getClients() const;
        const std::map<int, Client *>& getOperators() const;
        const std::map<std::string, Channel *>& getChannels() const;
        std::string getAvailableUserModes() const;
        std::string getAvailableChannelModes() const;
		const std::string& getPass() const;
		const std::string& getOperatorPass() const;
		void setOperatorPass(const std::string& pass);
		std::string getHostname() const;
		std::string getVersion() const;
		std::string getCreationDate() const;
		std::string getUptime() const;
		void executeCommand(Client *client, std::string &message);
		void incCommandStats(const std::string &command);
		const std::map<std::string, int>& getCommandStats() const;

		static std::map<std::string, Channel *> __channels;

	private:
		int __server_fd;
		std::string __port;
		std::string __pass;
		std::string __operatorPass;
		struct sockaddr_in __server_addr;
		int __addrlen;
		std::vector<pollfd> __pfds;
		std::map<int, Client *> __clients;
        std::map<int, Client *> __operators;
		std::map<int, void (*)(Command *)> __commands;
		std::string __hostname;
		std::string __version;
		time_t __creationDate;
        std::string __availableUserModes;
        std::string __availableChannelModes;
		std::map<std::string, int> __commandStats;

		void debugSendToAllClients(const std::string& message);
};
