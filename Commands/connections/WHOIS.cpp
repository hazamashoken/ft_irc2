/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WHOIS.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 11:13:52 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 20:23:22 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../parsers/Wildcard.hpp"
# include "../../parsers/Split.hpp"
# include "../../Channel.hpp"
namespace Commands {
	static void whois_output(Command *command, Client *client)
	{
		std::map<std::string, Channel *>::const_iterator it;
		Server *s = command->getServer();
		std::string reply;

		reply = client->getNickname() + " " + client->getUsername() + " "
				+ client->getHostname() + " * :" + client->getRealname();
		command->replyClient(IRC::RPL_WHOISUSER, reply);
		reply = client->getNickname() + " " + client->getServername()
				+ " :BLIRC v" + s->getVersion();
		command->replyClient(IRC::RPL_WHOISSERVER, reply);

		if (client->isOperator())
			command->replyClient(IRC::RPL_WHOISOPERATOR, client->getNickname()
								+ " :is an IRC operator");
		for (it = s->getChannels().begin(); it != s->getChannels().end(); it++)
		{
			if (it->second->hasClient(client))
			{
				reply = client->getNickname() + " :{";
				if (client->isOperator())
					reply += "@";
				reply += it->first + " }";
				command->replyClient(IRC::RPL_WHOISCHANNELS, reply);
			}
		}
		command->replyClient(IRC::RPL_ENDOFWHOIS, client->getNickname() + " :End of /WHOIS list");
	}

	void WHOIS(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		std::map<int, Client*>::const_iterator it;
		Wildcard w;
		Split masks;
		size_t i;
		bool found;

		s->incCommandStats("WHOIS");
		Debug::debug("WHOIS");
		if (m->getMsgType() != IRC_WHOIS)
		{
			command->replyNeedMoreParams("WHOIS");
			return ;
		}
		if (m->param(1) != "")
		{
			w.set(m->param(1));
			if (!w.match(s->getHostname()))
				command->replyClient(IRC::ERR_NOSUCHSERVER, ":Server to server not supported");
			return ;
		}
		found = false;
		masks.split(m->param(2), ',');
		for (i = 0; i < masks.size(); i++)
		{
			w.set(masks[i]);
			for (it = s->getClients().begin(); it != s->getClients().end(); it++)
			{
				if (w.match(it->second->getNickname()))
				{
					whois_output(command, it->second);
					found = true;
				}
			}
			if (!found)
				command->replyClient(IRC::ERR_NOSUCHNICK, m->param(2) + " :No such nick/channel");
		}
	}
}
