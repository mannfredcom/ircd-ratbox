/*   contrib/m_ojoin.c
 *   Copyright (C) 2002 Hybrid Development Team
 *   Copyright (C) 2004-2012 ircd-ratbox Development Team
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdinc.h>
#include <ratbox_lib.h>
#include <struct.h>
#include <channel.h>
#include <client.h>
#include <ircd.h>
#include <numeric.h>
#include <s_log.h>
#include <s_serv.h>
#include <s_newconf.h>
#include <send.h>
#include <whowas.h>
#include <match.h>
#include <hash.h>
#include <parse.h>
#include <modules.h>


static int mo_ojoin(struct Client *client_p, struct Client *source_p, int parc, const char *parv[]);


struct Message ojoin_msgtab = {
	.cmd = "OJOIN",
	.handlers[UNREGISTERED_HANDLER] =       { mm_unreg },
	.handlers[CLIENT_HANDLER] =             { mm_not_oper },
	.handlers[RCLIENT_HANDLER] =            { mm_ignore },
	.handlers[SERVER_HANDLER] =             { mm_ignore },
	.handlers[ENCAP_HANDLER] =              { mm_ignore },
	.handlers[OPER_HANDLER] =               { .handler = mo_ojoin, .min_para = 2 },
};

mapi_clist_av1 ojoin_clist[] = { &ojoin_msgtab, NULL };

DECLARE_MODULE_AV1(ojoin, NULL, NULL, ojoin_clist, NULL, NULL, "$Revision$");

/*
** mo_ojoin
**      parv[0] = sender prefix
**      parv[1] = channel
*/
static int
mo_ojoin(struct Client *client_p, struct Client *source_p, int parc, const char *parv[])
{
	struct Channel *chptr;
	int move_me = 0;

	/* admins only */
	if(!IsOperAdmin(source_p))
	{
		sendto_one_numeric(source_p, s_RPL(ERR_NOPRIVS), "ojoin");
		return 0;
	}


	if(*parv[1] == '@' || *parv[1] == '+')
	{
		parv[1]++;
		move_me = 1;
	}

	if((chptr = find_channel(parv[1])) == NULL)
	{
		sendto_one_numeric(source_p, s_RPL(ERR_NOSUCHCHANNEL), parv[1]);
		return 0;
	}

	if(IsMember(source_p, chptr))
	{
		sendto_one(source_p, ":%s NOTICE %s :Please part %s before using OJOIN",
			   me.name, source_p->name, parv[1]);
		return 0;
	}

	if(move_me == 1)
		parv[1]--;

	if(*parv[1] == '@')
	{
		add_user_to_channel(chptr, source_p, CHFL_CHANOP);
		sendto_server(client_p, chptr, NOCAPS, NOCAPS,
			      ":%s SJOIN %ld %s + :@%s",
			      me.name, (long)chptr->channelts, chptr->chname, source_p->name);
		sendto_channel_local(ALL_MEMBERS, chptr, ":%s!%s@%s JOIN %s",
				     source_p->name,
				     source_p->username, source_p->host, chptr->chname);
		sendto_channel_local(ALL_MEMBERS, chptr, ":%s MODE %s +o %s",
				     me.name, chptr->chname, source_p->name);

	}
	else if(*parv[1] == '+')
	{
		add_user_to_channel(chptr, source_p, CHFL_VOICE);
		sendto_server(client_p, chptr, NOCAPS, NOCAPS,
			      ":%s SJOIN %ld %s + :+%s",
			      me.name, (long)chptr->channelts, chptr->chname, source_p->name);
		sendto_channel_local(ALL_MEMBERS, chptr, ":%s!%s@%s JOIN %s",
				     source_p->name,
				     source_p->username, source_p->host, chptr->chname);
		sendto_channel_local(ALL_MEMBERS, chptr, ":%s MODE %s +v %s",
				     me.name, chptr->chname, source_p->name);
	}
	else
	{
		add_user_to_channel(chptr, source_p, CHFL_PEON);
		sendto_server(client_p, chptr, NOCAPS, NOCAPS,
			      ":%s SJOIN %ld %s + :%s",
			      me.name, (long)chptr->channelts, chptr->chname, source_p->name);
		sendto_channel_local(ALL_MEMBERS, chptr, ":%s!%s@%s JOIN %s",
				     source_p->name,
				     source_p->username, source_p->host, chptr->chname);
	}

	/* send the topic... */
	if(chptr->topic != NULL)
	{
		sendto_one_numeric(source_p, s_RPL(RPL_TOPIC), chptr->chname, chptr->topic->topic);
		sendto_one_numeric(source_p, s_RPL(RPL_TOPICWHOTIME),
			   chptr->chname, chptr->topic->topic_info,
			   chptr->topic->topic_time);
	}

	source_p->localClient->last_join_time = rb_current_time();
	channel_member_names(chptr, source_p, 1);

	return 0;
}
