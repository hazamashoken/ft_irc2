/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   STATS.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 10:40:01 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 20:04:40 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../parsers/Wildcard.hpp"
# include <map>
namespace Commands {
	static void command_stats(Command *command)
	{
		Server *s = command->getServer();

		std::map<std::string, int>::const_iterator it;

		for (it = s->getCommandStats().begin(); it != s->getCommandStats().end(); it++)
			command->replyClient(IRC::RPL_STATSCOMMANDS, it->first + " " + itostring(it->second, false));
	}

	void STATS(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("STATS");
		Debug::debug("STATS");
		if (m->getMsgType() != IRC_STATS)
		{
			command->replyNeedMoreParams("STATS");
			return ;
		}

		if (m->size() == 3 && !Wildcard(m->param(2)).match(s->getHostname()))
		{
			command->replyClient(IRC::ERR_NOSUCHSERVER, ":Server to server not supported");
			return ;
		}

		if (m->size() == 1)
		{
			command->replyClient(IRC::RPL_ENDOFSTATS, "* :End of /STATS report");
			return ;
		}

		if (m->param(1) == "l")
			command->replyClient(IRC::RPL_ENDOFSTATS, "l :End of /STATS report");
		else if (m->param(1) == "m")
		{
			command_stats(command);
			command->replyClient(IRC::RPL_ENDOFSTATS, "m :End of /STATS report");
		}
		else if (m->param(1) == "o")
		{
			std::map<int, Client *>::const_iterator it;

			for (it = s->getOperators().begin(); it != s->getOperators().end(); it++)
				command->replyClient(IRC::RPL_STATSOLINE, "O * * " + it->second->getUsername());
			command->replyClient(IRC::RPL_ENDOFSTATS, "o :End of /STATS report");
		}
		else if (m->param(1) == "u")
		{
			command->replyClient(IRC::RPL_STATSUPTIME, ":Server Up " + s->getUptime());
			command->replyClient(IRC::RPL_ENDOFSTATS, "u :End of /STATS report");
		}
	}
}
