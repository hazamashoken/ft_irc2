# pragma once

// Prototypes for the Channel class in an IRC server implementation in C++ (std=98)

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

class Client;

class Channel {
	public:
		Channel(const std::string& name);
		~Channel();

		const std::string& getName() const;
		void addClient(Client* client);
		void removeClient(Client* client);
		bool hasClient(Client* client) const;
		void setTopic(const std::string &topic);
		const std::string& getTopic() const;
		void setKey(const std::string &key);
		const std::string& getKey() const;
		void setInviteOnly(bool invite);
		bool getInviteOnly() const;
		void addInvite(const std::string& nickname);
		void removeInvite(const std::string& nickname);
		bool hasInvite(const std::string& nickname);
		void clearInvite();
		void addOperator(Client *client);
		void removeOperator(Client *client);
		bool isOperator(Client *client) const;
		int nrClients() const;
		int nrVisible() const;
		void sendMessage(Client* sender, const std::string& message);
		void sendToAllClients(const std::string& message);
		void sendToAllClients(short code, const std::string& message);
		const std::map<std::string, Client*>& getClients() const;
		std::map<std::string, Client*>::iterator getClientsBegin();

	private:
		std::string __name;
		std::string __topic;
		std::string __key;
		std::map<std::string, Client *> __clients;
		std::set<Client *> __operators;
		bool __inviteOnly;
		std::set<std::string> __invites;
};
