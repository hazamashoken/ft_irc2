/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ERROR.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 12:30:39 by abossel           #+#    #+#             */
/*   Updated: 2023/05/29 12:31:21 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
namespace Commands {
	void ERROR(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats(m->param(0));
		Debug::debug("ERROR");
		// if (m->getMsgType() != IRC_UNSUPPORTED)
		// {
		// 	command->replyNeedMoreParams("UNSUPPORTED");
		// 	return ;
		// }
	}
}
