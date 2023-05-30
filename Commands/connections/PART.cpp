/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PART.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 13:15:17 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 19:57:53 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../Channel.hpp"
# include "../../parsers/Split.hpp"
namespace Commands {
	void PART(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("PART");
		Debug::debug("PART");
		if (m->getMsgType() != IRC_PART)
		{
			command->replyNeedMoreParams("PART");
			return ;
		}

		std::map<std::string, Channel *>::const_iterator chan_it;
		std::string chan_name;

		Split channels;
		Channel *channel;
		size_t i;

		channels.split(m->param(1), ',');
		for (i = 0; i < channels.size(); i++)
		{
			channel = s->getChannelByName(channels[i]);
			if (channel == NULL)
			{
				command->replyClient(IRC::ERR_NOSUCHCHANNEL, channels[i] + " :No such channel");
				continue ;
			}
			if (!channel->hasClient(c))
			{
				command->replyClient(IRC::ERR_NOTONCHANNEL, channels[i] + " :You're not on that channel");
				continue ;
			}
			if (m->size() == 2)
				command->replyChannel(*channel, "PART :" + channels[i]);
			else
				command->replyChannel(*channel, "PART " + channels[i] + " :" + m->param(2));
			command->replyClient("PART :" + channels[i]);
			channel->removeOperator(c);
			c->partChannel(channels[i]);
		}
	}
}
