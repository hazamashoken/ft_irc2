# include "Client.hpp"
# include "Server.hpp"
# include "Channel.hpp"
# include "Status.hpp"

Client::Client(const int fd, struct sockaddr_in addr)
: __socket(fd), __address(addr), __hostname(std::string(inet_ntoa(addr.sin_addr))), __status(Status::PASSWORD), __registered(false), __modes("w")
{
	__servername = __hostname;
    Debug::debug("DEBUG", __hostname);
}

Client::~Client() {
}

void Client::sendMessage(Client *target, const std::string& message) {
	send(target->__socket, message.c_str(), message.length(), 0);
}

void Client::sendMessage(Channel *channel, const std::string& message) {
	std::map<std::string, Client *>::iterator it = channel->getClientsBegin();
	while (it != channel->getClients().end()) {
		send(it->second->__socket, message.c_str(), message.length(), 0);
		it++;
	}
}

void Client::joinChannel(const std::string& channelName) {
	// check if client is already in channel
	{
		std::map<std::string, Channel *>::iterator it = __currentChannels.find(channelName);
		if (it != __currentChannels.end()) {
			return;
		}
	}
	// check if channel exists
	{
		std::map<std::string, Channel *>::iterator it = Server::__channels.find(channelName);
		if (it == Server::__channels.end()) {
			Channel *newChannel = new Channel(channelName);
			Server::__channels.insert(std::pair<std::string, Channel *>(channelName, newChannel));
			__currentChannels.insert(std::pair<std::string, Channel *>(channelName, newChannel));
			newChannel->addClient(this);
		} else {
			it->second->addClient(this);
			__currentChannels.insert(std::pair<std::string, Channel *>(channelName, it->second));
		}
	}
}

void Client::partChannel(const std::string& channelName) {
	std::map<std::string, Channel *>::iterator it;

	// check if client is in channel	
	// it = __currentChannels.find(channelName);
	// if (it == __currentChannels.end()) {
	// 	return;
	// }
	// remove channel from client's currentChannels
	__currentChannels.erase(channelName);

	// remove client from channel's clients
	it = Server::__channels.find(channelName);
	if (it == Server::__channels.end()) {
		return;
	}
	it->second->removeClient(this);
	// check if channel is empty
	if (it->second->getClients().size() == 0) {
		// delete channel
		delete it->second;
		// remove channel from Server::__channels
		Server::__channels.erase(channelName);
	}
}

void Client::setNickname(const std::string& newNickname) {
	__nickname = newNickname;
}

void Client::setUsername(const std::string& newUsername) {
	__username = newUsername;
}

void Client::setRealname(const std::string& newRealname) {
	__realname = newRealname;
}

void Client::setServername(const std::string& newServername) {
	__servername = newServername;
}

const std::string& Client::getNickname() const {
	return __nickname;
}

const std::string& Client::getUsername() const {
	return __username;
}

const std::string& Client::getRealname() const {
	return __realname;
}

const std::string& Client::getHostname() const {
	return __hostname;
}

const std::string& Client::getServername() const {
	return __servername;
}

const std::string& Client::getReadBuffer() const {
	return __readBuffer;
}

bool Client::isAliveClient() {
	char buffer[512];
	int bytes = recv(__socket, buffer, 512, 0);
	if (bytes == 0 || bytes == -1) {
		return false;
	}
	__readBuffer.append(buffer, bytes);
	return true;
}

int Client::getFd() const {
	return __socket;
}

void Client::sendReply(const std::string& reply)
{
    std::string message = ":" + getPrefix() + " " + reply + "\r\n";
    if (DEBUG)
    {
        std::stringstream o;
        o << "fd: " << __socket << " > " << message;
        Debug::debug("REPLY", o.str());
    }
    __sendBuffer.append(message);
}

void Client::sendReply(short code, const std::string& reply) {
    std::string message = ":" + getPrefix() + " " + itostring(code, true) + " " + getNickname()+ " :" + reply + "\r\n";
	if (DEBUG)
	{
		std::stringstream o;
		o << "fd: " << __socket << " > " << message;
		Debug::debug("REPLY", o.str());
	}
    __sendBuffer.append(message);
}

void Client::sendReply(short code, const std::string& reply, const std::string& message) {
	std::string msg = ":" + getPrefix() + " " + itostring(code, true)  + " " + getNickname()+ " :" + reply + " " + message + "\r\n";
    if (DEBUG)
	{
		std::stringstream o;
		o << "fd: " << __socket << " > " << msg;
		Debug::debug("REPLY", o.str());
	}
	__sendBuffer.append(msg);
}

void Client::setReadBuffer(const std::string& newReadBuffer) {
	__readBuffer = newReadBuffer;
}

void Client::setRegistered(bool registered) {
	__registered = registered;
}

void Client::setOperator(bool ops) {
	__isops = ops;
	if (ops)
		addMode('o');
	else
		remMode('o');
}

const std::string  Client::getPrefix() const {
	return __nickname + "!" + __username + "@" + __hostname;
}

void Client::setStatus(int status) {
	__status = status;
}

const std::string& Client::getSendBuffer() const {
	return __sendBuffer;
}

void Client::appendSendBuffer(const std::string& message) {
	__sendBuffer.append(message + "\r\n");
}

void Client::clearSendBuffer() {
	__sendBuffer.clear();
}

int Client::getStatus() const {
	return __status;
}

bool Client::isRegistered() const {
    return __registered;
}

bool Client::isOperator() const {
    return __isops;
}

bool Client::isAway() const
{
	return !__away.empty();
}

const std::string& Client::getAway() const
{
	return __away;
}

void Client::setAway(const std::string& message)
{
	if (message.empty())
		__away = "is away";
	else
		__away = message;
}

void Client::removeAway()
{
	__away.clear();
}

void Client::flushSendBuffer() {
    if (__sendBuffer.empty())
        return;
    Debug::debug("FLUSH", __sendBuffer);
    if (send(__socket, __sendBuffer.c_str(), __sendBuffer.length(), 0) == -1)
        error("send");
    __sendBuffer.clear();
}

const std::string& Client::getModes() const {
    return __modes;
}

void Client::addPrevNick(const std::string& nick)
{
    __prev_nick.push_back(nick);
}

const std::vector<std::string>& Client::getPrevNicks() const {
    return __prev_nick;
}

void Client::addMode(char mode)
{
	if (__modes.find(mode) == std::string::npos)
		__modes.push_back(mode);
}

void Client::remMode(char mode)
{
	size_t p;

	p = __modes.find(mode);
	if (p != std::string::npos)
		__modes.erase(p, 1);
}

bool Client::hasMode(char mode)
{
	if (__modes.find(mode) == std::string::npos)
		return (false);
	return (true);
}
