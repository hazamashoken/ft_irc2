/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TIME.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/30 12:14:36 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 20:13:21 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../parsers/Wildcard.hpp"
# include <ctime>
namespace Commands {
	static std::string time_now()
	{
		time_t t = std::time(NULL);
		std::string s = std::ctime(&t);

		// remove \n from time string
		if (!s.empty())
			s.erase(s.size() - 1);
		return (s);
	}

	void TIME(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("TIME");
		Debug::debug("TIME");
		if (m->getMsgType() != IRC_TIME)
		{
			command->replyNeedMoreParams("TIME");
			return ;
		}

		if ((m->size() == 2 && !Wildcard(m->param(2)).match(s->getHostname())))
		{
			command->replyClient(IRC::ERR_NOSUCHSERVER, ":Server to server not supported");
			return ;
		}

		command->replyClient(IRC::RPL_TIME, s->getHostname() + " " + time_now());
	}
}
