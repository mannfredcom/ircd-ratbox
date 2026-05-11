/*
 *  ircd-ratbox: A slightly useful ircd.
 *  s_auth.c: Functions for querying a users ident.
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
 *   */

/*
 * Changes:
 *   July 6, 1999 - Rewrote most of the code here. When a client connects
 *     to the server and passes initial socket validation checks, it
 *     is owned by this module (auth) which returns it to the rest of the
 *     server when dns and auth queries are finished. Until the client is
 *     released, the server does not know it exists and does not process
 *     any messages from it.
 *     --Bleep	Thomas Helvey <tomh@inxpress.net>
 */
#include "stdinc.h"
#include "setup.h"
#include "ratbox_lib.h"
#include "struct.h"
#include "s_auth.h"
#include "s_conf.h"
#include "client.h"
#include "match.h"
#include "ircd.h"
#include "numeric.h"
#include "packet.h"
#include "s_log.h"
#include "s_stats.h"
#include "send.h"
#include "hook.h"
#include "dns.h"
#include "substitution.h"

#define RBL_FLAG_ISV4 0x1	
#define RBL_FLAG_ISV6 0x2
#define RBL_FLAG_FREEING 0x4
#define RBL_FLAG_MATCH_OTHER 0x8

#define rbl_setv4(x) ((x)->flags |= RBL_FLAG_ISV4)
#define rbl_clearv4(x) ((x)->flags &= ~RBL_FLAG_ISV4)
#define rbl_isv4(x) ((x)->flags & RBL_FLAG_ISV4)

#define rbl_setv6(x) ((x)->flags |= RBL_FLAG_ISV6)
#define rbl_clearv6(x) ((x)->flags &= ~RBL_FLAG_ISV6)
#define rbl_isv6(x) ((x)->flags & RBL_FLAG_ISV6)

#define rbl_setfreeing(x) ((x)->flags |= RBL_FLAG_FREEING)
#define rbl_clearfreeing(x) ((x)->flags &= ~RBL_FLAG_FREEING)
#define rbl_isfreeing(x) ((x)->flags & RBL_FLAG_FREEING)

#define rbl_setmatchother(x) ((x)->flags |= RBL_FLAG_MATCH_OTHER)
#define rbl_clearmatchother(x) ((x)->flags &= ~RBL_FLAG_MATCH_OTHER)
#define rbl_ismatchother(x) ((x)->flags & RBL_FLAG_MATCH_OTHER)


struct AuthRequest
{
	rb_dlink_node node;
	struct Client *client;	/* pointer to client struct for request */
	uint32_t dns_query;	/* DNS Query */
	rb_dlink_list rbl_queries;
	rb_fde_t *authF;
	unsigned int flags;	/* current state of request */
	time_t timeout;		/* time when query expires */
	int lport;
	int rport;
};

typedef struct _rblquery
{
        rb_dlink_node node;
        struct AuthRequest *auth;
        rbl_t *rbl;
        uint32_t queryid;
} rblquery_t;


typedef struct _rbl_answer
{
	rb_dlink_node node;
	char *answer;
	char *mask;
} rbl_answer_t;



struct _rbl
{
	rb_dlink_node node;
	char *rblname;
	char *mo_answer;
	rb_dlink_list answers;
	int refcount;
	uint8_t flags;
	unsigned long queries;
	unsigned long matches;
	unsigned long misses;
};

typedef enum
{
	REPORT_DO_DNS,
	REPORT_FIN_DNS,
	REPORT_FAIL_DNS,
	REPORT_DO_ID,
	REPORT_FIN_ID,
	REPORT_FAIL_ID,
	REPORT_HOST_TOOLONG,
	REPORT_DO_RBL,
	REPORT_RBL_MATCH,
	REPORT_FIN_RBL,
	
}
ReportType;

static const char *HeaderMessages[] = {
	[REPORT_DO_DNS] = "NOTICE AUTH :*** Looking up your hostname...",
	[REPORT_FIN_DNS] = "NOTICE AUTH :*** Found your hostname",
	[REPORT_FAIL_DNS] = "NOTICE AUTH :*** Couldn't look up your hostname",
	[REPORT_DO_ID] = "NOTICE AUTH :*** Checking Ident",
	[REPORT_FIN_ID] = "NOTICE AUTH :*** Got Ident response",
	[REPORT_FAIL_ID] = "NOTICE AUTH :*** No Ident response",
	[REPORT_HOST_TOOLONG] = "NOTICE AUTH :*** Your hostname is too long, ignoring hostname",
        [REPORT_DO_RBL] = "NOTICE AUTH :*** Checking RBLs",
        [REPORT_RBL_MATCH] = "NOTICE AUTH :*** Matched RBL Check....",
        [REPORT_FIN_RBL] = "NOTICE AUTH :*** RBL checks finished"
};

#define sendheader(c, r) sendto_one(c, "%s", HeaderMessages[(r)])

static rb_dlink_list auth_poll_list;
static rb_dlink_list rbl_lists;

static EVH timeout_auth_queries_event;
static void read_auth(rb_fde_t * F, void *data);
static void rbl_check_rbls(struct AuthRequest *auth);
static void rbl_cancel_lookups(struct AuthRequest *);
static void rbl_free_answer(rbl_answer_t *res);

/*
 * init_auth()
 *
 * Initialise the auth code
 */
void
init_auth(void)
{
	memset(&auth_poll_list, 0, sizeof(auth_poll_list));
	rb_event_addish("timeout_auth_queries_event", timeout_auth_queries_event, NULL, 3);

}

/*
 * make_auth_request - allocate a new auth request
 */
static struct AuthRequest *
make_auth_request(struct Client *client)
{
	struct AuthRequest *request = rb_malloc(sizeof(struct AuthRequest));
	client->localClient->auth_request = request;
	request->client = client;
	request->dns_query = 0;
	request->authF = NULL;
	request->timeout = rb_current_time() + ConfigFileEntry.connect_timeout;
	return request;
}

/*
 * release_auth_client - release auth client from auth system
 * this adds the client into the local client lists so it can be read by
 * the main io processing loop
 */
static void
release_auth_client(struct AuthRequest *auth)
{
	struct Client *client = auth->client;

	if(IsDNS(auth) || IsAuth(auth) || IsRBL(auth))
		return;

	client->localClient->auth_request = NULL;
	rb_dlinkDelete(&auth->node, &auth_poll_list);
	rb_free(auth);

	/*
	 * When a client has auth'ed, we want to start reading what it sends
	 * us. This is what read_packet() does.
	 *     -- adrian
	 */
	client->localClient->allow_read = MAX_FLOOD;
	rb_dlinkAddTail(client, &client->node, &global_client_list);
	read_packet(client->localClient->F, client);
}

/*
 * auth_dns_callback - called when resolver query finishes
 * if the query resulted in a successful search, hp will contain
 * a non-null pointer, otherwise hp will be null.
 * set the client on it's way to a connection completion, regardless
 * of success of failure
 */
static void
auth_dns_callback(const char *res, int status, int aftype, void *data)
{
	struct AuthRequest *auth = data;
	ClearDNS(auth);
	auth->dns_query = 0;

	if(status == 1 && strlen(res) < HOSTLEN)
	{
		rb_strlcpy(auth->client->host, res, sizeof(auth->client->host));
		sendheader(auth->client, REPORT_FIN_DNS);
	}
	else
	{
		if(!strcmp(res, "HOSTTOOLONG"))
		{
			sendheader(auth->client, REPORT_HOST_TOOLONG);
		}
		sendheader(auth->client, REPORT_FAIL_DNS);
	}
	release_auth_client(auth);

}

/*
 * authsenderr - handle auth send errors
 */
static void
auth_error(struct AuthRequest *auth)
{
	ServerStats.is_abad++;

	if(auth->authF != NULL)
		rb_close(auth->authF);
	auth->authF = NULL;
	ClearAuth(auth);
	sendheader(auth->client, REPORT_FAIL_ID);
	release_auth_client(auth);
}

static void
auth_connect_callback(rb_fde_t * F, int status, void *data)
{
	struct AuthRequest *auth = data;
	char authbuf[32];

	if(status != RB_OK)
	{
		auth_error(auth);
		return;
	}

	/* one shot at the send, socket buffers should be able to handle it
	 * if not, oh well, you lose
	 */
	snprintf(authbuf, sizeof(authbuf), "%d , %d\r\n", auth->rport, auth->lport);
	if(rb_write(auth->authF, authbuf, strlen(authbuf)) <= 0)
	{
		auth_error(auth);
		return;
	}
	read_auth(F, auth);
}


/*
 * start_auth_query - Flag the client to show that an attempt to 
 * contact the ident server on
 * the client's host.  The connect and subsequently the socket are all put
 * into 'non-blocking' mode.  Should the connect or any later phase of the
 * identifing process fail, it is aborted and the user is given a username
 * of "unknown".
 */
static void
start_auth_query(struct AuthRequest *auth)
{
	struct rb_sockaddr_storage *localaddr;
	struct rb_sockaddr_storage *remoteaddr;
	struct rb_sockaddr_storage destaddr;
	struct rb_sockaddr_storage bindaddr;
	int family;

	if(IsAnyDead(auth->client))
		return;

	sendheader(auth->client, REPORT_DO_ID);

	localaddr = auth->client->localClient->lip;
	remoteaddr = &auth->client->localClient->ip;

	family = GET_SS_FAMILY(remoteaddr);

	if((auth->authF = rb_socket(family, SOCK_STREAM, 0, "ident")) == NULL)
	{
		sendto_realops_flags(UMODE_DEBUG, L_ALL, "Error creating auth stream socket: %s", strerror(errno));
		ilog(L_IOERROR, "creating auth stream socket %s: %s", auth->client->sockhost, strerror(errno));
		auth_error(auth);
		return;
	}
	memcpy(&bindaddr, localaddr, sizeof(struct rb_sockaddr_storage));
	memcpy(&destaddr, remoteaddr, sizeof(struct rb_sockaddr_storage));

#ifdef RB_IPV6
	if(family == AF_INET6)
	{
		auth->lport = ntohs(((struct sockaddr_in6 *)localaddr)->sin6_port);
		auth->rport = ntohs(((struct sockaddr_in6 *)remoteaddr)->sin6_port);
		((struct sockaddr_in6 *)&bindaddr)->sin6_port = 0;
		((struct sockaddr_in6 *)&destaddr)->sin6_port = htons(113);

	}
	else
#endif
	{
		auth->lport = ntohs(((struct sockaddr_in *)localaddr)->sin_port);
		auth->rport = ntohs(((struct sockaddr_in *)remoteaddr)->sin_port);
		((struct sockaddr_in *)&bindaddr)->sin_port = 0;
		((struct sockaddr_in *)&destaddr)->sin_port = htons(113);
	}

	/* allocated in listener.c - after we copy this..we can discard it */
	rb_free(auth->client->localClient->lip);
	auth->client->localClient->lip = NULL;

	rb_connect_tcp(auth->authF, (struct sockaddr *)&destaddr, (struct sockaddr *)&bindaddr,
		       GET_SS_LEN(&destaddr), auth_connect_callback, auth, GlobalSetOptions.ident_timeout);

	return;
}

static char *
get_valid_ident(char *xbuf)
{
	int remp = 0;
	int locp = 0;
	char *colon1Ptr;
	char *colon2Ptr;
	char *colon3Ptr;
	char *commaPtr;
	char *remotePortString;

	/* All this to get rid of a sscanf() fun. */
	remotePortString = xbuf;

	colon1Ptr = strchr(remotePortString, ':');
	if(!colon1Ptr)
		return NULL;

	*colon1Ptr = '\0';
	colon1Ptr++;
	colon2Ptr = strchr(colon1Ptr, ':');
	if(!colon2Ptr)
		return NULL;

	*colon2Ptr = '\0';
	colon2Ptr++;
	commaPtr = strchr(remotePortString, ',');

	if(!commaPtr)
		return NULL;

	*commaPtr = '\0';
	commaPtr++;

	remp = atoi(remotePortString);
	if(!remp)
		return NULL;

	locp = atoi(commaPtr);
	if(!locp)
		return NULL;

	/* look for USERID bordered by first pair of colons */
	if(!strstr(colon1Ptr, "USERID"))
		return NULL;

	colon3Ptr = strchr(colon2Ptr, ':');
	if(!colon3Ptr)
		return NULL;

	*colon3Ptr = '\0';
	colon3Ptr++;
	return (colon3Ptr);
}

/*
 * start_auth - starts auth (identd) and dns queries for a client
 */
void
start_auth(struct Client *client)
{
	struct AuthRequest *auth = NULL;
	s_assert(NULL != client);
	if(client == NULL)
		return;

	/* to aid bopm which needs something unique to match against */
	sendto_one(client, "NOTICE AUTH :*** Processing connection to %s", me.name);

	auth = make_auth_request(client);

	sendheader(client, REPORT_DO_DNS);

	rb_dlinkAdd(auth, &auth->node, &auth_poll_list);

	/* Note that the order of things here are done for a good reason
	 * if you try to do start_auth_query before lookup_ip there is a 
	 * good chance that you'll end up with a double free on the auth
	 * and that is bad.  But you still must keep the SetDNSPending 
	 * before the call to start_auth_query, otherwise you'll have
	 * the same thing.  So think before you hack 
	 */
	SetDNS(auth);		/* set both at the same time to eliminate possible race conditions */
	SetAuth(auth);
	SetRBL(auth);
	
        rbl_check_rbls(auth);
	
	if(ConfigFileEntry.disable_auth == 0)
	{
		start_auth_query(auth);
	}
	else
	{
		rb_free(client->localClient->lip);
		client->localClient->lip = NULL;
		ClearAuth(auth);
	}
	auth->dns_query = lookup_ip(client->sockhost, GET_SS_FAMILY(&client->localClient->ip), auth_dns_callback, auth);
}

/*
 * timeout_auth_queries - timeout resolver and identd requests
 * allow clients through if requests failed
 */
static void
timeout_auth_queries_event(void *notused)
{
	rb_dlink_node *ptr;
	rb_dlink_node *next_ptr;

	RB_DLINK_FOREACH_SAFE(ptr, next_ptr, auth_poll_list.head)
	{
		struct AuthRequest *auth = ptr->data;

		if(auth->timeout < rb_current_time())
		{
			if(auth->authF != NULL)
			{
				rb_close(auth->authF);
				auth->authF = NULL;
			}
			if(IsAuth(auth))
			{
				ClearAuth(auth);
				ServerStats.is_abad++;
				sendheader(auth->client, REPORT_FAIL_ID);
			}
			if(IsDNS(auth))
			{
				ClearDNS(auth);
				cancel_lookup(auth->dns_query);
				auth->dns_query = 0;
				sendheader(auth->client, REPORT_FAIL_DNS);
			}
			if(IsRBL(auth))
			{
			        ClearRBL(auth);
			        rbl_cancel_lookups(auth);
			        sendheader(auth->client, REPORT_FIN_RBL);
			}

			auth->client->localClient->lasttime = rb_current_time();
			release_auth_client(auth);
		}
	}
	return;
}

#define AUTH_BUFSIZ 128
static void
read_auth(rb_fde_t * F, void *data)
{
	struct AuthRequest *auth = data;
	char *s = NULL, *t;
	char buf[AUTH_BUFSIZ + 1];
	int len, count;

	len = rb_read(auth->authF, buf, AUTH_BUFSIZ);

	if(len < 0 && rb_ignore_errno(errno))
	{
		rb_setselect(F, RB_SELECT_READ, read_auth, auth);
		return;
	}

	if(len > 0)
	{
		buf[len] = '\0';
		if((s = get_valid_ident(buf)))
		{
			t = auth->client->username;
			while(*s == '~' || *s == '^')
				s++;
			for(count = USERLEN; *s && count; s++)
			{
				if(*s == '@')
					break;
				if(!isspace(*s) && *s != ':' && *s != '[')
				{
					*t++ = *s;
					count--;
				}
			}
			*t = '\0';
		}
	}

	rb_close(auth->authF);
	auth->authF = NULL;
	ClearAuth(auth);

	if(s == NULL)
	{
		++ServerStats.is_abad;
		rb_strlcpy(auth->client->username, "unknown", sizeof(auth->client->username));
		sendheader(auth->client, REPORT_FAIL_ID);
	}
	else
	{
		sendheader(auth->client, REPORT_FIN_ID);
		++ServerStats.is_asuc;
		SetGotId(auth->client);
	}

	release_auth_client(auth);
}

/* this assumes the client is closing */
void
delete_auth_queries(struct Client *target_p)
{
	struct AuthRequest *auth;
	if(target_p == NULL || target_p->localClient == NULL || target_p->localClient->auth_request == NULL)
		return;
	auth = target_p->localClient->auth_request;
	target_p->localClient->auth_request = NULL;

	if(IsDNS(auth) && auth->dns_query > 0)
	{
		cancel_lookup(auth->dns_query);
		auth->dns_query = 0;
	}

	if(IsRBL(auth))
                rbl_cancel_lookups(auth);	        

	if(auth->authF != NULL)
		rb_close(auth->authF);

	rb_dlinkDelete(&auth->node, &auth_poll_list);
	rb_free(auth);
}

void
rbl_set_aftype(rbl_t *t, bool isv4, bool isv6)
{
	if(isv4 == true)
		rbl_setv4(t);
	else
		rbl_clearv4(t);

	if(isv6 == true)
		rbl_setv6(t);
	else
		rbl_clearv6(t);
}

rbl_t *
rbl_create(const char *zonename)
{
	rbl_t *t;
	t = rb_malloc(sizeof(rbl_t));
	rbl_setv4(t);
        t->rblname = rb_strdup(zonename);
	return t;
}

void
rbl_set_match_other(rbl_t *t, bool other_reply)
{
	if(other_reply == true)
		rbl_setmatchother(t);
	else
		rbl_clearmatchother(t);
}

void
rbl_add_other_answer(rbl_t *t, const char *reason)
{
	rb_free(t->mo_answer);
	t->mo_answer = rb_strdup(reason);
	rbl_setmatchother(t); /* make sure this gets done */
}

void
rbl_add_answer(rbl_t *t, const char *mask, const char *answer)
{
	rbl_answer_t *res;

	if(EmptyString(mask) || EmptyString(answer))
		return;

	res = rb_malloc(sizeof(rbl_answer_t));

	res->mask = rb_strdup(mask);
	res->answer = rb_strdup(answer);
	rb_dlinkAdd(res, &res->node, &t->answers);
}

static void
rbl_free_answer(rbl_answer_t *answer)
{
	rb_free(answer->answer);
	rb_free(answer->mask);
        rb_free(answer);
}

void
rbl_destroy(rbl_t *t, bool freeing)
{
	/* caller wants the list destroyed - conf parser usually */
	if(freeing == true) 
	{
		rbl_setfreeing(t);
	}
	
	if(t->refcount > 0)
		return;
			
	rb_dlink_node *ptr, *next;
	RB_DLINK_FOREACH_SAFE(ptr, next, t->answers.head)
	{
		rbl_answer_t *answer = ptr->data;
		rb_dlinkDelete(&answer->node, &t->answers);
		rbl_free_answer(answer);
	}
	rb_dlinkDelete(&t->node, &rbl_lists);
	rb_free(t->rblname);
	rb_free(t->mo_answer);
	rb_free(t);
}

static char *
rbl_string_v4(const struct sockaddr *addr, const char *domain, char *dst, size_t dstsz)
{
	const uint8_t *cp;
	size_t ret;
	if(addr->sa_family != AF_INET)
	        return NULL;

        cp = (const uint8_t *)&((const struct sockaddr_in *)addr)->sin_addr.s_addr;
	ret = snprintf(dst, dstsz, "%u.%u.%u.%u.%s", (unsigned int)cp[3], (unsigned int)cp[2], 
			(unsigned int)cp[1], (unsigned int)cp[0], domain);
	if(ret >= dstsz)
		return NULL;
	return dst;
}

static char *
rbl_string_v6(const struct sockaddr *addr, const char *domain, char *dst, size_t dstsz)
{
	const uint8_t *cp;
	size_t ret;
	if(addr->sa_family != AF_INET6)
		return NULL;

	cp = (const uint8_t *)&((const struct sockaddr_in6 *)addr)->sin6_addr.s6_addr;
	
	ret = snprintf(dst, dstsz, 
			"%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x."
			"%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%s",
                           (unsigned int)(cp[15] & 0xf), (unsigned int)(cp[15] >> 4),
                           (unsigned int)(cp[14] & 0xf), (unsigned int)(cp[14] >> 4),
                           (unsigned int)(cp[13] & 0xf), (unsigned int)(cp[13] >> 4),
                           (unsigned int)(cp[12] & 0xf), (unsigned int)(cp[12] >> 4),
                           (unsigned int)(cp[11] & 0xf), (unsigned int)(cp[11] >> 4),
                           (unsigned int)(cp[10] & 0xf), (unsigned int)(cp[10] >> 4),
                           (unsigned int)(cp[9] & 0xf), (unsigned int)(cp[9] >> 4),
                           (unsigned int)(cp[8] & 0xf), (unsigned int)(cp[8] >> 4),
                           (unsigned int)(cp[7] & 0xf), (unsigned int)(cp[7] >> 4),
                           (unsigned int)(cp[6] & 0xf), (unsigned int)(cp[6] >> 4),
                           (unsigned int)(cp[5] & 0xf), (unsigned int)(cp[5] >> 4),
                           (unsigned int)(cp[4] & 0xf), (unsigned int)(cp[4] >> 4),
                           (unsigned int)(cp[3] & 0xf), (unsigned int)(cp[3] >> 4),
                           (unsigned int)(cp[2] & 0xf), (unsigned int)(cp[2] >> 4),
                           (unsigned int)(cp[1] & 0xf), (unsigned int)(cp[1] >> 4),
                           (unsigned int)(cp[0] & 0xf), (unsigned int)(cp[0] >> 4),
                           domain);
	if(ret >= dstsz)
		return NULL;
					
	return dst;	
}


static char *
rbl_string(const struct sockaddr *addr, const char *domain, char *dst, size_t dstsz)
{
	switch(addr->sa_family)
	{
		case AF_INET:
			return rbl_string_v4(addr, domain, dst, dstsz);
		case AF_INET6:
			return rbl_string_v6(addr, domain, dst, dstsz);
		default:
			return NULL;
	}
}

static void
rbl_release_auth(struct AuthRequest *auth)
{
	if(rb_dlink_list_length(&auth->rbl_queries) > 0)
		return;
	
	ClearRBL(auth);
	sendheader(auth->client, REPORT_FIN_RBL);
	release_auth_client(auth);
}

static void
rbl_attach_rbl_to_query(rblquery_t *query, rbl_t *t)
{
        t->refcount++;
        query->rbl = t;	
}

static void
rbl_detach_rbl_from_query(rblquery_t *query)
{
	rbl_t *t;
	t = query->rbl;
	t->refcount--; 
        query->rbl = NULL;
        if(rbl_isfreeing(t) && t->refcount == 0)
        	rbl_destroy(t, false);
}

static void
rbl_set_banned(struct AuthRequest *auth, const char *rblname, const char *answer)
{
	rb_dlink_list varlist = { NULL, NULL, 0 };
	struct Client *source_p = auth->client;
	substitution_append_var(&varlist, "nick", EmptyString(source_p->name) ? "(unset)" : source_p->name);
	substitution_append_var(&varlist, "ip", source_p->sockhost);
	substitution_append_var(&varlist, "host", EmptyString(source_p->host) ? source_p->sockhost : source_p->host);
	substitution_append_var(&varlist, "dnsbl-host", rblname);
	substitution_append_var(&varlist, "network-name", ServerInfo.network_name);
	rb_free(source_p->localClient->rblreason);
	source_p->localClient->rblreason = rb_strdup(substitution_parse(answer, &varlist));
	substitution_free(&varlist);
	SetRBLBanned(source_p);
}

static void
rbl_dns_callback(const char *result, int status, int aftype, void *data)
{
        rblquery_t *query = data;
	struct AuthRequest *auth;
	struct Client *client;
	struct in_addr in;
	rb_dlink_node *ptr;
	bool matched = false;

	auth = query->auth;
	client = auth->client;

	if(status != 1 || aftype != AF_INET)
	        goto cleanup;

	if(rb_inet_pton(AF_INET, result, &in) != 1)
	        goto cleanup;

        RB_DLINK_FOREACH(ptr, query->rbl->answers.head)
        {
                rbl_answer_t *res = ptr->data;
                const char *mask = res->mask;

                /* match just the last octet if single digit */
		if(IsDigit(mask[0]) && mask[1] == '\0')
		{
			uint8_t val = (uint8_t)atoi(mask);
			uint8_t c = ((uint8_t *)&in.s_addr)[3];
			if(c == val)
			{
				rbl_set_banned(auth, query->rbl->rblname, res->answer);
				matched = true;
				goto cleanup;
			}
		}
		if(match(mask, result) || match_ips(mask, result))
		{
			rbl_set_banned(auth, query->rbl->rblname, res->answer);
			matched = true;
			goto cleanup;
		}
        }

	if(rbl_ismatchother(query->rbl))
	{
		const char *reason = query->rbl->mo_answer;

		if(EmptyString(reason))
			reason = "IP Address: ${ip} banned by RBL";
		rbl_set_banned(auth, query->rbl->rblname, reason);
		matched = true;
	}

cleanup:
	if(matched)
		query->rbl->matches++;
	else
		query->rbl->misses++;
        rbl_detach_rbl_from_query(query);
	rb_dlinkDelete(&query->node, &auth->rbl_queries);
        rb_free(query);
        rbl_release_auth(auth);

}

#define RBL_HOSTLEN 255
static void
rbl_check_rbls(struct AuthRequest *auth)
{
        char hostbuf[RBL_HOSTLEN];
	rb_dlink_node *ptr, *next;

	if(rb_dlink_list_length(&rbl_lists) <= 0)
	{
	        ClearRBL(auth);
	        return;
	}

	sendheader(auth->client, REPORT_DO_RBL);
                
	RB_DLINK_FOREACH_SAFE(ptr, next, rbl_lists.head)
	{
		rblquery_t *query;
		rbl_t *t = ptr->data;
		int safamily = GET_SS_FAMILY(&auth->client->localClient->ip);

		if(safamily == AF_INET6 && rbl_isv6(t) == false)
			continue;

		if(safamily == AF_INET && rbl_isv4(t) == false)
			continue;
			
		if(rbl_string((struct sockaddr *)&auth->client->localClient->ip, t->rblname, hostbuf, sizeof(hostbuf)) == NULL)
			continue;

                query = rb_malloc(sizeof(rblquery_t));
                query->auth = auth;
                rbl_attach_rbl_to_query(query, t);
                rb_dlinkAdd(query, &query->node, &auth->rbl_queries);
		/* Bump counter BEFORE lookup_hostname. lookup_hostname() can
		 * synchronously fire rbl_dns_callback via failed_resolver()
		 * when the resolver helper is unavailable; the callback
		 * detaches the query which decrements t->refcount, and if the
		 * zone was rbl_isfreeing() (e.g. a rehash removed it while an
		 * earlier query still held a ref) reaching refcount==0 at that
		 * point triggers rbl_destroy(t, false). A post-lookup t->queries
		 * bump would then dereference freed memory. */
		t->queries++;
		query->queryid = lookup_hostname(hostbuf, AF_INET, rbl_dns_callback, query);
        }

        if(rb_dlink_list_length(&auth->rbl_queries) <= 0)
        {
        	sendheader(auth->client, REPORT_FIN_RBL);
        	ClearRBL(auth);
	}
}

void
rbl_add_rbl_to_rbllists(rbl_t *rbl)
{
        rb_dlinkAdd(rbl, &rbl->node, &rbl_lists);
}

void
rbl_del_rbl_from_rblists(rbl_t *rbl)
{
        rb_dlinkFindDelete(rbl, &rbl_lists);
}

void
rbl_clear_rbllists(void)
{
        rb_dlink_node *ptr, *next;
        RB_DLINK_FOREACH_SAFE(ptr, next, rbl_lists.head)
        {
        	rbl_t *rbl = ptr->data;
        	rbl_destroy(rbl, true);
        }
}

void
rbl_dump_stats(rbl_stats_cb cb, void *arg)
{
	rb_dlink_node *ptr;
	RB_DLINK_FOREACH(ptr, rbl_lists.head)
	{
		rbl_t *t = ptr->data;
		cb(t->rblname, t->queries, t->matches, t->misses, arg);
	}
}

/* TESTRBL: oper-initiated spot-check of a single IP against all RBL zones.
 * Reuses the RBL infrastructure but does NOT mutate auth state; this is
 * a diagnostic path, not a real connection-time check. */

#define TESTRBL_TIMEOUT 10

typedef struct _testrbl_session
{
	char source_uid[IDLEN + 1];
	char target_str[HOSTIPLEN];
	rb_dlink_list queries;
	int pending;
	rb_ev_entry *timer;
	bool cancelled;
	bool dispatching;
} testrbl_session_t;

typedef struct _testrbl_query
{
	rb_dlink_node node;
	testrbl_session_t *session;
	rbl_t *rbl;
	uint32_t queryid;
	char zonename[256];
	bool sync_completed;
} testrbl_query_t;

static void testrbl_dns_callback(const char *result, int status, int aftype, void *data);
static void testrbl_timeout_cb(void *data);
static void testrbl_finish_if_done(testrbl_session_t *s);

static struct Client *
testrbl_live_source(testrbl_session_t *s)
{
	if(s->cancelled)
		return NULL;
	return find_id(s->source_uid);
}

static void
testrbl_free_query(testrbl_query_t *q)
{
	testrbl_session_t *s = q->session;
	rb_dlinkDelete(&q->node, &s->queries);
	q->rbl->refcount--;
	if(rbl_isfreeing(q->rbl) && q->rbl->refcount == 0)
		rbl_destroy(q->rbl, false);
	rb_free(q);
}

void
rbl_run_test(struct Client *source_p, const struct sockaddr *addr, const char *ipstr)
{
	char hostbuf[RBL_HOSTLEN];
	testrbl_session_t *s;
	rb_dlink_node *ptr;
	int dispatched = 0;

	if(rb_dlink_list_length(&rbl_lists) == 0)
	{
		sendto_one_notice(source_p,
				  ":TESTRBL %s: no RBL zones configured", ipstr);
		return;
	}

	s = rb_malloc(sizeof(*s));
	rb_strlcpy(s->source_uid, source_p->id, sizeof(s->source_uid));
	rb_strlcpy(s->target_str, ipstr, sizeof(s->target_str));
	/* Guard reference: lookup_hostname() can complete synchronously via
	 * failed_resolver() when the resolver helper is unavailable, firing
	 * testrbl_dns_callback() mid-loop. Without this guard a synchronous
	 * last-query would drive s->pending to 0 and free the session under
	 * our feet. The guard is released after the dispatch loop.
	 */
	s->pending = 1;
	/* While this flag is set, the callback marks q->sync_completed
	 * instead of freeing q, so we can safely write q->queryid after
	 * lookup_hostname() returns even on the synchronous-fail path.
	 */
	s->dispatching = true;

	RB_DLINK_FOREACH(ptr, rbl_lists.head)
	{
		rbl_t *t = ptr->data;
		testrbl_query_t *q;
		uint32_t qid;

		if(GET_SS_FAMILY(addr) == AF_INET && !rbl_isv4(t))
			continue;
		if(GET_SS_FAMILY(addr) == AF_INET6 && !rbl_isv6(t))
			continue;

		if(rbl_string(addr, t->rblname, hostbuf, sizeof(hostbuf)) == NULL)
			continue;

		q = rb_malloc(sizeof(*q));
		q->session = s;
		q->rbl = t;
		t->refcount++;
		rb_strlcpy(q->zonename, t->rblname, sizeof(q->zonename));
		rb_dlinkAdd(q, &q->node, &s->queries);
		s->pending++;
		dispatched++;
		qid = lookup_hostname(hostbuf, AF_INET, testrbl_dns_callback, q);
		if(q->sync_completed)
		{
			/* Callback fired synchronously; clean up here rather
			 * than in the callback, so we never write through
			 * a dangling q pointer. */
			testrbl_free_query(q);
			s->pending--;
		}
		else
		{
			q->queryid = qid;
		}
	}

	s->dispatching = false;

	if(dispatched == 0)
	{
		sendto_one_notice(source_p,
				  ":TESTRBL %s: no applicable zones for this address family",
				  ipstr);
		/* Guard is the only outstanding reference; drop it and free. */
		s->pending = 0;
		rb_free(s);
		return;
	}

	sendto_one_notice(source_p,
			  ":TESTRBL %s: dispatched %d %s, %ds timeout",
			  s->target_str, dispatched,
			  dispatched == 1 ? "query" : "queries",
			  TESTRBL_TIMEOUT);

	/* Schedule timeout before releasing the guard. If every query
	 * completed synchronously during the dispatch loop, releasing the
	 * guard will drive pending to 0 and testrbl_finish_if_done() will
	 * cancel this timer and free s. Otherwise the timer protects any
	 * outstanding async queries.
	 */
	s->timer = rb_event_addonce("testrbl_timeout", testrbl_timeout_cb, s, TESTRBL_TIMEOUT);

	/* Release guard. May free s; do not touch it after this. */
	s->pending--;
	testrbl_finish_if_done(s);
}

static void
testrbl_dns_callback(const char *result, int status, int aftype, void *data)
{
	testrbl_query_t *q = data;
	testrbl_session_t *s = q->session;
	struct Client *source_p = testrbl_live_source(s);
	struct in_addr in;
	rb_dlink_node *ptr;
	const char *reason = NULL;

	if(source_p == NULL)
		goto cleanup;

	if(status != 1)
	{
		/* Two failure paths feed this callback with status!=1:
		 *
		 *  - failed_resolver() in src/dns.c, invoked when the resolver
		 *    helper itself is unavailable. Calls cb("FAILED", 0, 0, ...)
		 *    DIRECTLY, bypassing results_callback and its aftype
		 *    coercion. This is the only path where aftype == 0 reaches
		 *    us, so that is a reliable ERROR signal.
		 *
		 *  - Normal NXDOMAIN / no-answer via the resolver helper.
		 *    resolver/resolver.c:send_answer emits result="FAILED",
		 *    result_code=0, aftype=0, but src/dns.c:results_callback
		 *    coerces aftype=0 to AF_INET before calling us. So checking
		 *    result=="FAILED" would over-match onto CLEAN zones.
		 */
		if(aftype == 0)
			sendto_one_notice(source_p,
					  ":TESTRBL %s: %s - ERROR (resolver helper unavailable)",
					  s->target_str, q->zonename);
		else
			sendto_one_notice(source_p,
					  ":TESTRBL %s: %s - CLEAN (no listing)",
					  s->target_str, q->zonename);
		goto cleanup;
	}

	if(aftype != AF_INET)
	{
		/* Defensive: we always query AF_INET, so a non-INET reply is
		 * a protocol oddity rather than a normal RBL response. */
		sendto_one_notice(source_p,
				  ":TESTRBL %s: %s - unexpected address family in reply",
				  s->target_str, q->zonename);
		goto cleanup;
	}

	if(rb_inet_pton(AF_INET, result, &in) != 1)
	{
		sendto_one_notice(source_p,
				  ":TESTRBL %s: %s - malformed response (%s)",
				  s->target_str, q->zonename, result);
		goto cleanup;
	}

	RB_DLINK_FOREACH(ptr, q->rbl->answers.head)
	{
		rbl_answer_t *res = ptr->data;
		const char *mask = res->mask;

		if(IsDigit(mask[0]) && mask[1] == '\0')
		{
			uint8_t val = (uint8_t)atoi(mask);
			uint8_t c = ((uint8_t *)&in.s_addr)[3];
			if(c == val)
			{
				reason = res->answer;
				break;
			}
		}
		if(match(mask, result) || match_ips(mask, result))
		{
			reason = res->answer;
			break;
		}
	}

	if(reason == NULL && rbl_ismatchother(q->rbl))
	{
		reason = EmptyString(q->rbl->mo_answer)
			? "IP Address: ${ip} banned by RBL"
			: q->rbl->mo_answer;
	}

	if(reason != NULL)
	{
		rb_dlink_list varlist = { NULL, NULL, 0 };
		const char *expanded;

		substitution_append_var(&varlist, "nick", "(test)");
		substitution_append_var(&varlist, "ip", s->target_str);
		substitution_append_var(&varlist, "host", s->target_str);
		substitution_append_var(&varlist, "dnsbl-host", q->zonename);
		substitution_append_var(&varlist, "network-name", ServerInfo.network_name);
		expanded = substitution_parse(reason, &varlist);

		sendto_one_notice(source_p,
				  ":TESTRBL %s: %s - MATCH reply=%s - %s",
				  s->target_str, q->zonename, result, expanded);
		substitution_free(&varlist);
	}
	else
	{
		sendto_one_notice(source_p,
				  ":TESTRBL %s: %s - reply=%s (no configured match rule hit)",
				  s->target_str, q->zonename, result);
	}

cleanup:
	if(s->dispatching)
	{
		/* Synchronous invocation from inside rbl_run_test's dispatch
		 * loop (failed_resolver path). Do not free q or decrement
		 * pending here — the caller checks sync_completed and cleans
		 * up after lookup_hostname() returns, avoiding a write-after-
		 * free when the caller stores q->queryid.
		 */
		q->sync_completed = true;
		return;
	}
	testrbl_free_query(q);
	s->pending--;
	testrbl_finish_if_done(s);
}

static void
testrbl_finish_if_done(testrbl_session_t *s)
{
	struct Client *source_p;

	if(s->pending > 0)
		return;

	source_p = testrbl_live_source(s);
	if(source_p != NULL)
		sendto_one_notice(source_p,
				  ":TESTRBL %s: End of /TESTRBL", s->target_str);

	if(s->timer != NULL)
	{
		rb_event_delete(s->timer);
		s->timer = NULL;
	}
	rb_free(s);
}

static void
testrbl_timeout_cb(void *data)
{
	testrbl_session_t *s = data;
	struct Client *source_p;
	rb_dlink_node *ptr, *next;

	s->timer = NULL;
	s->cancelled = true;
	source_p = find_id(s->source_uid);

	RB_DLINK_FOREACH_SAFE(ptr, next, s->queries.head)
	{
		testrbl_query_t *q = ptr->data;
		if(source_p != NULL)
			sendto_one_notice(source_p,
					  ":TESTRBL %s: %s - TIMEOUT after %ds",
					  s->target_str, q->zonename, TESTRBL_TIMEOUT);
		cancel_lookup(q->queryid);
		testrbl_free_query(q);
	}

	if(source_p != NULL)
		sendto_one_notice(source_p,
				  ":TESTRBL %s: End of /TESTRBL (with timeouts)",
				  s->target_str);

	rb_free(s);
}

static void
rbl_cancel_lookups(struct AuthRequest *auth)
{
        rb_dlink_node *ptr, *next;
        
        RB_DLINK_FOREACH_SAFE(ptr, next, auth->rbl_queries.head)
        {
                rblquery_t *query = ptr->data;
                cancel_lookup(query->queryid);
                rbl_detach_rbl_from_query(query);
                rb_dlinkDelete(&query->node, &auth->rbl_queries);
                rb_free(query);
        } 
}


