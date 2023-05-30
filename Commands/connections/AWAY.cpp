/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AWAY.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 12:10:12 by abossel           #+#    #+#             */
/*   Updated: 2023/05/29 12:16:25 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
namespace Commands {
	void AWAY(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("AWAY");
		Debug::debug("AWAY");
		if (m->getMsgType() != IRC_AWAY)
		{
			command->replyNeedMoreParams("AWAY");
			return ;
		}

		if (m->param(1).empty())
		{
			c->removeAway();
			command->replyClient(IRC::RPL_UNAWAY, ":You are no longer marked as being away");
		}
		else
		{
			c->setAway(m->param(1));
			command->replyClient(IRC::RPL_NOWAWAY, ":You have been marked as being away");
		}
	}
}
