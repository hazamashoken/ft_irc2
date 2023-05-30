/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SERVICE.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 23:28:37 by abossel           #+#    #+#             */
/*   Updated: 2023/05/28 23:29:15 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
namespace Commands {
	void SERVICE(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("SERVICE");
		Debug::debug("SERVICE");
		if (m->getMsgType() != IRC_SERVICE)
		{
			command->replyNeedMoreParams("SERVICE");
			return ;
		}
		command->replyClient(IRC::ERR_NOSUCHSERVER, ":Server to server not supported");
	}
}
