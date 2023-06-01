/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/27 09:33:08 by abossel           #+#    #+#             */
/*   Updated: 2023/06/01 08:28:40 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Split.hpp"
#include <sstream>
#include <iostream>

#define IRC_SPECIAL std::string("-[]\\`^{}")
#define IRC_TARGET std::string("#$!-.!@*?\\")
#define IRC_NONUSER std::string("\0\r\n @", 5)
#define IRC_NONPASS std::string("\0\r\n \t", 5)
#define IRC_NONPARAM std::string("\0\r\n :", 5)
#define IRC_NONREAL std::string("\0\r\n:", 4)
#define IRC_NONINFO std::string("\0\r\n", 3)
#define IRC_NONCHANNEL std::string("\0\a\r\n ,:", 7)

#define IRC_MODE_TOG std::string("iswofck")
#define IRC_MODE_ALLOWED std::string("io")
#define IRC_CMODE_GIV std::string("ov")
#define IRC_CMODE_TOG std::string("psitnm")
#define IRC_CMODE_SET std::string("lbk")

Message::Message()
{
	_messageType = IRC_INVALID;
	_commandStr.assign("INVALID");
	_params = new MessageParam(*this);
	expressionInit();
	messageInit();
}

Message::~Message()
{
	delete _params;
}

void Message::expressionInitHostaddr()
{
	Expression ip4byte;
	ip4byte.exp(Expression().any("2").any("5").any("012345")).jmp();
	ip4byte.exp(Expression().any("2").any("01234").digit()).jmp();
	ip4byte.exp(Expression().any("01").digit(2, 2)).jmp();
	ip4byte.digit(1, 2);

	Expression ip4addr;
	ip4addr.exp(ip4byte).any(".").exp(ip4byte).any(".").exp(ip4byte).any(".").exp(ip4byte);

	Expression ip6pre;
	ip6pre.xdigit(1, 4).any(":");
	Expression ip6suf;
	ip6suf.any(":").xdigit(1, 4);

	Expression ip6pre0;
	ip6pre0.any("0", 1, 4).any(":");
	Expression ip6suf0;
	ip6suf0.any(":").any("0", 1, 4);
	Expression ip6ffff;
	ip6ffff.any(":").any("fF", 4, 4).any(":");

	// IPv6 addresses
	Expression ip6addr;

	// IPv4 mapped addresses 0:0:0:0:0:ffff:a.b.c.d
	ip6addr.exp(Expression().exp(ip6pre0, 4, 4).any("0", 1, 4).exp(ip6ffff).exp(ip4addr)).jmp();
	ip6addr.exp(Expression().exp(ip6pre0, 1, 4).exp(ip6ffff).exp(ip4addr)).jmp();
	ip6addr.exp(Expression().any(":").exp(ip6suf0, 0, 4).exp(ip6ffff).exp(ip4addr)).jmp();

	// IPv4 translated addresses 0:0:0:0:ffff:0:a.b.c.d
	ip6addr.exp(Expression().exp(ip6pre0, 3, 3).any("0", 1, 4).exp(ip6ffff).exp(ip6pre0).exp(ip4addr)).jmp();
	ip6addr.exp(Expression().exp(ip6pre0, 1, 3).exp(ip6ffff).exp(ip6pre0).exp(ip4addr)).jmp();
	ip6addr.exp(Expression().any(":").exp(ip6suf0, 0, 3).exp(ip6ffff).exp(ip6pre0).exp(ip4addr)).jmp();

	// IPv4 standard addresses 0:0:0:0:0:0:0:0
	ip6addr.exp(Expression().exp(ip6pre, 7, 7).xdigit(1, 4)).jmp();
	ip6addr.exp(Expression().exp(ip6pre, 6, 6).exp(ip6suf, 1, 1)).jmp();
	ip6addr.exp(Expression().exp(ip6pre, 5, 5).exp(ip6suf, 1, 2)).jmp();
	ip6addr.exp(Expression().exp(ip6pre, 4, 4).exp(ip6suf, 1, 3)).jmp();
	ip6addr.exp(Expression().exp(ip6pre, 3, 3).exp(ip6suf, 1, 4)).jmp();
	ip6addr.exp(Expression().exp(ip6pre, 2, 2).exp(ip6suf, 1, 5)).jmp();
	ip6addr.exp(Expression().exp(ip6pre, 1, 1).exp(ip6suf, 1, 6)).jmp();
	ip6addr.exp(Expression().any(":").exp(ip6suf, 1, 7)).jmp();
	ip6addr.exp(Expression().exp(ip6pre, 1, 7).any(":")).jmp();

	// special case ::
	ip6addr.all("::");

	_hostaddrExp = Expression(_hostaddrStr);
	_hostaddrExp.exp(ip4addr).jmp().exp(ip6addr);
}

void Message::expressionInit()
{
	// nickname expression
	_nicknameExp = Expression(_nicknameStr);
	_nicknameExp.alpha().add(IRC_SPECIAL).exp(Expression().alnum().add(IRC_SPECIAL).add("-"), 0, 15);

	// username expression
	_userExp = Expression(_userStr);
	_userExp.inv(IRC_NONUSER, 1);

	// hostname expression
	Expression shortnameExp;
	shortnameExp.alnum(1).exp(Expression().any("-", 0).alnum(1), 0);
	_hostnameExp = Expression(_hostnameStr);
	_hostnameExp.exp(shortnameExp).exp(Expression().any(".").exp(shortnameExp, 1), 0);

	// hostaddr expression
	expressionInitHostaddr();

	// host expression
	_hostExp = Expression(_hostStr);
	_hostExp.exp(_hostaddrExp).jmp().exp(_hostnameExp);

	// servername expression
	_servernameExp = Expression(_hostnameExp, _servernameStr);

	// prefix expression
	_prefixExp = Expression(_prefixStr);
	_prefixExp.exp(Expression().exp(_nicknameExp).any("!").exp(_userExp).any("@").exp(_hostExp)).jmp();
	_prefixExp.exp(Expression().exp(_nicknameExp).any("@").exp(_hostExp)).jmp();
	_prefixExp.exp(_nicknameExp).jmpe();
	_prefixExp.exp(_servernameExp);

	// password expression
	_passwordExp = Expression(_passwordStr);
	_passwordExp.inv(IRC_NONPASS, 1);

	// bitmode expression
	_bitmodeExp = Expression(_bitmodeStr);
	_bitmodeExp.digit();

	// unused expression
	_unusedExp = Expression(_unusedStr);
	_unusedExp.inv(IRC_NONPARAM, 1);

	// realname expression
	_realnameExp = Expression(_realnameStr);
	_realnameExp.inv(IRC_NONREAL, 1);

	// mode expression
	_userModeExp = Expression(_userModeStr);
	_userModeExp.any("+-").any(IRC_MODE_TOG, 1);

	// mode expression
	_modeExp = Expression(_modeStr);
	_modeExp.any("+-", 0, 1).any(IRC_MODE_TOG, 1);
	_modeExp.exp(Expression().all(" ").any("+-", 0, 1).any(IRC_MODE_TOG, 1), 0);

	// wildname expression
	_targetExp = Expression(_targetStr);
	_targetExp.alnum(1).add(IRC_TARGET);

	// info expression
	_infoExp = Expression(_infoStr);
	_infoExp.inv(IRC_NONINFO, 1);

	// channel expression
	_channelExp = Expression(_channelStr);
	_channelExp.exp(Expression().all("!").upper(5, 5).add("0123456789")).jmp();
	_channelExp.any("#+&").con();
	_channelExp.inv(IRC_NONCHANNEL, 1);
	_channelExp.exp(Expression().all(":").inv(IRC_NONCHANNEL, 1), 0, 1);

	// channel list expression
	_channelListExp = Expression(_channelListStr);
	_channelListExp.all("0").jmp();
	_channelListExp.exp(_channelExp).exp(Expression().all(",").exp(_channelExp), 0);

	// key list expression
	_keyListExp = Expression(_keyListStr);
	_keyListExp.exp(_passwordExp, 1).exp(Expression().all(",").exp(_passwordExp), 0);

	// channel mode expression
	_channelModeExp = Expression(_channelModeStr);
	_channelModeExp.exp(Expression().any("+-").any(IRC_CMODE_GIV + IRC_CMODE_TOG + IRC_CMODE_SET, 1), 1);
	_channelModeExp.exp(Expression().all(" ").exp(_targetExp), 0);

	// channel mode list expression
	_channelModeListExp = Expression(_channelModeListStr);
	_channelModeListExp.exp(
		Expression().exp(_channelModeExp).exp(
			Expression().all(" ").exp(_channelModeExp), 0)).jmp();
	_channelModeListExp.all("b");

	// topic info expression
	_topicExp = Expression(_topicStr);
	_topicExp.all(":").exp(_infoExp, 0, 1);

	// user list expression
	_userListExp = Expression(_userListStr);
	_userListExp.exp(_userExp);
	_userListExp.exp(Expression().all(",").exp(_userExp), 0);

	// msgto expression
	_msgtoExp = Expression(_msgtoStr);
	_msgtoExp.exp(_channelExp).jmp();
	_msgtoExp.exp(Expression().exp(_userExp).all("%").exp(_hostExp).all("@").exp(_servernameExp)).jmp();
	_msgtoExp.exp(Expression().exp(_userExp).all("@").exp(_servernameExp)).jmp();
	_msgtoExp.exp(Expression().exp(_userExp).all("%").exp(_hostExp)).jmp();
	_msgtoExp.exp(Expression().exp(_nicknameExp).all("!").exp(_userExp).all("@").exp(_hostExp)).jmp();
	_msgtoExp.exp(Expression().exp(_nicknameExp)).jmp();
	_msgtoExp.exp(Expression().exp(_targetExp));

	// stat query expression
	_statQueryExp = Expression(_statQueryStr);
	_statQueryExp.any("lmou");

	// port number expression
	_portExp = Expression(_portStr);
	_portExp.exp(Expression().any("123456").digit(4, 4)).jmp();
	_portExp.digit(1, 4);

	// who option expression
	_whoOptExp = Expression(_whoOptStr);
	_whoOptExp.all("o");

	// target list expression
	_targetListExp = Expression(_targetListStr);
	_targetListExp.exp(_targetExp).exp(Expression().all(",").exp(_targetExp), 0);

	// nickname list expression
	_nicknameListExp = Expression(_nicknameListStr);
	_nicknameListExp.exp(_nicknameExp).exp(Expression().all(",").exp(_nicknameExp), 0);

	// number expression
	_numberExp = Expression(_numberStr);
	_numberExp.any("+-", 0, 1).digit(1);
}

void Message::messageInit()
{
	// PASS message
	_passMsgExp = Expression(_messageStr);
	_passMsgExp.all("PASS ").tag(IRC_TAG_CMD).exp(_passwordExp).all("\r\n").tag(IRC_TAG_END);

	// NICK message
	_nickMsgExp = Expression(_messageStr);
	_nickMsgExp.all("NICK ").tag(IRC_TAG_CMD).exp(_nicknameExp).all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// USER message
	_userMsgExp = Expression(_messageStr);
	_userMsgExp.all("USER ").tag(IRC_TAG_CMD).exp(_userExp).all(" ");
	_userMsgExp.exp(Expression().exp(_bitmodeExp).all(" ").exp(_unusedExp)).jmp();
	_userMsgExp.exp(Expression().exp(_userModeExp).all(" ").exp(_unusedExp)).jmp();
	_userMsgExp.exp(Expression().exp(_hostnameExp).all(" ").exp(_servernameExp)).con();
	_userMsgExp.all(" :").exp(_realnameExp).all("\r\n").tag(IRC_TAG_END);

	// OPER message
	_operMsgExp = Expression(_messageStr);
	_operMsgExp.all("OPER ").tag(IRC_TAG_CMD).exp(_userExp);
	_operMsgExp.all(" ").exp(_passwordExp).all("\r\n").tag(IRC_TAG_END);

	// MODE message
	_modeMsgExp = Expression(_messageStr);
	_modeMsgExp.all("MODE ").exp(_nicknameExp).all(" ");
	_modeMsgExp.exp(_modeExp).tag(IRC_TAG_CMD).all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// SERVICE message
	_serviceMsgExp = Expression(_messageStr);
	_serviceMsgExp.all("SERVICE ").tag(IRC_TAG_CMD).exp(_nicknameExp);
	_serviceMsgExp.all(" ").exp(_unusedExp);
	_serviceMsgExp.all(" ").exp(_targetExp);
	_serviceMsgExp.all(" ").exp(_unusedExp);
	_serviceMsgExp.all(" ").exp(_unusedExp);
	_serviceMsgExp.all(" :").exp(_infoExp);
	_serviceMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// QUIT message
	_quitMsgExp = Expression(_messageStr);
	_quitMsgExp.all("QUIT ").tag(IRC_TAG_CMD).all(":").exp(_infoExp).all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// SQUIT message
	_squitMsgExp = Expression(_messageStr);
	_squitMsgExp.all("SQUIT ").tag(IRC_TAG_CMD).exp(_servernameExp);
	_squitMsgExp.all(" :").exp(_infoExp).all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// channel JOIN message
	_joinMsgExp = Expression(_messageStr);
	_joinMsgExp.all("JOIN ").tag(IRC_TAG_CMD).exp(_channelListExp);
	_joinMsgExp.exp(Expression().all(" ").exp(_keyListExp), 0, 1);
	_joinMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// channel PART message
	_partMsgExp = Expression(_messageStr);
	_partMsgExp.all("PART ").tag(IRC_TAG_CMD).exp(_channelListExp);
	_partMsgExp.exp(Expression().all(" :").exp(_infoExp), 0, 1);
	_partMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// channel MODE message
	_cmodeMsgExp = Expression(_messageStr);
	_cmodeMsgExp.all("MODE ").exp(_channelExp).tag(IRC_TAG_CMD);
	_cmodeMsgExp.exp(Expression().all(" ").all(":", 0, 1).exp(_channelModeListExp), 0, 1);
	_cmodeMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// channel TOPIC message
	_topicMsgExp = Expression(_messageStr);
	_topicMsgExp.all("TOPIC").tag(IRC_TAG_CMD).all(" ").exp(_channelExp);
	_topicMsgExp.exp(Expression().all(" ").exp(_topicExp), 0, 1);
	_topicMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// channel NAMES message
	_namesMsgExp = Expression(_messageStr);
	_namesMsgExp.all("NAMES").tag(IRC_TAG_CMD).exp(
		Expression().all(" ").exp(_channelListExp).exp(
			Expression().all(" ").exp(_targetExp), 0, 1), 0, 1);
	_namesMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// channel LIST message
	_listMsgExp = Expression(_messageStr);
	_listMsgExp.all("LIST").tag(IRC_TAG_CMD).exp(
		Expression().all(" ").exp(_channelListExp).exp(
			Expression().all(" ").exp(_targetExp), 0, 1), 0, 1);
	_listMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);
	
	// channel INVITE message
	_inviteMsgExp = Expression(_messageStr);
	_inviteMsgExp.all("INVITE ").tag(IRC_TAG_CMD);
	_inviteMsgExp.exp(Expression().exp(_nicknameExp).all(" ").exp(_channelExp).all(" :").exp(_infoExp)).jmp();
	_inviteMsgExp.exp(Expression().exp(_nicknameExp).all(" ").exp(_channelExp)).jmp();
	_inviteMsgExp.exp(Expression().exp(_channelExp).all(" ").exp(_unusedExp).all(" :").exp(_infoExp)).jmp();
	_inviteMsgExp.exp(Expression().exp(_channelExp).all(" ").exp(_unusedExp)).con();
	_inviteMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// channel KICK message
	_kickMsgExp = Expression(_messageStr);
	_kickMsgExp.all("KICK ").tag(IRC_TAG_CMD).exp(_channelListExp);
	_kickMsgExp.all(" ").exp(_userListExp);
	_kickMsgExp.exp(Expression().all(" :").exp(_infoExp, 0, 1), 0, 1);
	_kickMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// messages PRIVMSG message
	_privmsgMsgExp = Expression(_messageStr);
	_privmsgMsgExp.all("PRIVMSG ").tag(IRC_TAG_CMD).exp(_msgtoExp);
	_privmsgMsgExp.all(" :").exp(_infoExp).all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// messages NOTICE message
	_noticeMsgExp = Expression(_messageStr);
	_noticeMsgExp.all("NOTICE ").tag(IRC_TAG_CMD).exp(_msgtoExp);
	_noticeMsgExp.all(" :").exp(_infoExp).all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// server MOTD message
	_motdMsgExp = Expression(_messageStr);
	_motdMsgExp.all("MOTD").tag(IRC_TAG_CMD);
	_motdMsgExp.exp(Expression().all(" ").exp(_targetExp), 0, 1);
	_motdMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// server LUSERS message
	_lusersMsgExp = Expression(_messageStr);
	_lusersMsgExp.all("LUSERS").tag(IRC_TAG_CMD).exp(
		Expression().all(" ").exp(_targetExp).exp(
			Expression().all(" ").exp(_servernameExp), 0, 1), 0, 1);
	_lusersMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// server VERSION message
	_versionMsgExp = Expression(_messageStr);
	_versionMsgExp.all("VERSION").tag(IRC_TAG_CMD);
	_versionMsgExp.exp(Expression().all(" ").exp(_targetExp), 0, 1);
	_versionMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// server STATS message
	_statsMsgExp = Expression(_messageStr);
	_statsMsgExp.all("STATS").tag(IRC_TAG_CMD).exp(
		Expression().all(" ").exp(_statQueryExp).exp(
			Expression().all(" ").exp(_targetExp).exp(
				Expression().all(" ").exp(_unusedExp), 0), 0, 1), 0, 1);
	_statsMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// server LINKS message
	_linksMsgExp = Expression(_messageStr);
	_linksMsgExp.all("LINKS").tag(IRC_TAG_CMD);
	_linksMsgExp.exp(Expression().all(" ").exp(_servernameExp).all(" ").exp(_targetExp)).jmp();
	_linksMsgExp.exp(Expression().all(" ").exp(_targetExp)).jmp().con();
	_linksMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// server TIME message
	_timeMsgExp = Expression(_messageStr);
	_timeMsgExp.all("TIME").tag(IRC_TAG_CMD);
	_timeMsgExp.exp(Expression().all(" ").exp(_targetExp), 0, 1);
	_timeMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// server CONNECT message
	_connectMsgExp = Expression(_messageStr);
	_connectMsgExp.all("CONNECT ").tag(IRC_TAG_CMD);
	_connectMsgExp.exp(_servernameExp).all(" ").exp(_portExp);
	_connectMsgExp.exp(Expression().all(" ").exp(_targetExp), 0, 1);
	_connectMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// server TRACE message
	_traceMsgExp = Expression(_messageStr);
	_traceMsgExp.all("TRACE").tag(IRC_TAG_CMD);
	_traceMsgExp.exp(Expression().all(" ").exp(_targetExp), 0, 1);
	_traceMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// server ADMIN message
	_adminMsgExp = Expression(_messageStr);
	_adminMsgExp.all("ADMIN").tag(IRC_TAG_CMD);
	_adminMsgExp.exp(Expression().all(" ").exp(_targetExp), 0, 1);
	_adminMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// server INFO message
	_infoMsgExp = Expression(_messageStr);
	_infoMsgExp.all("INFO").tag(IRC_TAG_CMD);
	_infoMsgExp.exp(Expression().all(" ").exp(_targetExp), 0, 1);
	_infoMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// service SERVLIST message
	_servlistMsgExp = Expression(_messageStr);
	_servlistMsgExp.all("SERVLIST").tag(IRC_TAG_CMD).exp(
		Expression().all(" ").exp(_targetExp).exp(
			Expression().all(" ").exp(_infoExp), 0, 1), 0, 1);
	_servlistMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// service SQUERY message
	_squeryMsgExp = Expression(_messageStr);
	_squeryMsgExp.all("SQUERY ").tag(IRC_TAG_CMD).exp(_msgtoExp);
	_squeryMsgExp.all(" :").exp(_infoExp).all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// user WHO message
	_whoMsgExp = Expression(_messageStr);
	_whoMsgExp.all("WHO").tag(IRC_TAG_CMD).exp(
		Expression().all(" ").exp(_targetExp).exp(
			Expression().all(" ").exp(_whoOptExp), 0, 1), 0, 1);
	_whoMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// user WHOIS message
	_whoisMsgExp = Expression(_messageStr);
	_whoisMsgExp.all("WHOIS ").tag(IRC_TAG_CMD);
	_whoisMsgExp.exp(Expression().exp(_targetListExp).all(" ", 0).all("\r\n")).jmp();
	_whoisMsgExp.exp(Expression().exp(_servernameExp).all(" ").exp(_targetListExp).all(" ", 0).all("\r\n"));
	_whoisMsgExp.con().tag(IRC_TAG_END);

	// user WHOWAS message
	_whowasMsgExp = Expression(_messageStr);
	_whowasMsgExp.all("WHOWAS ").tag(IRC_TAG_CMD);
	_whowasMsgExp.exp(_nicknameListExp).exp(
		Expression().all(" ").exp(_numberExp).exp(
			Expression().all(" ").exp(_targetExp), 0, 1), 0, 1);
	_whowasMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// KILL message
	_killMsgExp = Expression(_messageStr);
	_killMsgExp.all("KILL ").tag(IRC_TAG_CMD).exp(_nicknameExp);
	_killMsgExp.all(" :").exp(_infoExp).all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// PING message
	_pingMsgExp = Expression(_messageStr);
	_pingMsgExp.all("PING ").tag(IRC_TAG_CMD);
	_pingMsgExp.exp(Expression().all("LAG!").any("0123456789.", 1).all(" "), 0, 1);
	_pingMsgExp.exp(Expression().all(":").exp(_infoExp)).jmp();
	_pingMsgExp.exp(Expression().exp(_targetExp).exp(
		Expression().all(" ").exp(_servernameExp), 0, 1)).exp(
		Expression().all(" ").exp(_infoExp), 0, 1).jmp();
	_pingMsgExp.exp(_infoExp).con();
	_pingMsgExp.all("\r\n").tag(IRC_TAG_END);

	// PONG message
	_pongMsgExp = Expression(_messageStr);
	_pongMsgExp.all("PONG ").tag(IRC_TAG_CMD);
	_pongMsgExp.exp(Expression().exp(_targetExp).exp(
		Expression().all(" ").exp(_servernameExp), 0, 1));
	_pongMsgExp.all(" ", 0).all("\r\n").tag(IRC_TAG_END);

	// ERROR message
	_errorMsgExp = Expression(_messageStr);
	_errorMsgExp.all("ERROR ").tag(IRC_TAG_CMD);
	_errorMsgExp.all(":").exp(_infoExp).all("\r\n").tag(IRC_TAG_END);

	// AWAY message
	_awayMsgExp = Expression(_messageStr);
	_awayMsgExp.all("AWAY").tag(IRC_TAG_CMD);
	_awayMsgExp.exp(Expression().all(" :").exp(_infoExp)).jmp();
	_awayMsgExp.all(" :").con();
	_awayMsgExp.all("\r\n").tag(IRC_TAG_END);

	// UNSUPPORTED message
	_unsupportedMsgExp = Expression(_messageStr);
	_unsupportedMsgExp.all("REHASH").jmp().all("DIE").jmp();
	_unsupportedMsgExp.all("RESTART").jmp().all("SUMMON").jmp().all("USERS").jmp();
	_unsupportedMsgExp.all("WALLOPS").jmp().all("USERHOST").jmp().all("ISON").con();
	_unsupportedMsgExp.tag(IRC_TAG_CMD).inv("\r\n", 0).all("\r\n").tag(IRC_TAG_END);
}

void Message::stringClear()
{
	_messageStr.clear();
	_nicknameStr.clear();
	_userStr.clear();
	_hostnameStr.clear();
	_hostaddrStr.clear();
	_hostStr.clear();
	_servernameStr.clear();
	_prefixStr.clear();
	_passwordStr.clear();
	_bitmodeStr.clear();
	_unusedStr.clear();
	_realnameStr.clear();
	_userModeStr.clear();
	_modeStr.clear();
	_targetStr.clear();
	_infoStr.clear();
	_channelStr.clear();
	_channelListStr.clear();
	_keyListStr.clear();
	_channelModeStr.clear();
	_channelModeListStr.clear();
	_topicStr.clear();
	_userListStr.clear();
	_msgtoStr.clear();
	_statQueryStr.clear();
	_portStr.clear();
	_whoOptStr.clear();
	_targetListStr.clear();
	_nicknameListStr.clear();
	_numberStr.clear();
}

// splits a channel mode string
// e.g. +im or +i+m will become "+i +m"
// special case +b with no target will become b
bool Message::splitCModeStr()
{
	Split sp;
	std::string result;
	size_t target;
	size_t index;
	size_t mode;
	size_t pos;
	bool plus;

	if (_channelModeListStr == "" || _channelModeListStr == "b")
		return (true);

	Expression opts;
	opts.any("+-").any(IRC_CMODE_GIV + IRC_CMODE_TOG + IRC_CMODE_SET, 1);

	sp.split(_channelModeListStr, ' ');
	for (index = 0; index < sp.size(); index++)
	{
		pos = sp[index].find_first_of("+-");
		while (pos != std::string::npos)
		{
			if (pos != 0 && sp[index][pos - 1] != ' ')
				sp[index].insert(pos, " ");
			pos = sp[index].find_first_of("+-", pos + 1);
		}
	}
	_channelModeListStr = sp.join(' ');

	sp.split(_channelModeListStr, ' ');
	index = 0;
	while (index < sp.size())
	{
		if (!opts.match(sp[index]))
			return (false);
		// if +b or b is the last mode then append b and break
		if ((sp[index] == "+b" || sp[index] == "b") && sp.size() == index + 1)
		{
			if (index != 0)
				result.append(" ");
			result.append("b");
			break ;
		}
		target = index + 1;
		for (mode = 1; mode < sp[index].size(); mode++)
		{
			plus = sp[index][0] == '+';
			if (index != 0 || mode != 1)
				result.append(" ");
			result.append(1, sp[index][0]);
			result.append(1, sp[index][mode]);
			if (IRC_CMODE_GIV.find(sp[index][mode]) != std::string::npos
				|| (IRC_CMODE_SET.find(sp[index][mode]) != std::string::npos && plus))
			{
				if (target >= sp.size())
					return (false);
				result.append(" ");
				result.append(sp[target]);
				target++;
			}
		}
		index = target;
	}
	_channelModeListStr = result;
	return (true);
}

void Message::splitModeStr()
{
	Split sp;
	std::string result;
	size_t index;
	size_t mode;

	sp.split(_userModeStr, ' ');
	for (index = 0; index < sp.size(); index++)
	{
		if (sp[index][0] != '+' && sp[index][0] != '-')
			sp[index].insert(0, 1, '+');
		for (mode = 1; mode < sp[index].size(); mode++)
		{
			if (index != 0 || mode != 1)
				result.push_back(' ');
			if (IRC_MODE_ALLOWED.find(sp[index][mode]) != std::string::npos)
			{
				result.push_back(sp[index][0]);
				result.push_back(sp[index][mode]);
			}
		}
	}
	_modeStr = result;
}

void Message::modeToBitmode()
{
	Split sp;
	size_t index;
	int bitmode;
	std::stringstream ss;

	if (_userModeStr.empty())
		return ;

	splitModeStr();

	bitmode = 0;
	sp.split(_modeStr, ' ');
	for (index = 0; index < sp.size(); index++)
	{
		if (sp[index].compare("+w") == 0)
			bitmode |= 0x2;
		if (sp[index].compare("+i") == 0)
			bitmode |= 0x4;
	}
	ss << bitmode;
	_bitmodeStr = ss.str();
}

const std::string &Message::getNickname() const
{
	return (_nicknameStr);
}

const std::string &Message::getUser() const
{
	return (_userStr);
}

const std::string &Message::getHostname() const
{
	return (_hostaddrStr);
}

const std::string &Message::getHostaddr() const
{
	return (_hostaddrStr);
}

const std::string &Message::getHost() const
{
	return (_hostStr);
}

const std::string &Message::getServername() const
{
	return (_servernameStr);
}

const std::string &Message::getPrefix() const
{
	return (_prefixStr);
}

const std::string &Message::getPassword() const
{
	return(_passwordStr);
}

const std::string &Message::getBitmode() const
{
	return(_bitmodeStr);
}

const std::string &Message::getUnused() const
{
	return(_unusedStr);
}

const std::string &Message::getRealname() const
{
	return(_realnameStr);
}

const std::string &Message::getMode() const
{
	return(_modeStr);
}

const std::string &Message::getTarget() const
{
	return(_targetStr);
}

const std::string &Message::getInfo() const
{
	return(_infoStr);
}

const std::string &Message::getChannel() const
{
	return(_channelStr);
}

const std::string &Message::getChannelList() const
{
	return(_channelListStr);
}

const std::string &Message::getKeyList() const
{
	return(_keyListStr);
}

const std::string &Message::getChannelModeList() const
{
	return(_channelModeListStr);
}

const std::string &Message::getTopic() const
{
	return(_topicStr);
}

const std::string &Message::getUserList() const
{
	return(_userListStr);
}

const std::string &Message::getMsgto() const
{
	return(_msgtoStr);
}

const std::string &Message::getStatQuery() const
{
	return (_statQueryStr);
}

const std::string &Message::getPort() const
{
	return (_portStr);
}

const std::string &Message::getWhoOpt() const
{
	return (_whoOptStr);
}

const std::string &Message::getTargetList() const
{
	return (_targetListStr);
}

const std::string &Message::getNicknameList() const
{
	return (_nicknameListStr);
}

const std::string &Message::getNumber() const
{
	return (_numberStr);
}

bool Message::checkTag(int type, int num) const
{
	bool result;

	result = false;
	switch (type)
	{
		case IRC_PASS:
			result = _passMsgExp.tagged(num); break;
		case IRC_NICK:
			result = _nickMsgExp.tagged(num); break;
		case IRC_USER:
			result = _userMsgExp.tagged(num); break;
		case IRC_OPER:
			result = _operMsgExp.tagged(num); break;
		case IRC_MODE:
			result = _modeMsgExp.tagged(num); break;
		case IRC_SERVICE:
			result = _serviceMsgExp.tagged(num); break;
		case IRC_QUIT:
			result = _quitMsgExp.tagged(num); break;
		case IRC_SQUIT:
			result = _squitMsgExp.tagged(num); break;
		case IRC_JOIN:
			result = _joinMsgExp.tagged(num); break;
		case IRC_PART:
			result = _partMsgExp.tagged(num); break;
		case IRC_CMODE:
			result = _cmodeMsgExp.tagged(num); break;
		case IRC_TOPIC:
			result = _topicMsgExp.tagged(num); break;
		case IRC_NAMES:
			result = _namesMsgExp.tagged(num); break;
		case IRC_LIST:
			result = _listMsgExp.tagged(num); break;
		case IRC_INVITE:
			result = _inviteMsgExp.tagged(num); break;
		case IRC_KICK:
			result = _kickMsgExp.tagged(num); break;
		case IRC_PRIVMSG:
			result = _privmsgMsgExp.tagged(num); break;
		case IRC_NOTICE:
			result = _noticeMsgExp.tagged(num); break;
		case IRC_MOTD:
			result = _motdMsgExp.tagged(num); break;
		case IRC_LUSERS:
			result = _lusersMsgExp.tagged(num); break;
		case IRC_VERSION:
			result = _versionMsgExp.tagged(num); break;
		case IRC_STATS:
			result = _statsMsgExp.tagged(num); break;
		case IRC_LINKS:
			result = _linksMsgExp.tagged(num); break;
		case IRC_TIME:
			result = _timeMsgExp.tagged(num); break;
		case IRC_CONNECT:
			result = _connectMsgExp.tagged(num); break;
		case IRC_TRACE:
			result = _traceMsgExp.tagged(num); break;
		case IRC_ADMIN:
			result = _adminMsgExp.tagged(num); break;
		case IRC_INFO:
			result = _infoMsgExp.tagged(num); break;
		case IRC_SERVLIST:
			result = _servlistMsgExp.tagged(num); break;
		case IRC_SQUERY:
			result = _squeryMsgExp.tagged(num); break;
		case IRC_WHO:
			result = _whoMsgExp.tagged(num); break;
		case IRC_WHOIS:
			result = _whoisMsgExp.tagged(num); break;
		case IRC_WHOWAS:
			result = _whowasMsgExp.tagged(num); break;
		case IRC_KILL:
			result = _killMsgExp.tagged(num); break;
		case IRC_PING:
			result = _pingMsgExp.tagged(num); break;
		case IRC_PONG:
			result = _pongMsgExp.tagged(num); break;
		case IRC_ERROR:
			result = _errorMsgExp.tagged(num); break;
		case IRC_AWAY:
			result = _awayMsgExp.tagged(num); break;
		case IRC_UNSUPPORTED:
			result = _unsupportedMsgExp.tagged(num); break;
	}
	return (result);
}

void Message::setMsg(const std::string &message)
{
	int type;

	stringClear();
	type = IRC_INVALID;
	_commandStr.assign("INVALID");
	if (_passMsgExp.match(message) || _passMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_PASS; _commandStr.assign("PASS"); }
	else if (_nickMsgExp.match(message) || _nickMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_NICK; _commandStr.assign("NICK"); }
	else if (_userMsgExp.match(message) || _userMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_USER; _commandStr.assign("USER"); }
	else if (_operMsgExp.match(message) || _operMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_OPER; _commandStr.assign("OPER"); }
	else if (_modeMsgExp.match(message) || _modeMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_MODE; _commandStr.assign("MODE"); }
	else if (_serviceMsgExp.match(message) || _serviceMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_SERVICE; _commandStr.assign("SERVICE"); }
	else if (_quitMsgExp.match(message) || _quitMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_QUIT; _commandStr.assign("QUIT"); }
	else if (_squitMsgExp.match(message) || _squitMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_SQUIT; _commandStr.assign("SQUIT"); }
	else if (_joinMsgExp.match(message) || _joinMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_JOIN; _commandStr.assign("JOIN"); }
	else if (_partMsgExp.match(message) || _partMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_PART; _commandStr.assign("PART"); }
	else if (_cmodeMsgExp.match(message) || _cmodeMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_CMODE; _commandStr.assign("CMODE"); }
	else if (_topicMsgExp.match(message) || _topicMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_TOPIC; _commandStr.assign("TOPIC"); }
	else if (_namesMsgExp.match(message) || _namesMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_NAMES; _commandStr.assign("NAMES"); }
	else if (_listMsgExp.match(message) || _listMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_LIST; _commandStr.assign("LIST"); }
	else if (_inviteMsgExp.match(message) || _inviteMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_INVITE; _commandStr.assign("INVITE"); }
	else if (_kickMsgExp.match(message) || _kickMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_KICK; _commandStr.assign("KICK"); }
	else if (_privmsgMsgExp.match(message) || _privmsgMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_PRIVMSG; _commandStr.assign("PRIVMSG"); }
	else if (_noticeMsgExp.match(message) || _noticeMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_NOTICE; _commandStr.assign("NOTICE"); }
	else if (_motdMsgExp.match(message) || _motdMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_MOTD; _commandStr.assign("MOTD"); }
	else if (_lusersMsgExp.match(message) || _lusersMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_LUSERS; _commandStr.assign("LUSERS"); }
	else if (_versionMsgExp.match(message) || _versionMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_VERSION; _commandStr.assign("VERSION"); }
	else if (_statsMsgExp.match(message) || _statsMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_STATS; _commandStr.assign("STATS"); }
	else if (_linksMsgExp.match(message) || _linksMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_LINKS; _commandStr.assign("LINKS"); }
	else if (_timeMsgExp.match(message) || _timeMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_TIME; _commandStr.assign("TIME"); }
	else if (_connectMsgExp.match(message) || _connectMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_CONNECT; _commandStr.assign("CONNECT"); }
	else if (_traceMsgExp.match(message) || _traceMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_TRACE; _commandStr.assign("TRACE"); }
	else if (_adminMsgExp.match(message) || _adminMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_ADMIN; _commandStr.assign("ADMIN"); }
	else if (_infoMsgExp.match(message) || _infoMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_INFO; _commandStr.assign("INFO"); }
	else if (_servlistMsgExp.match(message) || _servlistMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_SERVLIST; _commandStr.assign("SERVLIST"); }
	else if (_squeryMsgExp.match(message) || _squeryMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_SQUERY; _commandStr.assign("SQUERY"); }
	else if (_whoisMsgExp.match(message) || _whoisMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_WHOIS; _commandStr.assign("WHOIS"); }
	else if (_whowasMsgExp.match(message) || _whowasMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_WHOWAS; _commandStr.assign("WHOWAS"); }
	else if (_whoMsgExp.match(message) || _whoMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_WHO; _commandStr.assign("WHO"); }
	else if (_killMsgExp.match(message) || _killMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_KILL; _commandStr.assign("KILL"); }
	else if (_pingMsgExp.match(message) || _pingMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_PING; _commandStr.assign("PING"); }
	else if (_pongMsgExp.match(message) || _pongMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_PONG; _commandStr.assign("PONG"); }
	else if (_errorMsgExp.match(message) || _errorMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_ERROR; _commandStr.assign("ERROR"); }
	else if (_awayMsgExp.match(message) || _awayMsgExp.tagged(IRC_TAG_CMD))
		{ type = IRC_AWAY; _commandStr.assign("AWAY"); }
	else if (_unsupportedMsgExp.match(message) || _unsupportedMsgExp.tagged(IRC_TAG_CMD))
		{
			type = IRC_UNSUPPORTED;
			_commandStr.assign("UNSUPPORTED");
			Split sp;
			sp.split(_messageStr, ' ');
			if (sp.size() != 0)
				_commandStr.assign(sp[0]);
		}

	if (type == IRC_TOPIC && _topicStr.size() > 1 && _topicStr[0] == ':')
		_topicStr.erase(0, 1);
	if (type == IRC_CMODE)
		if (!splitCModeStr())
		{
			type = IRC_INVALID;
			_commandStr.assign("INVALID");
		}
	if (type == IRC_MODE)
		splitModeStr();
	if (type == IRC_USER)
		modeToBitmode();

	_messageType = type;
}

int Message::getMsgType(std::string message)
{
	setMsg(message);
	return (getMsgType());
}

int Message::getMsgType()
{
	if (checkTag(_messageType, IRC_TAG_END))
		return (_messageType);
	return (IRC_INVALID);
}

int Message::getMsgTypeCmd()
{
	if (checkTag(_messageType, IRC_TAG_CMD))
		return (_messageType);
	return (IRC_INVALID);
}

const std::string &Message::getMsgTypeCmdStr()
{
	return (_commandStr);
}

const std::string &Message::getMsgStr() const
{
	return (_messageStr);
}

/*
 * sets the message string for parsing
 */
void Message::set(const std::string &message)
{
	setMsg(message);
}

/*
 * returns the message string that was parsed
 */
const std::string &Message::get() const
{
	return (getMsgStr());
}

/*
 * returns the message type as an integer
 * will return a type if message is incomplete
 */
int Message::type()
{
	return (getMsgTypeCmd());
}

/*
 * returns the message type as a string
 */
const std::string &Message::command()
{
	return (getMsgTypeCmdStr());
}

/*
 * returns true if the message is correctly parsed
 */
bool Message::is_complete()
{
	return (getMsgType() != IRC_INVALID);
}

/*
 * returns true if the message has a recognised command
 */
bool Message::is_command()
{
	return (getMsgTypeCmd() != IRC_INVALID && getMsgType() == IRC_INVALID);
}

/*
 * returns the number of parameters in the message
 * parameter 0 is always the command
 */
int Message::size() const
{
	return (_params->size());
}

/*
 * returns the parameter in the message at num
 * returns "" if the message or num is invalid
 * parameter 0 is always the command
 */
const std::string &Message::param(int num) const
{
	return (_params->param(num));
}
