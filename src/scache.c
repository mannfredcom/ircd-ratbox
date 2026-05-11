/*
 *  ircd-ratbox: A slightly useful ircd.
 *  scache.c: Server names cache.
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
#include "stdinc.h"
#include "ratbox_lib.h"
#include "match.h"
#include "ircd.h"
#include "numeric.h"
#include "send.h"
#include "hash.h"
#include "scache.h"

/*
 * Each interned name is stored as [ size_t refcount | char name[] ] in a
 * single allocation.  Callers receive a pointer to the name portion.
 * scache_remove() decrements the refcount and only frees when it reaches
 * zero, keeping the string alive as long as any holder (struct Client,
 * gline_pending, …) still references it.
 */

static size_t scache_allocated = 0;

/* Return a pointer to the refcount word that precedes the interned string.
 * The (uintptr_t) round-trip silences -Wcast-qual: the const-strip is
 * deliberate (the refcount header is mutable; only the trailing name
 * portion is exposed to callers as const). */
static inline size_t *
scache_refcount(const char *sc)
{
	return (size_t *)(uintptr_t)(sc - sizeof(size_t));
}

const char *
scache_add(const char *name)
{
	char *sc;
	size_t *rc;
	size_t len;

	if(EmptyString(name))
		return NULL;

	len = strlen(name) + 1;

	if((sc = hash_find_data_len(HASH_SCACHE, name, len)) != NULL)
	{
		(*scache_refcount(sc))++;
		return sc;
	}

	rc = rb_malloc(sizeof(size_t) + len);
	*rc = 1;
	sc = (char *)(rc + 1);
	memcpy(sc, name, len);
	scache_allocated += sizeof(size_t) + len;

	hash_add_len(HASH_SCACHE, sc, len, sc);
	return sc;
}

void
scache_remove(const char *name)
{
	char *sc;
	size_t *rc;
	size_t len;

	if(EmptyString(name))
		return;

	len = strlen(name) + 1;
	sc = hash_find_data_len(HASH_SCACHE, name, len);
	if(sc == NULL)
		return;

	rc = scache_refcount(sc);
	if(--(*rc) > 0)
		return;

	hash_del_len(HASH_SCACHE, sc, len, sc);
	scache_allocated -= sizeof(size_t) + len;
	rb_free(rc);
}

void
count_scache(size_t *number, size_t *mem)
{
	hash_get_memusage(HASH_SCACHE, number, mem);
	(*mem) += scache_allocated;
}
