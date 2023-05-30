/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/27 09:24:28 by abossel           #+#    #+#             */
/*   Updated: 2023/05/29 12:06:03 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <string>
# include "Expression.hpp"
# include "MessageParam.hpp"

class MessageParam;

# define IRC_INVALID 1
# define IRC_PASS 2
# define IRC_NICK 3
# define IRC_USER 4
# define IRC_OPER 5
# define IRC_MODE 6
# define IRC_SERVICE 7
# define IRC_QUIT 8
# define IRC_SQUIT 9
# define IRC_JOIN 10
# define IRC_PART 11
# define IRC_CMODE 12
# define IRC_TOPIC 13
# define IRC_NAMES 14
# define IRC_LIST 15
# define IRC_INVITE 16
# define IRC_KICK 17
# define IRC_PRIVMSG 18
# define IRC_NOTICE 19
# define IRC_MOTD 20
# define IRC_LUSERS 21
# define IRC_VERSION 22
# define IRC_STATS 23
# define IRC_LINKS 24
# define IRC_TIME 25
# define IRC_CONNECT 26
# define IRC_TRACE 27
# define IRC_ADMIN 28
# define IRC_INFO 29
# define IRC_SERVLIST 30
# define IRC_SQUERY 31
# define IRC_WHO 32
# define IRC_WHOIS 33
# define IRC_WHOWAS 34
# define IRC_KILL 35
# define IRC_PING 36
# define IRC_PONG 37
# define IRC_ERROR 38
# define IRC_AWAY 39
# define IRC_UNSUPPORTED 40

# define IRC_TAG_CMD 1000
# define IRC_TAG_END 1010

class Message
{
	public:
		Message();
		virtual ~Message();
		const std::string &getNickname() const;
		const std::string &getUser() const;
		const std::string &getHostname() const;
		const std::string &getHostaddr() const;
		const std::string &getHost() const;
		const std::string &getServername() const;
		const std::string &getPrefix() const;
		const std::string &getPassword() const;
		const std::string &getBitmode() const;
		const std::string &getUnused() const;
		const std::string &getRealname() const;
		const std::string &getMode() const;
		const std::string &getUserMode() const;
		const std::string &getTarget() const;
		const std::string &getInfo() const;
		const std::string &getChannel() const;
		const std::string &getChannelList() const;
		const std::string &getKeyList() const;
		const std::string &getChannelModeList() const;
		const std::string &getTopic() const;
		const std::string &getUserList() const;
		const std::string &getMsgto() const;
		const std::string &getStatQuery() const;
		const std::string &getPort() const;
		const std::string &getWhoOpt() const;
		const std::string &getTargetList() const;
		const std::string &getNicknameList() const;
		const std::string &getNumber() const;
		bool checkTag(int type, int num) const;
		int getMsgType(std::string message);
		int getMsgType();
		int getMsgTypeCmd();
		const std::string &getMsgTypeCmdStr();
		void setMsg(const std::string &message);
		const std::string &getMsgStr() const;

		void set(const std::string &message);
		const std::string &get() const;
		int type();
		const std::string &command();
		bool is_complete();
		bool is_command();
		int size() const;
		const std::string &param(int num) const;

	private:
		void expressionInitHostaddr();
		void expressionInit();
		void messageInit();
		void stringClear();
		bool splitCModeStr();
		void splitModeStr();
		void modeToBitmode();

		Expression _nicknameExp;
		std::string _nicknameStr;

		Expression _userExp;
		std::string _userStr;

		Expression _hostnameExp;
		std::string _hostnameStr;

		Expression _hostaddrExp;
		std::string _hostaddrStr;

		Expression _hostExp;
		std::string _hostStr;

		Expression _servernameExp;
		std::string _servernameStr;

		Expression _prefixExp;
		std::string _prefixStr;

		Expression _passwordExp;
		std::string _passwordStr;

		Expression _bitmodeExp;
		std::string _bitmodeStr;

		Expression _unusedExp;
		std::string _unusedStr;

		Expression _realnameExp;
		std::string _realnameStr;

		Expression _userModeExp;
		std::string _userModeStr;

		Expression _modeExp;
		std::string _modeStr;

		Expression _targetExp;
		std::string _targetStr;

		Expression _infoExp;
		std::string _infoStr;

		Expression _channelExp;
		std::string _channelStr;

		Expression _channelListExp;
		std::string _channelListStr;

		Expression _keyListExp;
		std::string _keyListStr;

		Expression _channelModeExp;
		std::string _channelModeStr;

		Expression _channelModeListExp;
		std::string _channelModeListStr;

		Expression _topicExp;
		std::string _topicStr;

		Expression _userListExp;
		std::string _userListStr;

		Expression _msgtoExp;
		std::string _msgtoStr;

		Expression _statQueryExp;
		std::string _statQueryStr;

		Expression _portExp;
		std::string _portStr;

		Expression _whoOptExp;
		std::string _whoOptStr;

		Expression _targetListExp;
		std::string _targetListStr;

		Expression _nicknameListExp;
		std::string _nicknameListStr;

		Expression _numberExp;
		std::string _numberStr;

		Expression _passMsgExp;
		Expression _nickMsgExp;
		Expression _userMsgExp;
		Expression _operMsgExp;
		Expression _modeMsgExp;
		Expression _serviceMsgExp;
		Expression _quitMsgExp;
		Expression _squitMsgExp;
		Expression _joinMsgExp;
		Expression _partMsgExp;
		Expression _cmodeMsgExp;
		Expression _topicMsgExp;
		Expression _namesMsgExp;
		Expression _listMsgExp;
		Expression _inviteMsgExp;
		Expression _kickMsgExp;
		Expression _privmsgMsgExp;
		Expression _noticeMsgExp;
		Expression _motdMsgExp;
		Expression _lusersMsgExp;
		Expression _versionMsgExp;
		Expression _statsMsgExp;
		Expression _linksMsgExp;
		Expression _timeMsgExp;
		Expression _connectMsgExp;
		Expression _traceMsgExp;
		Expression _adminMsgExp;
		Expression _infoMsgExp;
		Expression _servlistMsgExp;
		Expression _squeryMsgExp;
		Expression _whoMsgExp;
		Expression _whoisMsgExp;
		Expression _whowasMsgExp;
		Expression _killMsgExp;
		Expression _pingMsgExp;
		Expression _pongMsgExp;
		Expression _errorMsgExp;
		Expression _awayMsgExp;
		Expression _unsupportedMsgExp;

		MessageParam *_params;
		std::string _messageStr;
		std::string _commandStr;
		int _messageType;
};

#endif
