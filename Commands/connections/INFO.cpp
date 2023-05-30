/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INFO.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/07 10:54:57 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 19:44:31 by abossel          ###   ########.fr       */
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
	void INFO(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("INFO");
		Debug::debug("INFO");
		if (m->getMsgType() != IRC_INFO)
		{
			command->replyNeedMoreParams("INFO");
			return ;
		}

		if (m->size() == 1 || (m->size() == 2 && Wildcard(m->param(1)).match(s->getHostname())))
		{
			command->replyClient(IRC::RPL_INFO, ":BLIRC v" + s->getVersion());
			command->replyClient(IRC::RPL_INFO, ":Compile date: " + std::string(__DATE__)
									+ " " + std::string(__TIME__));
			command->replyClient(IRC::RPL_INFO, ":Started: " + s->getCreationDate());
			command->replyClient(IRC::RPL_INFO, ":Uptime: " + s->getUptime());
			command->replyClient(IRC::RPL_ENDOFINFO, ":End of /INFO list");
		}
		else
		{
			command->replyClient(IRC::ERR_NOSUCHSERVER, ":Server to server not supported");
		}
	}
}
