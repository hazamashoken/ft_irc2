# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"

namespace Commands{
	void PING(Command *command)
	{
		Server *s = command->getServer();
		Message *m = command->getMessage();
		std::string message;

		s->incCommandStats("PING");
		Debug::debug("PING: " + m->getPrefix());
		if (m->getMsgType() != IRC_PING)
		{
			Debug::debug("PING: invalid message type");
			message = ":" + s->getHostname() + " " + itostring(IRC::ERR_NEEDMOREPARAMS, true)
						+ " * " + m->getNickname() + " :Invalid PING";
			Debug::debug("REPLY: ", message);
			command->reply(IRC::ERR_NEEDMOREPARAMS, "Invalid PING");
			return ;
		}
		message = "PONG";
		if (m->param(1) != "")
			message += " " + m->param(1);
		if (m->param(2) != "")
			message += " " + m->param(2);
		if (m->param(3) != "")
			message += " :" + m->param(3);
		Debug::debug("REPLY: ", message);
		command->reply(message);
	}
}
