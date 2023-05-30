/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SQUIT.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 09:09:32 by abossel           #+#    #+#             */
/*   Updated: 2023/05/30 09:13:45 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
namespace Commands {
	void SQUIT(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("SQUIT");
		Debug::debug("SQUIT");
		if (m->getMsgType() != IRC_SQUIT)
		{
			command->replyNeedMoreParams("SQUIT");
			return ;
		}
		if (!c->isOperator())
		{
			command->replyClient(IRC::ERR_NOPRIVILEGES, ":Permission Denied- You're not an IRC operator");
			return ;
		}
		command->replyClient(IRC::ERR_NOSUCHSERVER, m->param(1) + " :No such server");
	}
}
