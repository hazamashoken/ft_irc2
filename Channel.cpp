# include "Channel.hpp"
# include "Client.hpp"

Channel::Channel(const std::string& name)
: __name(name)
{
}

Channel::~Channel() {
    // Destructor implementation
}

const std::string& Channel::getName() const {
	return __name;
}

void Channel::addClient(Client* client) {
	__clients.insert(std::pair<std::string, Client*>(client->getNickname(), client));
}

void Channel::removeClient(Client* client) {
	removeOperator(client);
	__clients.erase(client->getNickname());
}

bool Channel::hasClient(Client* client) const {
	std::map<std::string, Client*>::const_iterator it = __clients.find(client->getNickname());
	if (it == __clients.end()) {
		return false;
	}
	return true;
}

void Channel::setTopic(const std::string &topic) {
	__topic = topic;
}

const std::string& Channel::getTopic() const {
	return __topic;
}

void Channel::setKey(const std::string &key) {
	__key = key;
}

const std::string& Channel::getKey() const {
	return __key;
}

void Channel::setInviteOnly(bool invite)
{
	__inviteOnly = invite;
}

bool Channel::getInviteOnly() const
{
	return __inviteOnly;
}

void Channel::addInvite(const std::string& nickname)
{
	__invites.insert(nickname);
}

void Channel::removeInvite(const std::string& nickname)
{
	__invites.erase(nickname);
}

bool Channel::hasInvite(const std::string& nickname)
{
	return (__invites.count(nickname) != 0);
}

void Channel::clearInvite()
{
	__invites.clear();
}

void Channel::addOperator(Client *client) {
	if (hasClient(client))
		__operators.insert(client);
}

void Channel::removeOperator(Client *client) {
	__operators.erase(client);
}

bool Channel::isOperator(Client *client) const {
	return (__operators.count(client) == 1);
}

int Channel::nrClients() const {
	return static_cast<int>(__clients.size());
}

int Channel::nrVisible() const {
	std::map<std::string, Client *>::const_iterator it;
	int count;

	count = 0;
	for (it = __clients.begin(); it != __clients.end(); it++)
		if (!it->second->hasMode('i'))
			count++;
	return (count);
}

void Channel::sendMessage(Client* sender, const std::string& message) {
	std::map<std::string, Client*>::iterator it = __clients.begin();
	while (it != __clients.end()) {
		it->second->sendMessage(sender, message);
		it++;
	}
}

const std::map<std::string, Client*>& Channel::getClients() const {
	return __clients;
}

std::map<std::string, Client*>::iterator Channel::getClientsBegin() {
	return __clients.begin();
}

void Channel::sendToAllClients(const std::string& message) {
	std::map<std::string, Client*>::iterator it = __clients.begin();
	while (it != __clients.end()) {
		it->second->sendReply(message);
		it++;
	}
}

void Channel::sendToAllClients(short code, const std::string& message) {
	std::map<std::string, Client*>::iterator it = __clients.begin();
	while (it != __clients.end()) {
		it->second->sendReply(code, message);
		it++;
	}
}
