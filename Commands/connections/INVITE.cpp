/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 11:24:56 by abossel           #+#    #+#             */
/*   Updated: 2023/05/31 12:08:46 by abossel          ###   ########.fr       */
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
	void INVITE(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("INVITE");
		Debug::debug("INVITE");
		if (m->getMsgType() != IRC_INVITE)
		{
			command->replyNeedMoreParams("INVITE");
			return ;
		}

		// bug workaround for bitchx
		if (!m->getUnused().empty())
			return ;

		Channel *channel;
		Client *client;

		channel = s->getChannelByName(m->param(2));
		if (channel == NULL)
		{
			command->replyClient(IRC::ERR_NOSUCHCHANNEL, m->param(2) + " :No such channel");
			return ;
		}
		if (!channel->hasClient(c))
		{
			command->replyClient(IRC::ERR_NOTONCHANNEL, m->param(2) + " :You're not on that channel");
			return ;
		}
		if (!channel->isOperator(c))
		{
			command->replyClient(IRC::ERR_CHANOPRIVSNEEDED, m->param(2) + " :You're not channel operator");
			return ;
		}
		client = s->getClientByNickname(m->param(1));
		if (client == NULL)
		{
			command->replyClient(IRC::ERR_NOSUCHNICK, m->param(1) + " :No such nick/channel");
			return ;
		}
		if (channel->hasClient(client))
		{
			command->replyClient(IRC::ERR_USERONCHANNEL, m->param(1) + " " + m->param(2) + " :is already on channel");
			return ;
		}
		channel->addInvite(m->param(1));
		command->replyClient(IRC::RPL_INVITING, m->param(1) + " " + m->param(2));
		if (!channel->getKey().empty())
			command->replyOther(*client, IRC::RPL_INVITING, m->param(1) + " " + m->param(2) + " :" + channel->getKey());
		else
			command->replyOther(*client, IRC::RPL_INVITING, m->param(1) + " " + m->param(2));
	}
}
