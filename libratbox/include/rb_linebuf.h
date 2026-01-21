/*
 *  ircd-ratbox: A slightly useful ircd.
 *  linebuf.h: A header for the linebuf code.
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

#ifndef RB_LIB_H
# error "Do not use rb_linebuf.h directly"
#endif

#ifndef RB_LINEBUF_H__
#define RB_LINEBUF_H__

enum {
	LINEBUF_COMPLETE = 0,
	LINEBUF_PARTIAL = 1,
};

enum {
	LINEBUF_PARSED	= 0,
	LINEBUF_RAW = 1
};

struct _rb_buf_line;

struct _rb_buf_head
{
	rb_dlink_list list;	/* the actual dlink list */
	size_t len;		/* length of all the data */
	size_t alloclen;	/* Actual allocated data length */
	size_t writeofs;	/* offset in the first line for the write */
	unsigned int numlines;	/* number of lines */
};



typedef struct _rb_buf_line rb_buf_line_t;
typedef struct _rb_buf_head rb_buf_head_t;

size_t rb_linebuf_len(rb_buf_head_t *);
size_t rb_linebuf_alloclen(rb_buf_head_t *);
unsigned int rb_linebuf_numlines(rb_buf_head_t *);



void rb_linebuf_init(void);
void rb_linebuf_newbuf(rb_buf_head_t *);
void rb_linebuf_donebuf(rb_buf_head_t *);
int rb_linebuf_parse(rb_buf_head_t *, char *, size_t, bool);
ssize_t rb_linebuf_get(rb_buf_head_t * bufhead, char *buf, size_t buflen, bool partial, bool raw);
void rb_linebuf_putmsg(rb_buf_head_t *, const char *, va_list *, const char *, ...);
void rb_linebuf_put(rb_buf_head_t *, const char *, ...);
void rb_linebuf_putbuf(rb_buf_head_t * bufhead, const char *buffer);
void rb_linebuf_attach(rb_buf_head_t *, rb_buf_head_t *);
void rb_count_rb_linebuf_memory(size_t *, size_t *);
ssize_t rb_linebuf_flush(rb_fde_t *F, rb_buf_head_t *);
rb_buf_head_t *rb_linebuf_bufhead_alloc(void);
void rb_linebuf_bufhead_free(rb_buf_head_t *);

#endif 
