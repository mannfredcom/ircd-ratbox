/*
 *  ircd-ratbox: A slightly useful ircd.
 *  numeric.h: A header for the numeric functions.
 *
 *  Copyright (C) 1990 Jarkko Oikarinen and University of Oulu, Co Center
 *  Copyright (C) 1996-2002 Hybrid Development Team
 *  Copyright (C) 2002-2026 ircd-ratbox development team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

#ifndef INCLUDED_numeric_h
#define INCLUDED_numeric_h

/*
 * form_str - return a format string for a message number
 * messages are defined below
 */

#define FORM_STR1(n)		NUMERIC_STR_##n
#define form_str(n)		FORM_STR1(n)
#define s_RPL(x)		x, form_str(x)


#define NUMERIC_STR_1	/*** RPL_WELCOME ***/		":Welcome to the %s Internet Relay Chat Network %s"
#define NUMERIC_STR_2	/*** RPL_YOURHOST ***/		":Your host is %s, running version %s"
#define NUMERIC_STR_3	/*** RPL_CREATED ***/		":This server was created %s"
#define NUMERIC_STR_4	/*** RPL_MYINFO ***/		"%s %s oiwszcrkfydnxbauglZCD biklmnopstveIrS bkloveI"
#define NUMERIC_STR_5	/*** RPL_ISUPPORT ***/		"%s :are supported by this server"
#define NUMERIC_STR_10	/*** RPL_REDIR ***/		":%s %d :Please use this Server/Port instead"
#define NUMERIC_STR_15	/*** RPL_MAP ***/		":%s"
#define NUMERIC_STR_17	/*** RPL_MAPEND ***/		":End of /MAP"
#define NUMERIC_STR_43	/*** RPL_SAVENICK ***/		"%s :Nick collision, forcing nick change to your unique ID"
#define NUMERIC_STR_200	/*** RPL_TRACELINK ***/		"Link %s %s %s"
#define NUMERIC_STR_201	/*** RPL_TRACECONNECTING ***/	"Try. %s %s"
#define NUMERIC_STR_202	/*** RPL_TRACEHANDSHAKE ***/	"H.S. %s %s"
#define NUMERIC_STR_203	/*** RPL_TRACEUNKNOWN ***/	"???? %s %s (%s) %" RBTT_FMT
#define NUMERIC_STR_204	/*** RPL_TRACEOPERATOR ***/	"Oper %s %s (%s) %" RBTT_FMT " %" RBTT_FMT
#define NUMERIC_STR_205	/*** RPL_TRACEUSER ***/		"User %s %s (%s) %" RBTT_FMT " %" RBTT_FMT
#define NUMERIC_STR_206	/*** RPL_TRACESERVER ***/	"Serv %s %dS %dC %s %s!%s@%s %" RBTT_FMT
#define NUMERIC_STR_208	/*** RPL_TRACENEWTYPE ***/	"<newtype> 0 %s"
#define NUMERIC_STR_209	/*** RPL_TRACECLASS ***/	"Class %s %d"
#define NUMERIC_STR_211	/*** RPL_STATSLINKINFO ***/	"%s %zu %u %" PRIu64 " %u %" PRIu64 " :%" RBTT_FMT " %" RBTT_FMT " %s"
#define NUMERIC_STR_212	/*** RPL_STATSCOMMANDS ***/	"%s %lu %lu :%lu"
#define NUMERIC_STR_213	/*** RPL_STATSCLINE ***/	"C %s %s %s %d %s"
#define NUMERIC_STR_215	/*** RPL_STATSILINE ***/	"I %s * %s@%s %d %s"
#define NUMERIC_STR_216	/*** RPL_STATSKLINE ***/	"%c %s * %s :%s%s%s"
#define NUMERIC_STR_217	/*** RPL_STATSQLINE ***/	"%c %d %s :%s"
#define NUMERIC_STR_218	/*** RPL_STATSYLINE ***/	"Y %s %d %d %d %u %d.%d %d.%d %u"
#define NUMERIC_STR_219	/*** RPL_ENDOFSTATS ***/	"%c :End of /STATS report"
#define NUMERIC_STR_220	/*** RPL_STATSPLINE ***/	"%c %d %s %d :%s%s"
#define NUMERIC_STR_221	/*** RPL_UMODEIS ***/		"%s"
#define NUMERIC_STR_225	/*** RPL_STATSDLINE ***/	"%c %s :%s%s%s"
#define NUMERIC_STR_241	/*** RPL_STATSLLINE ***/	"L %s * %s 0 -1"
#define NUMERIC_STR_242	/*** RPL_STATSUPTIME ***/	":Server Up %" RBTT_FMT " days, %" RBTT_FMT ":%02" RBTT_FMT ":%02" RBTT_FMT
#define NUMERIC_STR_243	/*** RPL_STATSOLINE ***/	"O %s@%s * %s %s %s"
#define NUMERIC_STR_244	/*** RPL_STATSHLINE ***/	"H %s * %s 0 -1"
#define NUMERIC_STR_247	/*** RPL_STATSXLINE ***/	"%c %d %s :%s"
#define NUMERIC_STR_248	/*** RPL_STATSULINE ***/	"U %s %s@%s %s"
#define NUMERIC_STR_250	/*** RPL_STATSCONN ***/		":Highest connection count: %lu (%lu clients) (%lu connections received)"
#define NUMERIC_STR_251	/*** RPL_LUSERCLIENT ***/	":There are %lu users and %lu invisible on %lu servers"
#define NUMERIC_STR_252	/*** RPL_LUSEROP ***/		"%lu :IRC Operators online"
#define NUMERIC_STR_253	/*** RPL_LUSERUNKNOWN ***/	"%lu :unknown connection(s)"
#define NUMERIC_STR_254	/*** RPL_LUSERCHANNELS ***/	"%lu :channels formed"
#define NUMERIC_STR_255	/*** RPL_LUSERME ***/		":I have %lu clients and %lu servers"
#define NUMERIC_STR_256	/*** RPL_ADMINME ***/		":Administrative info about %s"
#define NUMERIC_STR_257	/*** RPL_ADMINLOC1 ***/		":%s"
#define NUMERIC_STR_258	/*** RPL_ADMINLOC2 ***/		":%s"
#define NUMERIC_STR_259	/*** RPL_ADMINEMAIL ***/	":%s"
#define NUMERIC_STR_262	/*** RPL_ENDOFTRACE ***/	"%s :End of TRACE"
#define NUMERIC_STR_263	/*** RPL_LOAD2HI ***/		"%s :Server load is temporarily too heavy. Please wait a while and try again."
#define NUMERIC_STR_265	/*** RPL_LOCALUSERS ***/	"%lu %lu :Current local users %lu, max %lu"
#define NUMERIC_STR_266	/*** RPL_GLOBALUSERS ***/	"%lu %lu :Current global users %lu, max %lu"
#define NUMERIC_STR_276	/*** RPL_WHOISCERTFP ***/	"%s :has client certificate fingerprint %s"
#define NUMERIC_STR_281	/*** RPL_ACCEPTLIST ***/	"%s"
#define NUMERIC_STR_282	/*** RPL_ENDOFACCEPT ***/	":End of /ACCEPT list."
#define NUMERIC_STR_301	/*** RPL_AWAY ***/		"%s :%s"
#define NUMERIC_STR_302	/*** RPL_USERHOST ***/		":%s"
#define NUMERIC_STR_303	/*** RPL_ISON ***/		":%s"
#define NUMERIC_STR_305	/*** RPL_UNAWAY ***/		":You are no longer marked as being away"
#define NUMERIC_STR_306	/*** RPL_NOWAWAY ***/		":You have been marked as being away"
#define NUMERIC_STR_311	/*** RPL_WHOISUSER ***/		"%s %s %s * :%s"
#define NUMERIC_STR_312	/*** RPL_WHOISSERVER ***/	"%s %s :%s"
#define NUMERIC_STR_313	/*** RPL_WHOISOPERATOR ***/	"%s :%s"
#define NUMERIC_STR_314	/*** RPL_WHOWASUSER ***/	"%s %s %s * :%s"
#define NUMERIC_STR_315	/*** RPL_ENDOFWHO ***/		"%s :End of /WHO list."
#define NUMERIC_STR_317	/*** RPL_WHOISIDLE ***/		"%s %" RBTT_FMT " %" RBTT_FMT " :seconds idle, signon time"
#define NUMERIC_STR_318	/*** RPL_ENDOFWHOIS ***/	"%s :End of /WHOIS list."
#define NUMERIC_STR_319	/*** RPL_WHOISCHANNELS ***/	":%s 319 %s %s :"
#define NUMERIC_STR_321	/*** RPL_LISTSTART ***/		"Channel :Users  Name"
#define NUMERIC_STR_322	/*** RPL_LIST ***/		"%s %lu :%s"
#define NUMERIC_STR_323	/*** RPL_LISTEND ***/		":End of /LIST"
#define NUMERIC_STR_324	/*** RPL_CHANNELMODEIS ***/	"%s %s"
#define NUMERIC_STR_329	/*** RPL_CREATIONTIME ***/	"%s %" RBTT_FMT
#define NUMERIC_STR_330	/*** RPL_WHOISLOGGEDIN ***/	":%s 330 %s %s %s :is logged in as"
#define NUMERIC_STR_331	/*** RPL_NOTOPIC ***/		"%s :No topic is set."
#define NUMERIC_STR_332	/*** RPL_TOPIC ***/		"%s :%s"
#define NUMERIC_STR_333	/*** RPL_TOPICWHOTIME ***/	"%s %s %" RBTT_FMT
#define NUMERIC_STR_338	/*** RPL_WHOISACTUALLY ***/	"%s %s :actually using host"
#define NUMERIC_STR_341	/*** RPL_INVITING ***/		"%s %s"
#define NUMERIC_STR_346	/*** RPL_INVITELIST ***/	"%s %s %s %" RBTT_FMT
#define NUMERIC_STR_347	/*** RPL_ENDOFINVITELIST ***/	"%s :End of Channel Invite List"
#define NUMERIC_STR_348	/*** RPL_EXCEPTLIST ***/	"%s %s %s %" RBTT_FMT
#define NUMERIC_STR_349	/*** RPL_ENDOFEXCEPTLIST ***/	"%s :End of Channel Exception List"
#define NUMERIC_STR_351	/*** RPL_VERSION ***/		"%s(%s). %s :%s TS%dow %s"
#define NUMERIC_STR_352	/*** RPL_WHOREPLY ***/		"%s %s %s %s %s %s :%d %s"
#define NUMERIC_STR_353	/*** RPL_NAMREPLY ***/		":%s 353 %s %s %s :"
#define NUMERIC_STR_362	/*** RPL_CLOSING ***/		"%s :Closed. Status = %d"
#define NUMERIC_STR_363	/*** RPL_CLOSEEND ***/		"%d :Connections Closed"
#define NUMERIC_STR_364	/*** RPL_LINKS ***/		"%s %s :%d %s"
#define NUMERIC_STR_365	/*** RPL_ENDOFLINKS ***/	"%s :End of /LINKS list."
#define NUMERIC_STR_366	/*** RPL_ENDOFNAMES ***/	"%s :End of /NAMES list."
#define NUMERIC_STR_367	/*** RPL_BANLIST ***/		"%s %s %s %" RBTT_FMT
#define NUMERIC_STR_368	/*** RPL_ENDOFBANLIST ***/	"%s :End of Channel Ban List"
#define NUMERIC_STR_369	/*** RPL_ENDOFWHOWAS ***/	"%s :End of WHOWAS"
#define NUMERIC_STR_371	/*** RPL_INFO ***/		":%s"
#define NUMERIC_STR_372	/*** RPL_MOTD ***/		":- %s"
#define NUMERIC_STR_374	/*** RPL_ENDOFINFO ***/		":End of /INFO list."
#define NUMERIC_STR_375	/*** RPL_MOTDSTART ***/		":- %s Message of the Day - "
#define NUMERIC_STR_376	/*** RPL_ENDOFMOTD ***/		":End of /MOTD command."
#define NUMERIC_STR_381	/*** RPL_YOUREOPER ***/		":You have entered... the PANTS FREE ZONE!"
#define NUMERIC_STR_382	/*** RPL_REHASHING ***/		"%s :Rehashing"
#define NUMERIC_STR_386	/*** RPL_RSACHALLENGE ***/	":%s"
#define NUMERIC_STR_391	/*** RPL_TIME ***/		"%s :%s"
#define NUMERIC_STR_401	/*** ERR_NOSUCHNICK ***/	"%s :No such nick/channel"
#define NUMERIC_STR_402	/*** ERR_NOSUCHSERVER ***/	"%s :No such server"
#define NUMERIC_STR_403	/*** ERR_NOSUCHCHANNEL ***/	"%s :No such channel"
#define NUMERIC_STR_404	/*** ERR_CANNOTSENDTOCHAN ***/	"%s :Cannot send to channel"
#define NUMERIC_STR_405	/*** ERR_TOOMANYCHANNELS ***/	"%s :You have joined too many channels"
#define NUMERIC_STR_406	/*** ERR_WASNOSUCHNICK ***/	"%s :There was no such nickname"
#define NUMERIC_STR_407	/*** ERR_TOOMANYTARGETS ***/	":%s :Too many recipients."
#define NUMERIC_STR_409	/*** ERR_NOORIGIN ***/		":No origin specified"
#define NUMERIC_STR_410	/*** ERR_INVALIDCAPCMD ***/	"%s :Invalid CAP subcommand"
#define NUMERIC_STR_411	/*** ERR_NORECIPIENT ***/	":No recipient given (%s)"
#define NUMERIC_STR_412	/*** ERR_NOTEXTTOSEND ***/	":No text to send"
#define NUMERIC_STR_413	/*** ERR_NOTOPLEVEL ***/	"%s :No toplevel domain specified"
#define NUMERIC_STR_414	/*** ERR_WILDTOPLEVEL ***/	"%s :Wildcard in toplevel Domain"
#define NUMERIC_STR_416	/*** ERR_TOOMANYMATCHES ***/	"%s :output too large, truncated"
#define NUMERIC_STR_421	/*** ERR_UNKNOWNCOMMAND ***/	"%s :Unknown command"
#define NUMERIC_STR_422	/*** ERR_NOMOTD ***/		":MOTD File is missing"
#define NUMERIC_STR_431	/*** ERR_NONICKNAMEGIVEN ***/	":No nickname given"
#define NUMERIC_STR_432	/*** ERR_ERRONEUSNICKNAME ***/	"%s :Erroneous Nickname"
#define NUMERIC_STR_433	/*** ERR_NICKNAMEINUSE ***/	"%s :Nickname is already in use."
#define NUMERIC_STR_436	/*** ERR_NICKCOLLISION ***/	"%s :Nickname collision KILL"
#define NUMERIC_STR_437	/*** ERR_UNAVAILRESOURCE ***/	"%s :Nick/channel is temporarily unavailable"
#define NUMERIC_STR_438	/*** ERR_NICKTOOFAST ***/	"%s %s :Nick change too fast. Please wait %d seconds."
#define NUMERIC_STR_441	/*** ERR_USERNOTINCHANNEL ***/	"%s %s :They aren't on that channel"
#define NUMERIC_STR_442	/*** ERR_NOTONCHANNEL ***/	"%s :You're not on that channel"
#define NUMERIC_STR_443	/*** ERR_USERONCHANNEL ***/	"%s %s :is already on channel"
#define NUMERIC_STR_451	/*** ERR_NOTREGISTERED ***/	":You have not registered"
#define NUMERIC_STR_456	/*** ERR_ACCEPTFULL ***/	":Accept list is full"
#define NUMERIC_STR_457	/*** ERR_ACCEPTEXIST ***/	"%s :is already on your accept list"
#define NUMERIC_STR_458	/*** ERR_ACCEPTNOT ***/		"%s :is not on your accept list"
#define NUMERIC_STR_461	/*** ERR_NEEDMOREPARAMS ***/	"%s :Not enough parameters"
#define NUMERIC_STR_462	/*** ERR_ALREADYREGISTRED ***/	":You may not reregister"
#define NUMERIC_STR_464	/*** ERR_PASSWDMISMATCH ***/	":Password Incorrect"
#define NUMERIC_STR_465	/*** ERR_YOUREBANNEDCREEP ***/	":You are banned from this server- %s"
#define NUMERIC_STR_471	/*** ERR_CHANNELISFULL ***/	"%s :Cannot join channel (+l)"
#define NUMERIC_STR_472	/*** ERR_UNKNOWNMODE ***/	"%c :is unknown mode char to me"
#define NUMERIC_STR_473	/*** ERR_INVITEONLYCHAN ***/	"%s :Cannot join channel (+i)"
#define NUMERIC_STR_474	/*** ERR_BANNEDFROMCHAN ***/	"%s :Cannot join channel (+b)"
#define NUMERIC_STR_475	/*** ERR_BADCHANNELKEY ***/	"%s :Cannot join channel (+k)"
#define NUMERIC_STR_477	/*** ERR_NEEDREGGEDNICK ***/	"%s :Cannot join channel (+r)"
#define NUMERIC_STR_478	/*** ERR_BANLISTFULL ***/	"%s %s :Channel ban list is full"
#define NUMERIC_STR_479	/*** ERR_BADCHANNAME ***/	"%s :Illegal channel name"
#define NUMERIC_STR_480	/*** ERR_SSLONLYCHAN ***/	"%s :Cannot join channel (+S) - SSL/TLS required"
#define NUMERIC_STR_481	/*** ERR_NOPRIVILEGES ***/	":Permission Denied - You're not an IRC operator"
#define NUMERIC_STR_482	/*** ERR_CHANOPRIVSNEEDED ***/	"%s :You're not channel operator"
#define NUMERIC_STR_483	/*** ERR_CANTKILLSERVER ***/	":You can't kill a server!"
#define NUMERIC_STR_484	/*** ERR_ISCHANSERVICE ***/	"%s %s :Cannot kick or deop a network service"
#define NUMERIC_STR_489	/*** ERR_VOICENEEDED ***/	"%s :You're neither voiced nor channel operator"
#define NUMERIC_STR_491	/*** ERR_NOOPERHOST ***/	":Only few of mere mortals may try to enter the twilight zone"
#define NUMERIC_STR_501	/*** ERR_UMODEUNKNOWNFLAG ***/	":Unknown MODE flag"
#define NUMERIC_STR_502	/*** ERR_USERSDONTMATCH ***/	":Can't change mode for other users"
#define NUMERIC_STR_504	/*** ERR_USERNOTONSERV ***/	"%s :User is not on this server"
#define NUMERIC_STR_513	/*** ERR_WRONGPONG ***/		":To connect type /QUOTE PONG %08lX"
#define NUMERIC_STR_524	/*** ERR_HELPNOTFOUND ***/	"%s :Help not found"
#define NUMERIC_STR_671	/*** RPL_WHOISSECURE ***/	"%s :%s"
#define NUMERIC_STR_702	/*** RPL_MODLIST ***/		"%s %p %s %s"
#define NUMERIC_STR_703	/*** RPL_ENDOFMODLIST ***/	":End of /MODLIST."
#define NUMERIC_STR_704	/*** RPL_HELPSTART ***/		"%s :%s"
#define NUMERIC_STR_705	/*** RPL_HELPTXT ***/		"%s :%s"
#define NUMERIC_STR_706	/*** RPL_ENDOFHELP ***/		"%s :End of /HELP."
#define NUMERIC_STR_707	/*** ERR_TARGCHANGE ***/	"%s :Targets changing too fast, message dropped"
#define NUMERIC_STR_708	/*** RPL_ETRACEFULL ***/	"%s %s %s %s %s %s %s :%s"
#define NUMERIC_STR_709	/*** RPL_ETRACE ***/		"%s %s %s %s %s %s :%s"
#define NUMERIC_STR_710	/*** RPL_KNOCK ***/		"%s %s!%s@%s :has asked for an invite."
#define NUMERIC_STR_711	/*** RPL_KNOCKDLVR ***/		"%s :Your KNOCK has been delivered."
#define NUMERIC_STR_712	/*** ERR_TOOMANYKNOCK ***/	"%s :Too many KNOCKs (%s)."
#define NUMERIC_STR_713	/*** ERR_CHANOPEN ***/		"%s :Channel is open."
#define NUMERIC_STR_714	/*** ERR_KNOCKONCHAN ***/	"%s :You are already on that channel."
#define NUMERIC_STR_715	/*** ERR_KNOCKDISABLED ***/	":KNOCKs are disabled."
#define NUMERIC_STR_716	/*** ERR_TARGUMODEG ***/	"%s :is in +g mode (server-side ignore.)"
#define NUMERIC_STR_717	/*** RPL_TARGNOTIFY ***/	"%s :has been informed that you messaged them."
#define NUMERIC_STR_718	/*** RPL_UMODEGMSG ***/		"%s %s@%s :is messaging you, and you have umode +g."
#define NUMERIC_STR_720	/*** RPL_OMOTDSTART ***/	":Start of OPER MOTD"
#define NUMERIC_STR_721	/*** RPL_OMOTD ***/		"%s"
#define NUMERIC_STR_722	/*** RPL_ENDOFOMOTD ***/	":End of OPER MOTD"
#define NUMERIC_STR_723	/*** ERR_NOPRIVS ***/		"%s :Insufficient oper privs"
#define NUMERIC_STR_725	/*** RPL_TESTLINE ***/		"%c %" RBTT_FMT " %s :%s"
#define NUMERIC_STR_726	/*** RPL_NOTESTLINE ***/	"%s :No matches"
#define NUMERIC_STR_727	/*** RPL_TESTMASKGECOS ***/	"%d %d %s!%s@%s %s :Local/remote clients match"
#define NUMERIC_STR_730	/*** RPL_MONONLINE ***/		":%s 730 %s :%s"
#define NUMERIC_STR_731	/*** RPL_MONOFFLINE ***/	":%s 731 %s :%s"
#define NUMERIC_STR_732	/*** RPL_MONLIST ***/		":%s 732 %s :%s"
#define NUMERIC_STR_733	/*** RPL_ENDOFMONLIST ***/	":End of MONITOR list"
#define NUMERIC_STR_734	/*** ERR_MONLISTFULL ***/	"%d %s :Monitor list is full"
#define NUMERIC_STR_740	/*** RPL_RSACHALLENGE2 ***/	":%s"
#define NUMERIC_STR_741	/*** RPL_ENDOFRSACHALLENGE2 */	":End of CHALLENGE"

#define RPL_WELCOME	     1
#define RPL_YOURHOST	     2
#define RPL_CREATED	     3
#define RPL_MYINFO	     4
#define RPL_ISUPPORT	     5

#define RPL_REDIR	     10
#define RPL_MAP		     15	/* Undernet extension */
#define RPL_MAPMORE	     16	/* Undernet extension */
#define RPL_MAPEND	     17	/* Undernet extension */
#define RPL_SAVENICK	     43	/* From ircnet */

/*
 * Numeric replies from server commands.
 * These are currently in the range 200-399.
 */
#define RPL_TRACELINK	     200
#define RPL_TRACECONNECTING  201
#define RPL_TRACEHANDSHAKE   202
#define RPL_TRACEUNKNOWN     203
#define RPL_TRACEOPERATOR    204
#define RPL_TRACEUSER	     205
#define RPL_TRACESERVER	     206
#define RPL_TRACENEWTYPE     208
#define RPL_TRACECLASS	     209

#define RPL_STATSLINKINFO    211
#define RPL_STATSCOMMANDS    212
#define RPL_STATSCLINE	     213
#define RPL_STATSNLINE	     214
#define RPL_STATSILINE	     215
#define RPL_STATSKLINE	     216
#define RPL_STATSQLINE	     217
#define RPL_STATSYLINE	     218
#define RPL_ENDOFSTATS	     219
/* note ircu uses 217 for STATSPLINE frip. conflict
 * as RPL_STATSQLINE was used in old 2.8 for Q line 
 * I'm going to steal 220 for now *sigh*
 * -Dianora
 */
#define RPL_STATSPLINE	     220
#define RPL_UMODEIS	     221

#define RPL_STATSFLINE	     224
#define RPL_STATSDLINE	     225

#define RPL_SERVLIST	     234
#define RPL_SERVLISTEND	     235

#define RPL_STATSLLINE	     241
#define RPL_STATSUPTIME	     242
#define RPL_STATSOLINE	     243
#define RPL_STATSHLINE	     244
/* 245 No longer used in ircd-ratbox */
#define RPL_STATSSLINE	     245
#define RPL_STATSXLINE	     247
#define RPL_STATSULINE	     248
#define RPL_STATSDEBUG	     249
#define RPL_STATSCONN	     250
#define RPL_LUSERCLIENT	     251
#define RPL_LUSEROP	     252
#define RPL_LUSERUNKNOWN     253
#define RPL_LUSERCHANNELS    254
#define RPL_LUSERME	     255
#define RPL_ADMINME	     256
#define RPL_ADMINLOC1	     257
#define RPL_ADMINLOC2	     258
#define RPL_ADMINEMAIL	     259

#define RPL_TRACELOG	     261
#define RPL_ENDOFTRACE	     262
#define RPL_LOAD2HI	     263

#define RPL_LOCALUSERS	     265
#define RPL_GLOBALUSERS	     266
#define RPL_WHOISCERTFP      276

#define RPL_ACCEPTLIST	     281
#define RPL_ENDOFACCEPT	     282

/* numeric_replies */

#define RPL_NONE	     300
#define RPL_AWAY	     301
#define RPL_USERHOST	     302
#define RPL_ISON	     303
#define RPL_TEXT	     304
#define RPL_UNAWAY	     305
#define RPL_NOWAWAY	     306

/*	RPL_WHOISADMIN	     308 -- hybrid */

#define RPL_WHOISUSER	     311
#define RPL_WHOISSERVER	     312
#define RPL_WHOISOPERATOR    313

#define RPL_WHOWASUSER	     314
/* rpl_endofwho below (315) */
#define RPL_ENDOFWHOWAS	     369

#define RPL_WHOISCHANOP	     316	/* redundant and not needed but reserved */
#define RPL_WHOISIDLE	     317

#define RPL_ENDOFWHOIS	     318
#define RPL_WHOISCHANNELS    319

#define RPL_LISTSTART	     321
#define RPL_LIST	     322
#define RPL_LISTEND	     323
#define RPL_CHANNELMODEIS    324

#define RPL_CREATIONTIME     329
#define RPL_WHOISLOGGEDIN    330

#define RPL_NOTOPIC	     331
#define RPL_TOPIC	     332
#define RPL_TOPICWHOTIME     333
#define RPL_WHOISACTUALLY    338

#define RPL_INVITING	     341
#define RPL_SUMMONING	     342

#define RPL_INVITELIST	     346
#define RPL_ENDOFINVITELIST  347
#define RPL_EXCEPTLIST	     348
#define RPL_ENDOFEXCEPTLIST  349

#define RPL_VERSION	     351

#define RPL_WHOREPLY	     352
#define RPL_ENDOFWHO	     315
#define RPL_NAMREPLY	     353
#define RPL_ENDOFNAMES	     366

#define RPL_KILLDONE	     361
#define RPL_CLOSING	     362
#define RPL_CLOSEEND	     363
#define RPL_LINKS	     364
#define RPL_ENDOFLINKS	     365
/* rpl_endofnames above (366) */
#define RPL_BANLIST	     367
#define RPL_ENDOFBANLIST     368
/* rpl_endofwhowas above (369) */

#define RPL_INFO	     371
#define RPL_MOTD	     372
#define RPL_INFOSTART	     373
#define RPL_ENDOFINFO	     374
#define RPL_MOTDSTART	     375
#define RPL_ENDOFMOTD	     376

#define RPL_YOUREOPER	     381
#define RPL_REHASHING	     382
#define RPL_MYPORTIS	     384
#define RPL_NOTOPERANYMORE   385
#define RPL_RSACHALLENGE     386

#define RPL_TIME	     391
#define RPL_USERSSTART	     392
#define RPL_USERS	     393
#define RPL_ENDOFUSERS	     394
#define RPL_NOUSERS	     395

/*
 * Errors are in the range from 400-599 currently and are grouped by what
 * commands they come from.
 */
#define ERR_NOSUCHNICK	     401
#define ERR_NOSUCHSERVER     402
#define ERR_NOSUCHCHANNEL    403
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_TOOMANYCHANNELS  405
#define ERR_WASNOSUCHNICK    406
#define ERR_TOOMANYTARGETS   407
#define ERR_NOORIGIN	     409

#define ERR_INVALIDCAPCMD    410

#define ERR_NORECIPIENT	     411
#define ERR_NOTEXTTOSEND     412
#define ERR_NOTOPLEVEL	     413
#define ERR_WILDTOPLEVEL     414

#define ERR_TOOMANYMATCHES   416

#define ERR_UNKNOWNCOMMAND   421
#define ERR_NOMOTD	     422
#define ERR_NOADMININFO	     423
#define ERR_FILEERROR	     424

#define ERR_NONICKNAMEGIVEN  431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE    433
#define ERR_NICKCOLLISION    436
#define ERR_UNAVAILRESOURCE  437
#define ERR_NICKTOOFAST	     438	/* We did it first Undernet! ;) db */

#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL     442
#define ERR_USERONCHANNEL    443
#define ERR_NOLOGIN	     444
#define ERR_SUMMONDISABLED   445
#define ERR_USERSDISABLED    446

#define ERR_NOTREGISTERED    451

#define ERR_ACCEPTFULL	     456
#define ERR_ACCEPTEXIST	     457
#define ERR_ACCEPTNOT	     458

#define ERR_NEEDMOREPARAMS   461
#define ERR_ALREADYREGISTRED 462
#define ERR_NOPERMFORHOST    463
#define ERR_PASSWDMISMATCH   464
#define ERR_YOUREBANNEDCREEP 465
#define ERR_YOUWILLBEBANNED  466
#define ERR_KEYSET	     467

#define ERR_CHANNELISFULL    471
#define ERR_UNKNOWNMODE	     472
#define ERR_INVITEONLYCHAN   473
#define ERR_BANNEDFROMCHAN   474
#define ERR_BADCHANNELKEY    475
#define ERR_BADCHANMASK	     476
#define ERR_NEEDREGGEDNICK   477
#define ERR_BANLISTFULL	     478	/* I stole the numeric from ircu -db */
#define ERR_BADCHANNAME	     479
#define ERR_SSLONLYCHAN	     480
#define ERR_NOPRIVILEGES     481
#define ERR_CHANOPRIVSNEEDED 482
#define ERR_CANTKILLSERVER   483
#define ERR_ISCHANSERVICE    484
/* #define ERR_RESTRICTED	484	- hyb derived, no longer here */
#define ERR_BANNEDNICK	     485

#define ERR_VOICENEEDED		489

#define ERR_NOOPERHOST	     491

#define ERR_UMODEUNKNOWNFLAG 501
#define ERR_USERSDONTMATCH   502

#define ERR_GHOSTEDCLIENT    503

#define ERR_USERNOTONSERV    504

/* #define ERR_LAST_ERR_MSG 505 
 * moved to 999
 */
#define ERR_WRONGPONG	     513

#define ERR_HELPNOTFOUND     524

#define RPL_WHOISSECURE	     671

#define RPL_MODLIST	     702
#define RPL_ENDOFMODLIST     703

#define RPL_HELPSTART	     704
#define RPL_HELPTXT	     705
#define RPL_ENDOFHELP	     706

#define ERR_TARGCHANGE		707

#define RPL_ETRACEFULL	     708
#define RPL_ETRACE	     709

#define RPL_KNOCK	     710
#define RPL_KNOCKDLVR	     711

#define ERR_TOOMANYKNOCK     712
#define ERR_CHANOPEN	     713
#define ERR_KNOCKONCHAN	     714
#define ERR_KNOCKDISABLED    715

#define ERR_TARGUMODEG	     716
#define RPL_TARGNOTIFY	     717
#define RPL_UMODEGMSG	     718

#define RPL_OMOTDSTART	     720
#define RPL_OMOTD	     721
#define RPL_ENDOFOMOTD	     722

#define ERR_NOPRIVS		723

#define RPL_TESTMASK		724
#define RPL_TESTLINE		725
#define RPL_NOTESTLINE		726
#define RPL_TESTMASKGECOS	727

#define RPL_MONONLINE		730
#define RPL_MONOFFLINE		731
#define RPL_MONLIST		732
#define RPL_ENDOFMONLIST	733
#define ERR_MONLISTFULL		734

#define RPL_RSACHALLENGE2	740
#define RPL_ENDOFRSACHALLENGE2	741

#define ERR_LAST_ERR_MSG     999


#endif
