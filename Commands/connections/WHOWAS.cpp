/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WHOWAS.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 09:59:09 by abossel           #+#    #+#             */
/*   Updated: 2023/05/26 08:09:25 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../parsers/Split.hpp"
# include "../../parsers/Wildcard.hpp"
# include <sstream>
namespace Commands {
	static int ston(const std::string &str)
	{
		std::stringstream ss;
		int num;

		ss << str;
		num = 0;
		ss >> num;
		return (num);
	}

	void WHOWAS(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		std::map<int, Client*>::const_iterator client_it;
		std::vector<std::string>::const_iterator oldnick_it;
		Wildcard w;
		Split nicks;
		int count;
		int limit;
		size_t i;

		s->incCommandStats("WHOWAS");
		Debug::debug("WHOWAS");
		if (m->getMsgType() != IRC_WHOWAS)
		{
			command->replyNeedMoreParams("WHOWAS");
			return ;
		}
		if (m->param(3) != "")
		{
			w.set(m->param(3));
			if (!w.match(s->getHostname()))
				command->replyClient(IRC::ERR_NOSUCHSERVER, ":Server to server not supported");
			return ;
		}
		limit = INT_MAX;
		if (m->param(2) != "")
			limit = ston(m->param(2));
		nicks.split(m->param(1), ',');
		for (i = 0; i < nicks.size(); i++)
		{
			count = 0;
			client_it = s->getClients().begin();
			while (client_it != s->getClients().end())
			{
				oldnick_it = client_it->second->getPrevNicks().begin();
				while (oldnick_it != client_it->second->getPrevNicks().end())
				{
					if (*oldnick_it == nicks[i])
					{
						command->replyClient(IRC::RPL_WHOWASUSER, nicks[i] + " "
											+ client_it->second->getUsername() + " "
											+ client_it->second->getHostname() + " * :"
											+ client_it->second->getRealname());
						count++;
					}
					oldnick_it++;
				}
				if (count >= limit)
					break ;
				client_it++;
			}
			if (count == 0)
				command->replyClient(IRC::ERR_WASNOSUCHNICK, nicks[i] + " :There was no such nickname");
			command->replyClient(IRC::RPL_ENDOFWHOWAS, nicks[i] + " :End of WHOWAS");
		}
	}
}
