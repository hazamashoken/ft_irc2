/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/22 13:10:31 by abossel           #+#    #+#             */
/*   Updated: 2023/05/29 21:03:35 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../Channel.hpp"
# include "../../parsers/Expression.hpp"
# include "../../parsers/Split.hpp"
namespace Commands {
	void PRIVMSG(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("PRIVMSG");
		Debug::debug("PRIVMSG");
		if (m->getMsgType() != IRC_PRIVMSG)
		{
			command->replyNeedMoreParams("PRIVMSG");
			return ;
		}

		if (m->param(2) == "")
		{
			command->replyClient(IRC::ERR_NOTEXTTOSEND, ":No text to send");
			return ;
		}
		
		Split recievers;
		Split::iterator r_it;
		Channel *channel;
		Client *client;

		// nickname expression
		Expression nicknameExp;
		nicknameExp.alpha().add("-[]\\`^{}");
		nicknameExp.exp(Expression().alnum().add("-[]\\`^{}"), 0, 8);

		// channel expression
		Expression channelExp;
		channelExp.exp(Expression().all("!").upper(5, 5).add("0123456789")).jmp();
		channelExp.any("#+&").con();
		channelExp.inv(std::string("\0\a\r\n ,:", 7), 1);
		channelExp.exp(Expression().all(":").inv(std::string("\0\a\r\n ,:", 7), 1), 0, 1);

		recievers.split(m->param(1), ',');
		for (r_it = recievers.begin(); r_it != recievers.end(); r_it++)
		{
			if (channelExp.match(*r_it))
			{
				channel = s->getChannelByName(*r_it);
				if (channel == NULL)
					command->replyClient(IRC::ERR_NOSUCHCHANNEL, *r_it + " :No such channel");
				else
					command->replyChannel(*channel, "PRIVMSG " + *r_it + " :" + m->param(2));
			}
			else if (nicknameExp.match(*r_it))
			{
				client = s->getClientByNickname(*r_it);
				if (client == NULL)
					command->replyClient(IRC::ERR_NOSUCHNICK, *r_it + " :No such nick/channel");
				else 
				{
					if (client->isAway())
						command->replyFromServer(IRC::RPL_AWAY, client->getNickname() + " :" + client->getAway());
					command->replyOther(*client, "PRIVMSG " + *r_it + " :" + m->param(2));
				}
			}
		}
	}
}
