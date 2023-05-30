/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SQUERY.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 08:47:58 by abossel           #+#    #+#             */
/*   Updated: 2023/05/28 15:10:36 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../parsers/Wildcard.hpp"
# include "../../parsers/Calculator.hpp"
namespace Commands {
	static std::string dtos(double num)
	{
		std::stringstream ss;

		ss << num;
		return (ss.str());
	}

	void SQUERY(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("SQUERY");
		Debug::debug("SQUERY");
		if (m->getMsgType() != IRC_SQUERY)
		{
			command->replyNeedMoreParams("SQUERY");
			return ;
		}
		if (m->param(1) != "calc")
		{
			command->replyClient(IRC::ERR_NORECIPIENT, ":No recipient given (SQUERY)");
			return ;
		}
		Calculator calc;
		if (!calc.set(m->param(2)))
		{
			command->replyFromServer("PRIVMSG " + c->getNickname() + " :CALC : Invalid input");
			return ;
		}
		command->replyFromServer("PRIVMSG " + c->getNickname() + " :CALC : " + calc.str());
		command->replyFromServer("PRIVMSG " + c->getNickname() + " :CALC = " + dtos(calc.calculate()));
	}
}
