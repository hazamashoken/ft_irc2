/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LIST.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 11:08:21 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 19:50:37 by abossel          ###   ########.fr       */
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
	void LIST(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("LIST");
		Debug::debug("LIST");
		if (m->getMsgType() != IRC_LIST)
		{
			command->replyNeedMoreParams("LIST");
			return ;
		}

		if (m->size() == 3)
		{
			if (!Wildcard(m->param(2)).match(s->getHostname()))
			{
				command->replyClient(IRC::ERR_NOSUCHSERVER, ":Server to server not supported");
				return ;
			}
		}

		std::map<std::string, Channel *>::const_iterator ch_it;
		Channel *channel;
		Split sp;
		size_t i;

		command->replyClient(IRC::RPL_LISTSTART, "Channel :Users  Name");
		if (m->size() == 1)
		{
			// list all channels
			for (ch_it = s->getChannels().begin(); ch_it != s->getChannels().end(); ch_it++)
			{
				channel = ch_it->second;
				command->replyClient(IRC::RPL_LIST, channel->getName() + " "
									+ itostring(channel->nrVisible(), false) + " :"
									+ channel->getTopic());
			}
		}
		else
		{
			// list channels from the list
			sp.split(m->param(1), ',');
			for (i = 0; i < sp.size(); i++)
			{
				channel = s->getChannelByName(sp[i]);
				if (channel != NULL)
				{
					command->replyClient(IRC::RPL_LIST, channel->getName() + " "
										+ itostring(channel->nrVisible(), false) + " :"
										+ channel->getTopic());
				}
			}
		}
		command->replyClient(IRC::RPL_LISTEND, ":End of /LIST");
	}
}
