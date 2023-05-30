/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VERSION.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/30 14:03:10 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 20:20:46 by abossel          ###   ########.fr       */
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
	void VERSION(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("VERSION");
		Debug::debug("VERSION");
		if (m->getMsgType() != IRC_VERSION)
		{
			command->replyNeedMoreParams("VERSION");
			return ;
		}

		if (m->size() == 2 && !Wildcard(m->param(1)).match(s->getHostname()))
		{
			command->replyClient(IRC::ERR_NOSUCHSERVER, ":Server to server not supported");
		}

		command->replyClient(IRC::RPL_VERSION, s->getVersion() + " " + s->getHostname()
							+ " :BLIRC v" + s->getVersion());
	}
}
