# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../Channel.hpp"

namespace Commands {
	static void part_all_channels(Command *command)
	{
		std::map<std::string, Channel*>::const_iterator ch_it;
		Server *s = command->getServer();
		Client *c = command->getClient();
		Channel *channel;
		std::string msg;

		ch_it = s->getChannels().begin();
		while (ch_it != s->getChannels().end())
		{
			channel = ch_it->second;
			if (channel->hasClient(c))
			{
				msg = "PART " + channel->getName() + "\r\n";
				ch_it++; // increment incase the channel is deleted
				s->executeCommand(c, msg);
			}
			else
				ch_it++;
		}
	}

	void QUIT(Command *command)
	{
		command->getServer()->incCommandStats("QUIT");
		Debug::debug("QUIT: " + command->getMessage()->getInfo());
		if (command->getMessage()->getInfo().size() > 1)
			command->getClient()->appendSendBuffer(":" + command->getClient()->getPrefix()
													+ " " + command->getMessage()->getMsgStr());
		part_all_channels(command);
		command->getClient()->setStatus(Status::DELETE);
	}
}
