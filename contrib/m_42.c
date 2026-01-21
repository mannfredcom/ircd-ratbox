/*
 *   Copyright (C) infinity-infinity God <God@Heaven>
 *
 *   Bob was here 
 */

#include <stdinc.h>
#include <ratbox_lib.h>
#include <struct.h>
#include <parse.h>
#include <modules.h>
#include <client.h>
#include <ircd.h>
#include <send.h>

static int mclient_42(struct Client *client_p, struct Client *source_p, int parc,
		      const char *parv[]);
static int mclient_kilroy(struct Client *client_p, struct Client *source_p, int parc,
			  const char *parv[]);

struct Message hgtg_msgtab = {
	.cmd = "42",
	.handlers[UNREGISTERED_HANDLER] =       { mm_ignore },
	.handlers[CLIENT_HANDLER] =             { mclient_42 },
	.handlers[RCLIENT_HANDLER] =            { mm_ignore },
	.handlers[SERVER_HANDLER] =             { mm_ignore },
	.handlers[ENCAP_HANDLER] =              { mm_ignore },
	.handlers[OPER_HANDLER] =               { mclient_42 },
};

struct Message kilroy_msgtab = {
	.cmd = "KILROY",
	.handlers[UNREGISTERED_HANDLER] =       { mm_ignore },
	.handlers[CLIENT_HANDLER] =             { mclient_kilroy },
	.handlers[RCLIENT_HANDLER] =            { mm_ignore },
	.handlers[SERVER_HANDLER] =             { mm_ignore },
	.handlers[ENCAP_HANDLER] =              { mm_ignore },
	.handlers[OPER_HANDLER] =               { mclient_kilroy },
};


mapi_clist_av1 hgtg_clist[] = { &hgtg_msgtab, &kilroy_msgtab, NULL };


DECLARE_MODULE_AV1(42, NULL, NULL, hgtg_clist, NULL, NULL, "Revision 0.42");


static int
mclient_42(struct Client *client_p, struct Client *source_p, int parc, const char *parv[])
{
	sendto_one(source_p, ":%s NOTICE %s :The Answer to Life, the Universe, and Everything.",
		   me.name, source_p->name);
	return 0;
}

static int
mclient_kilroy(struct Client *client_p, struct Client *source_p, int parc, const char *parv[])
{
	SetCork(source_p);
	sendto_one(source_p, ":%s NOTICE %s :                ___              ", me.name,
		   source_p->name);
	sendto_one(source_p, ":%s NOTICE %s :___________mm__(O O)__mm_________", me.name,
		   source_p->name);
	sendto_one(source_p, ":%s NOTICE %s :           \"\"    U    \"\"         ", me.name,
		   source_p->name);
	ClearCork(source_p);
	sendto_one(source_p, ":%s NOTICE %s :Kilroy was here", me.name, source_p->name);
	return 0;
}
