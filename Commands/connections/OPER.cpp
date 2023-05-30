/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   OPER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 09:51:44 by abossel           #+#    #+#             */
/*   Updated: 2023/05/30 13:37:40 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
namespace Commands {
	void OPER(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("OPER");
		Debug::debug("OPER");
		if (m->getMsgType() != IRC_OPER)
		{
			command->replyNeedMoreParams("OPER");
			return ;
		}
		// FIXME add another password for operator privileges
		if (m->param(2) != s->getOperatorPass())
		{
			command->replyClient(IRC::ERR_PASSWDMISMATCH, ":Password incorrect");
			return ;
		}

		Client *client;

		if (m->param(1) == c->getNickname())
		{
			c->setOperator(true);
			command->replyClient(IRC::RPL_YOUREOPER, ":You are now an IRC operator");
			command->replyClient("MODE +o " + c->getNickname());
			command->replyServer("MODE +o " + c->getNickname());
		}
		else
		{
			client = s->getClientByNickname(m->param(1));
			if (client == NULL)
			{
				command->replyClient(IRC::ERR_NOSUCHNICK, m->param(1) + " :No such nick/channel");
				return ;
			}
			client->setOperator(true);
			command->replyOther(*client, IRC::RPL_YOUREOPER, ":You are now an IRC operator");
			command->replyClient("MODE +o " + client->getNickname());
			command->replyServer("MODE +o " + client->getNickname());
		}
	}
}
