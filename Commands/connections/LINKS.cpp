/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LINKS.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 12:05:33 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 19:49:07 by abossel          ###   ########.fr       */
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
	void LINKS(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("LINKS");
		Debug::debug("LINKS");
		if (m->getMsgType() != IRC_LINKS)
		{
			command->replyNeedMoreParams("LINKS");
			return ;
		}

		command->replyClient(IRC::RPL_LINKS, s->getHostname() + " " + s->getHostname()
								+ " :0 BLIRC v" + s->getVersion());
		command->replyClient(IRC::RPL_ENDOFLINKS, s->getHostname() + " :End of /LINKS list");
	}
}
