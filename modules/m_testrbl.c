/* modules/m_testrbl.c
 *
 *  Copyright (C) 2026 ircd-ratbox development team
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1.Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * 2.Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3.The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "stdinc.h"
#include "ratbox_lib.h"
#include "struct.h"
#include "client.h"
#include "send.h"
#include "modules.h"
#include "parse.h"
#include "numeric.h"
#include "s_auth.h"

static int mo_testrbl(struct Client *, struct Client *, int, const char **);

struct Message testrbl_msgtab = {
	.cmd = "TESTRBL",
	.handlers[UNREGISTERED_HANDLER] =	{ mm_unreg },
	.handlers[CLIENT_HANDLER] =		{ mm_not_oper },
	.handlers[RCLIENT_HANDLER] =		{ mm_ignore },
	.handlers[SERVER_HANDLER] =		{ mm_ignore },
	.handlers[ENCAP_HANDLER] =		{ mm_ignore },
	.handlers[OPER_HANDLER] =		{ .handler = mo_testrbl, .min_para = 2 },
};

mapi_clist_av1 testrbl_clist[] = { &testrbl_msgtab, NULL };

DECLARE_MODULE_AV1(testrbl, NULL, NULL, testrbl_clist, NULL, NULL, "$Revision$");

static int
mo_testrbl(struct Client *client_p, struct Client *source_p, int parc, const char *parv[])
{
	struct rb_sockaddr_storage ip;
	const char *ipstr = parv[1];

	memset(&ip, 0, sizeof(ip));

	if(rb_inet_pton_sock(ipstr, (struct sockaddr *)&ip) <= 0)
	{
		sendto_one_notice(source_p, ":TESTRBL: %s is not a valid IP address", ipstr);
		return 0;
	}

	rbl_run_test(source_p, (struct sockaddr *)&ip, ipstr);
	return 0;
}
