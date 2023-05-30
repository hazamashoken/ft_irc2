#pragma once

# include <iostream>
# include <string>
# include <map>
# include <cstring>
# include <cstdlib>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <fcntl.h>
# include <arpa/inet.h>
# include <limits.h>
#include <netdb.h>

# include "utils/utils.hpp"

class Server;
class Channel;

class Client {
public:
	Client(const int fd, struct sockaddr_in addr);
	~Client();

	void sendMessage(Client *target, const std::string& message);
	void sendMessage(Channel *channel, const std::string& message);
	void sendReply(const std::string& reply);
	void sendReply(short code, const std::string& reply);
	void sendReply(short code, const std::string& reply, const std::string& message);
	void joinChannel(const std::string& channelName);
	void partChannel(const std::string& channelName);
	void setNickname(const std::string& newNickname);
	void setUsername(const std::string& newUsername);
	void setRealname(const std::string& newRealname);
	void setServername(const std::string& newServername);
	void setStatus(int status);
	void addPrevNick(const std::string& nick);
	void addMode(char mode);
	void remMode(char mode);
	bool hasMode(char mode);
	const std::string& getNickname() const;
	const std::string& getUsername() const;
	const std::string& getRealname() const;
	const std::string& getHostname() const;
	const std::string& getServername() const;
	const std::string& getModes() const;
	const std::vector<std::string>& getPrevNicks() const;
	int getStatus() const;
	bool isRegistered() const;
	bool isOperator() const;
	bool isAway() const;
	const std::string& getAway() const;
	void setAway(const std::string& message);
	void removeAway();

	const std::string& getReadBuffer() const;
	void setReadBuffer(const std::string& message);
	int getFd() const;
	bool isAliveClient();
	void setRegistered(bool registered);
	void setOperator(bool ops);
	const std::string getPrefix() const;

	const std::string& getSendBuffer() const;
	void appendSendBuffer(const std::string& message);
	void clearSendBuffer();
	bool checkSendBuffer() const;
	void flushSendBuffer();

private:
	int __socket;
	struct sockaddr_in __address;
	std::string __nickname;
	std::string __username;
	std::string __realname;
	const std::string __hostname;

	std::string __servername;
	int	__status;
	std::map<std::string, Channel*> __currentChannels;
	std::string __readBuffer;
	bool __registered;
	bool __isops;
	std::string __modes;
	std::vector<std::string> __prev_nick;
	std::string __away;



	std::string __sendBuffer;
};


