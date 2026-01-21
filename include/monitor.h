/* 
 * ircd-ratbox: an advanced Internet Relay Chat Daemon(ircd).
 * monitor.h: Code for server-side notify lists.
 *
 * Copyright (C) 2005 Lee Hardy <lee -at- leeh.co.uk>
 * Copyright (C) 2005-2026 ircd-ratbox development team
 */
#ifndef INCLUDED_monitor_h
#define INCLUDED_monitor_h

#include <hash.h>

struct monitor
{
	hash_node *hnode;
	rb_dlink_list users;
	char *name;
};

struct monitor *find_monitor(const char *name, bool add);
void free_monitor(struct monitor *);

void clear_monitor(struct Client *);

void monitor_signon(struct Client *);
void monitor_signoff(struct Client *);

#endif
