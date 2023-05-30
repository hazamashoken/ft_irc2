# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../utils/utils.hpp"
# include "../../Status.hpp"
# include "../../parsers/Split.hpp"
namespace Commands {

	void	MODE(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();
		Split modes;
		size_t i;

		s->incCommandStats("MODE");
		Debug::debug("MODE: " + m->getNickname());
		if (m->getMsgType() != IRC_MODE)
		{
			Debug::debug("MODE: invalid message type");
			std::string message = ":" + s->getHostname() + " " + itostring(IRC::ERR_NEEDMOREPARAMS, true) + " * " + m->getNickname() + " :Invalid MODE";
			Debug::debug("REPLY: ", message);
			command->reply(IRC::ERR_NEEDMOREPARAMS, "Invalid MODE");
			return ;
		}
		if (m->size() == 3)
		{
			Debug::debug("MODE: mode string " + m->param(2));
			modes.split(m->param(2), ' ');
			for (i = 0; i < modes.size(); i++)
			{
				// TODO check for operator mode permissions
				if (modes[i][0] == '+')
					c->addMode(modes[i][1]);
				if (modes[i][0] == '-')
					c->remMode(modes[i][1]);
			}
		}
		command->reply("MODE " + c->getNickname() + " :+" + c->getModes());
	}
}
