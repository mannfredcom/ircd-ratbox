/*
 *  ircd-ratbox: A slightly useful ircd.
 *  rawbuf.h: A header for rawbuf.c
 *
 *  Copyright (C) 2007 Aaron Sethman <androsyn@ratbox.org>
 *  Copyright (C) 2007-2026 ircd-ratbox development team
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

#ifndef RB_LIB_H
# error "Do not use rawbuf.h directly"
#endif

#ifndef INCLUDED_RAWBUF_H__
#define INCLUDED_RAWBUF_H__



typedef struct _rb_rawbuf rb_rawbuf_t;
typedef struct _rb_rawbuf_head rb_rawbuf_head_t;

void rb_init_rawbuffers(void);
void rb_free_rawbuffer(rb_rawbuf_head_t *);
rb_rawbuf_head_t *rb_new_rawbuffer(void);
size_t rb_rawbuf_get(rb_rawbuf_head_t *, void *data, size_t len);
void rb_rawbuf_append(rb_rawbuf_head_t *, void *data, size_t len);
ssize_t rb_rawbuf_flush(rb_rawbuf_head_t *, rb_fde_t *F);
size_t rb_rawbuf_length(rb_rawbuf_head_t * rb);

#endif
