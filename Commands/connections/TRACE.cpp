/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TRACE.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/07 14:45:11 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 20:15:19 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../Channel.hpp"
# include "../../parsers/Wildcard.hpp"
# include "../../parsers/Split.hpp"
namespace Commands {
	void TRACE(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("TRACE");
		Debug::debug("TRACE");
		if (m->getMsgType() != IRC_TRACE)
		{
			command->replyNeedMoreParams("TRACE");
			return ;
		}
		command->replyClient(IRC::ERR_NOSUCHSERVER, ":Server to server not supported");
	}
}
