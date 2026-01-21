/* contrib/m_force.c
 * Copyright (C) 1996-2002 Hybrid Development Team
 * Copyright (C) 2004-2012 ircd-ratbox Development Team
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  1.Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  2.Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  3.The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 *  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdinc.h>
#include <ratbox_lib.h>
#include <struct.h>
#include <channel.h>
#include <class.h>
#include <client.h>
#include <match.h>
#include <ircd.h>
#include <hostmask.h>
#include <numeric.h>
#include <s_conf.h>
#include <s_newconf.h>
#include <s_log.h>
#include <send.h>
#include <hash.h>
#include <s_serv.h>
#include <parse.h>
#include <modules.h>


static int mo_forcejoin(struct Client *client_p, struct Client *source_p,
			int parc, const char *parv[]);
static int mo_forcepart(struct Client *client_p, struct Client *source_p,
			int parc, const char *parv[]);

struct Message forcejoin_msgtab = {
	.cmd = "FORCEJOIN", 
	.handlers[UNREGISTERED_HANDLER] =       { mm_unreg },
	.handlers[CLIENT_HANDLER] =             { mm_not_oper },
	.handlers[RCLIENT_HANDLER] =            { mm_ignore },
	.handlers[SERVER_HANDLER] =             { mm_ignore },
	.handlers[ENCAP_HANDLER] =              { mm_ignore },
	.handlers[OPER_HANDLER] =               { .handler = mo_forcejoin, .min_para = 3 },
};

struct Message forcepart_msgtab = {
	.cmd = "FORCEPART", 
	.handlers[UNREGISTERED_HANDLER] =       { mm_unreg },
	.handlers[CLIENT_HANDLER] =             { mm_not_oper },
	.handlers[RCLIENT_HANDLER] =            { mm_ignore },
	.handlers[SERVER_HANDLER] =             { mm_ignore },
	.handlers[ENCAP_HANDLER] =              { mm_ignore },
	.handlers[OPER_HANDLER] =               { .handler = mo_forcepart, .min_para = 3 },
};

mapi_clist_av1 force_clist[] = { &forcejoin_msgtab, &forcepart_msgtab, NULL };

DECLARE_MODULE_AV1(force, NULL, NULL, force_clist, NULL, NULL, "$Revision$");

/*
 * m_forcejoin
 *      parv[0] = sender prefix
 *      parv[1] = user to force
 *      parv[2] = channel to force them into
 */
static int
mo_forcejoin(struct Client *client_p, struct Client *source_p, int parc, const char *parv[])
{
	struct Client *target_p;
	struct Channel *chptr;
	int type;
	char mode;
	char sjmode;
	char *newch;

	if(!IsOperAdmin(source_p))
	{
		sendto_one_numeric(source_p, s_RPL(ERR_NOPRIVS), "forcejoin");
		return 0;
	}

	if((hunt_server(client_p, source_p, ":%s FORCEJOIN %s %s", 1, parc, parv)) != HUNTED_ISME)
		return 0;

	/* if target_p is not existant, print message
	 * to source_p and bail - scuzzy
	 */
	if((target_p = find_client(parv[1])) == NULL)
	{
		sendto_one_numeric(source_p, ERR_NOSUCHNICK, form_str(ERR_NOSUCHNICK), parv[1]);
		return 0;
	}

	if(!IsClient(target_p))
		return 0;

	/* select our modes from parv[2] if they exist... (chanop) */
	if(*parv[2] == '@')
	{
		type = CHFL_CHANOP;
		mode = 'o';
		sjmode = '@';
	}
	else if(*parv[2] == '+')
	{
		type = CHFL_VOICE;
		mode = 'v';
		sjmode = '+';
	}
	else
	{
		type = CHFL_PEON;
		mode = sjmode = '\0';
	}

	if(mode != '\0')
		parv[2]++;

	if((chptr = find_channel(parv[2])) != NULL)
	{
		if(IsMember(target_p, chptr))
		{
			/* debugging is fun... */
			sendto_one(source_p, ":%s NOTICE %s :*** Notice -- %s is already in %s",
				   me.name, source_p->name, target_p->name, chptr->chname);
			return 0;
		}

		add_user_to_channel(chptr, target_p, type);

		sendto_server(target_p, chptr, NOCAPS, NOCAPS,
			      ":%s SJOIN %ld %s + :%c%s",
			      me.name, (long)chptr->channelts,
			      chptr->chname, type ? sjmode : ' ', target_p->name);

		sendto_channel_local(ALL_MEMBERS, chptr, ":%s!%s@%s JOIN :%s",
				     target_p->name, target_p->username,
				     target_p->host, chptr->chname);

		if(type)
			sendto_channel_local(ALL_MEMBERS, chptr, ":%s MODE %s +%c %s",
					     me.name, chptr->chname, mode, target_p->name);

		if(chptr->topic != NULL)
		{
			sendto_one_numeric(target_p, s_RPL(RPL_TOPIC), chptr->chname, chptr->topic->topic);
			sendto_one_numeric(target_p, s_RPL(RPL_TOPICWHOTIME),
				   chptr->chname,
				   chptr->topic->topic_info, chptr->topic->topic_time);
		}

		channel_member_names(chptr, target_p, 1);
	}
	else
	{
		newch = LOCAL_COPY(parv[2]);
		if(!check_channel_name(newch))
		{
			sendto_one_numeric(source_p, ERR_BADCHANNAME, form_str(ERR_BADCHANNAME), newch);
			return 0;
		}

		/* channel name must begin with & or # */
		if(!IsChannelName(newch))
		{
			sendto_one_numeric(source_p, ERR_BADCHANNAME, form_str(ERR_BADCHANNAME), newch);
			return 0;
		}

		/* newch can't be longer than CHANNELLEN */
		if(strlen(newch) > CHANNELLEN)
		{
			sendto_one(source_p, ":%s NOTICE %s :Channel name is too long", me.name,
				   source_p->name);
			return 0;
		}

		chptr = get_or_create_channel(target_p, newch, NULL);
		add_user_to_channel(chptr, target_p, CHFL_CHANOP);

		/* send out a join, make target_p join chptr */
		sendto_server(target_p, chptr, NOCAPS, NOCAPS,
			      ":%s SJOIN %ld %s +nt :@%s", me.name,
			      (long)chptr->channelts, chptr->chname, target_p->name);

		sendto_channel_local(ALL_MEMBERS, chptr, ":%s!%s@%s JOIN :%s",
				     target_p->name, target_p->username,
				     target_p->host, chptr->chname);

		chptr->mode.mode |= MODE_TOPICLIMIT;
		chptr->mode.mode |= MODE_NOPRIVMSGS;

		sendto_channel_local(ALL_MEMBERS, chptr, ":%s MODE %s +nt", me.name, chptr->chname);

		target_p->localClient->last_join_time = rb_current_time();
		channel_member_names(chptr, target_p, 1);

		/* we do this to let the oper know that a channel was created, this will be
		 * seen from the server handling the command instead of the server that
		 * the oper is on.
		 */
		sendto_one(source_p, ":%s NOTICE %s :*** Notice -- Creating channel %s", me.name,
			   source_p->name, chptr->chname);
	}
	return 0;
}


static int
mo_forcepart(struct Client *client_p, struct Client *source_p, int parc, const char *parv[])
{
	struct Client *target_p;
	struct Channel *chptr;
	struct membership *msptr;

	if(!IsOperAdmin(source_p))
	{
		sendto_one_numeric(source_p, s_RPL(ERR_NOPRIVS), "forcepart");
		return 0;
	}

	if((hunt_server(client_p, source_p, ":%s FORCEPART %s %s", 1, parc, parv)) != HUNTED_ISME)
		return 0;

	/* if target_p == NULL then let the oper know */
	if((target_p = find_client(parv[1])) == NULL)
	{
		sendto_one_numeric(source_p, ERR_NOSUCHNICK, form_str(ERR_NOSUCHNICK), parv[1]);
		return 0;
	}

	if(!IsClient(target_p))
		return 0;


	if((chptr = find_channel(parv[2])) == NULL)
	{
		sendto_one_numeric(source_p, ERR_NOSUCHCHANNEL,
				   form_str(ERR_NOSUCHCHANNEL), parv[1]);
		return 0;
	}

	if((msptr = find_channel_membership(chptr, target_p)) == NULL)
	{
		sendto_one_numeric(source_p, ERR_USERNOTINCHANNEL, form_str(ERR_USERNOTINCHANNEL),
			   parv[1], parv[2]);
		return 0;
	}

	sendto_server(target_p, chptr, NOCAPS, NOCAPS,
		      ":%s PART %s :%s", target_p->name, chptr->chname, target_p->name);

	sendto_channel_local(ALL_MEMBERS, chptr, ":%s!%s@%s PART %s :%s",
			     target_p->name, target_p->username,
			     target_p->host, chptr->chname, target_p->name);


	remove_user_from_channel(msptr);

	return 0;
}
