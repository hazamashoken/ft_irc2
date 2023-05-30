# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../utils/utils.hpp"
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

	void	NICK(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("NICK");
		Debug::debug("NICK: " + m->getNickname());
		if (m->getMsgType() != IRC_NICK)
		{
			Debug::debug("NICK: invalid message type");
			std::string message = s->getHostname() + " " + itostring(IRC::ERR_NONICKNAMEGIVEN, true) + " * " + m->getNickname() + " :Invalid Nickname";
			Debug::debug("REPLY: ", message);
			if (c->getStatus() != Status::REGISTER)
				c->appendSendBuffer(":" + message);
			else
				command->reply(IRC::ERR_ERRONEUSNICKNAME, "Erroneus nickname");
			return ;
		}
		const std::string &nickname = m->getNickname();
		if (nickTaken(command, nickname))
			command->reply(IRC::ERR_NICKNAMEINUSE, nickname + " :Nickname is already in use");
		else
		{
			command->replyClient("NICK :" + nickname);
			c->addPrevNick(c->getNickname());
			c->setNickname(nickname);
			Debug::debug("Nickname changed to " + nickname);
		}
	}
}
