/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 10:01:15 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 20:14:05 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../Channel.hpp"
namespace Commands {
	void TOPIC(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("TOPIC");
		Debug::debug("TOPIC");
		if (m->getMsgType() != IRC_TOPIC)
		{
			command->replyNeedMoreParams("TOPIC");
			return ;
		}

		Channel *channel = s->getChannelByName(m->param(1));

		if (channel == NULL || !channel->hasClient(c))
		{
			command->replyClient(IRC::ERR_NOTONCHANNEL, m->param(1) + " :You're not on that channel");
			return ;
		}
		if (m->size() == 2)
		{
			if (channel->getTopic().empty())
				command->replyClient(IRC::RPL_NOTOPIC, channel->getName() + " :No topic is set");
			else
				command->replyClient(IRC::RPL_TOPIC, channel->getName() + " :" + channel->getTopic());
		}
		else if (m->size() == 3)
		{
			Debug::debug("TOPIC topic: " + m->param(2));
			if (!channel->isOperator(c))
			{
				command->replyClient(IRC::ERR_CHANOPRIVSNEEDED, channel->getName() + " :You're not channel operator");
				return ;
			}
			if (m->param(2) == ":")
			{
				channel->setTopic("");
				command->replyChannel(*channel, IRC::RPL_NOTOPIC, channel->getName() + " :No topic is set");
			}
			else
			{
				channel->setTopic(m->param(2));
				command->replyChannel(*channel, IRC::RPL_TOPIC, channel->getName() + " :" + channel->getTopic());
			}
		}
	}
}
