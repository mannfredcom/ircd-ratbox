/*
 *  OpenServices 1.0
 *  Base Structure and parsing tools.
 *
 *  Copyright (C) 2005 Alan "alz" Milford
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *  USA
 */

#ifndef INCLUDED_services_h
#define INCLUDED_services_h


typedef enum {
	SVC_PRIVMSG,
	SVC_NOTICE
} svc_type_t;


typedef int (*SVCMessageHandler) (struct Client *, struct Client *, int, const char *[], int);
#define SVC_MSG_HASH 512
/* service structure */
struct Service
{
	rb_dlink_node node;
	rb_dlink_list command_tbl[SVC_MSG_HASH]; 
	SVCMessageHandler unknown;
	struct Client *client_p;
};

/* Message table structure */
struct SVCMessage
{
	const char *cmd;
	int min_para;		/* minimum number of params */
	const char *help;

	SVCMessageHandler handler;
};

//service message function pointer
typedef int (*sfn) (void *data);

struct Service *create_service(const char *nick, const char *username, const char *host,
			       const char *gecos, bool opered);
void destroy_service(struct Service *service_p);

int handle_services_message(hook_service_message_data *);
void parse_services_message(struct Client *client_p, struct Client *target_p, const char *text,
			    int length, int direct);
void process_unknown_command(struct Client *client_p, struct Client *target_p, int i,
			     const char *hpara[MAXPARA], int direct);
int is_svc_command(struct Service *service_p, struct SVCMessage *msg);
void svc_add_cmd(struct Service *service_p, struct SVCMessage *msg);
void svc_del_cmd(struct Service *service_p, struct SVCMessage *msg);
void svc_message(struct Service *service, struct Client *target_p, svc_type_t type, const char *pattern,
		 ...);
struct Service *find_service(struct Client *client_p);
void try_command(struct Client *source_p, struct Service *service_p, const char *cmd);
void svc_set_unknown(struct Service *service_p, SVCMessageHandler unknown);
struct SVCMessage *svc_get_cmd(struct Service *service_p, char *cmd);

void init_fake_services(void);
#endif

extern struct Client *create_fake_client(const char *, const char *, const char *, const char *, bool opered);
extern void destroy_fake_client(struct Client *);
extern struct Client *create_fake_server(const char *, const char *, int);
extern void destroy_fake_server(struct Client *, int);

