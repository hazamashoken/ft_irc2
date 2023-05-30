/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ADMIN.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/07 10:11:26 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 19:46:09 by abossel          ###   ########.fr       */
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
	void ADMIN(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("ADMIN");
		Debug::debug("ADMIN");
		if (m->getMsgType() != IRC_ADMIN)
		{
			command->replyNeedMoreParams("ADMIN");
			return ;
		}
		if (m->size() == 1 || (m->size() == 2 && Wildcard(m->param(1)).match(s->getHostname())))
		{
			command->replyClient(IRC::RPL_ADMINME, s->getHostname() + " :Administrative info");
			command->replyClient(IRC::RPL_ADMINLOC1, ":Location: Lat Krabang, Bangkok, Thailand");
			command->replyClient(IRC::RPL_ADMINLOC2, ":School: 42 Bangkok, KMITL");
			command->replyClient(IRC::RPL_ADMINEMAIL, ":Email: abossel@student.42bangkok.com");
		}
		else
		{
			command->replyClient(IRC::ERR_NOSUCHSERVER, ":Server to server not supported");
		}
	}
}
