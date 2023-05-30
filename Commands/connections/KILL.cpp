/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KILL.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 12:34:21 by abossel           #+#    #+#             */
/*   Updated: 2023/05/26 17:35:34 by abossel          ###   ########.fr       */
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
	void KILL(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("KILL");
		Debug::debug("KILL");
		if (m->getMsgType() != IRC_KILL)
		{
			command->replyNeedMoreParams("KILL");
			return ;
		}

		Client *client;

		if (!c->isOperator())
		{
			command->replyClient(IRC::ERR_NOPRIVILEGES, ":Permission Denied- You're not an IRC operator");
			return ;
		}
		client = s->getClientByUsername(m->param(1));
		if (client == NULL)
		{
			command->replyClient(IRC::ERR_NOSUCHNICK, m->param(1) + " :No such nick/channel");
			return ;
		}
		if (m->size() == 3)
			command->replyOther(*client, "KILL " + client->getUsername() + " " + m->param(2));
		else
			command->replyOther(*client, "KILL " + client->getUsername());
		c->setStatus(Status::DELETE);
		//s->disconnectClient(client);
	}
}
