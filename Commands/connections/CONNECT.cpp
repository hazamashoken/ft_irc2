/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CONNECT.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 12:07:37 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 19:44:11 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
namespace Commands {
	void CONNECT(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("CONNECT");
		Debug::debug("CONNECT");
		if (m->getMsgType() != IRC_CONNECT)
		{
			command->replyNeedMoreParams("CONNECT");
			return ;
		}
		command->replyClient(IRC::ERR_NOSUCHSERVER, ":Server to server not supported");
	}
}
