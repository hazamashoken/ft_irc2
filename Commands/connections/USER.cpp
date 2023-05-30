# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
namespace Commands {
	static bool nickTaken(Command *command, const std::string &nickname)
	{
		// check if client already has this nickname
		if (command->getClient()->getNickname().compare(nickname) == 0)
			return (false);
		// check if nickname is in use
		if (command->getServer()->getClientByNickname(nickname) == NULL)
			return (false);
		return (true);
	}

	static std::string makeTempNick(Command *command)
	{
		int i;
		std::string nickname;

		nickname = command->getClient()->getUsername();
		for (i = 1; i < 1000; i++)
		{
			if (!nickTaken(command, nickname))
				break ;
			nickname = command->getClient()->getUsername() + itostring(i, false);
		}
		return (nickname);
	}

	static std::string getModeStr(Command *command)
	{
		std::string result;
		int bitmode;

		bitmode = 0;
		if (!command->getMessage()->getBitmode().empty())
			bitmode = ft_stoi(command->getMessage()->getBitmode());

		if ((bitmode & 0x2) != 0)
			result += 'w';
		if ((bitmode & 0x4) != 0)
			result += 'i';
		return (result);
	}

	void USER(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();
		std::string msg;

		s->incCommandStats("USER");
		Debug::debug("USER");
		if (m->getMsgType() != IRC_USER)
		{
			command->replyNeedMoreParams("USER");
			return ;
		}
		c->setUsername(m->getUser());
		c->setRealname(m->getRealname());
		c->setNickname(makeTempNick(command));
		c->setRegistered(true);
		Debug::debug("CONNECTED", "");
		c->setStatus(Status::CONNECTED);
		command->reply(IRC::RPL_WELCOME, "Welcome to the Internet Relay Network " + c->getPrefix() + "!");
		command->reply(IRC::RPL_YOURHOST, "Your host is " + s->getHostname() + ", running version " + s->getVersion());
		command->reply(IRC::RPL_CREATED, "This server was created " + s->getCreationDate());
		command->reply(IRC::RPL_MYINFO, s->getHostname() + " " + s->getVersion() + " " + s->getAvailableUserModes() + " " + s->getAvailableChannelModes());

		msg = "LUSERS\r\n";
		s->executeCommand(c, msg);
		msg = "MOTD\r\n";
		s->executeCommand(c, msg);
		msg = "MODE " + c->getNickname() + " +";
		if (getModeStr(command).empty())
			msg += "w\r\n";
		else
			msg += getModeStr(command) + "\r\n";
		s->executeCommand(c, msg);
	}
}
