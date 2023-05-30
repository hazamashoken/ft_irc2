/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MOTD.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/30 12:34:47 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 19:53:16 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../parsers/Wildcard.hpp"
namespace Commands {
	void MOTD(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("MOTD");
		Debug::debug("MOTD");
		if (m->getMsgType() != IRC_MOTD)
		{
			command->replyNeedMoreParams("MOTD");
			return ;
		}

		if (m->size() == 1 || (m->size() == 2 && Wildcard(m->param(2)).match(s->getHostname())))
		{
			command->replyClient(IRC::RPL_MOTDSTART, ":- " + s->getHostname() + " Message of the day -");
			command->replyClient(IRC::RPL_MOTD, ":          :::      ::::::::");
			command->replyClient(IRC::RPL_MOTD, ":        :+:      :+:    :+:");
			command->replyClient(IRC::RPL_MOTD, ":      +:+ +:+         +:+");
			command->replyClient(IRC::RPL_MOTD, ":    +#+  +:+       +#+");
			command->replyClient(IRC::RPL_MOTD, ":  +#+#+#+#+#+   +#+");
			command->replyClient(IRC::RPL_MOTD, ":       #+#    #+#");
			command->replyClient(IRC::RPL_MOTD, ":      ###   ########.fr");
			command->replyClient(IRC::RPL_MOTD, ": ");
			command->replyClient(IRC::RPL_MOTD, ":    /\\_/\\            /\\");
			command->replyClient(IRC::RPL_MOTD, ":   = o_o =_______    \\ \\");
			command->replyClient(IRC::RPL_MOTD, ":    __^      __(  \\.__) )");
			command->replyClient(IRC::RPL_MOTD, ":(@)<_____>__(_____)____/");
			command->replyClient(IRC::RPL_ENDOFMOTD, ":End of /MOTD command");
		}
		else
		{
			command->replyClient(IRC::ERR_NOMOTD, ":Server to server not supported");
		}
	}
}
