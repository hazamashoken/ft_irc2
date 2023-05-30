/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SERVLIST.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 08:40:33 by abossel           #+#    #+#             */
/*   Updated: 2023/05/28 15:16:17 by abossel          ###   ########.fr       */
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
	void SERVLIST(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("SERVLIST");
		Debug::debug("SERVLIST");
		if (m->getMsgType() != IRC_SERVLIST)
		{
			command->replyNeedMoreParams("SERVLIST");
			return ;
		}
		command->replyFromGeneric("calc", IRC::RPL_SERVLIST, "calc * * * 0 :Calculator (ex. /squery calc 1 + 1)");
		command->replyFromGeneric("calc", IRC::RPL_SERVLISTEND, ":End of /SERVLIST list");
	}
}
