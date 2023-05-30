# pragma once

# include <iostream>
# include <string>
# include <vector>
# include <map>

class Message;
class Client;
class Channel;
class Server;
class Command
{
	public:
		Command(Client *client, Server *server, Message *message);
		~Command();

		Client		*getClient(void) const;
		Server		*getServer(void) const;
		Message		*getMessage(void) const;

		void reply(const std::string& message) const;
		void reply(short code, const std::string& message) const;
		void reply(Client &client, short code, const std::string& message) const;
		void replyClient(short code, const std::string& message) const;
		void replyOther(Client &client, short code, const std::string& message) const;
		void replyChannel(Channel &channel, short code, const std::string& message) const;
		void replyServer(short code, const std::string& message) const;
		void replyClient(const std::string& message) const;
		void replyOther(Client &client, const std::string& message) const;
		void replyChannel(Channel &channel, const std::string& message) const;
		void replyServer(const std::string& message) const;
		void replyNeedMoreParams(const std::string& message) const;
		void replyFromGeneric(const std::string& sender, short code, const std::string& message) const;
		void replyFromGeneric(const std::string& sender, const std::string& message) const;
		void replyFromServer(short code, const std::string& message) const;
		void replyFromServer(const std::string& message) const;

	private:
		Client				*__client;
		Server				*__server;
		Message				*__message;
};

namespace Commands{
	void	NICK(Command *command);
	void	USER(Command *command);
	void	OPER(Command *command);
	void	QUIT(Command *command);
	void	SQUIT(Command *command);
	void	PASS(Command *command);
	void    MODE(Command *command);
	void	SERVICE(Command *command);
    void    PING(Command *command);
	void	JOIN(Command *command);
	void	PART(Command *command);
	void	CMODE(Command *command);
	void	TOPIC(Command *command);
	void	NAMES(Command *command);
	void	LIST(Command *command);
	void	INVITE(Command *command);
	void	KICK(Command *command);
	void	PRIVMSG(Command *command);
	void	NOTICE(Command *command);
	void	MOTD(Command *command);
	void	LUSERS(Command *command);
	void	VERSION(Command *command);
	void    STATS(Command *command);
    void    LINKS(Command *command);
	void    TIME(Command *command);
    void    CONNECT(Command *command);
	void	TRACE(Command *command);
    void    ADMIN(Command *command);
	void	INFO(Command *command);
	void	SERVLIST(Command *command);
	void	SQUERY(Command *command);
	void	WHO(Command *command);
	void	WHOIS(Command *command);
	void	WHOWAS(Command *command);
	void	KILL(Command *command);
	void	ERROR(Command *command);
	void	AWAY(Command *command);
	void	UNSUPPORTED(Command *command);
}
