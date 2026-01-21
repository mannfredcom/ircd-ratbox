/*
 *  ircd-ratbox: A slightly useful ircd.
 *  commio-ssl.h: A header for the ssl code
 *
 *  Copyright (C) 2008-2026 ircd-ratbox development team
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

#ifndef _COMMIO_SSL_H
#define _COMMIO_SSL_H

/* These are functions internal to libratbox */

int rb_init_ssl(void);

int rb_ssl_listen(rb_fde_t *F, int backlog, bool defer_accept);
void rb_ssl_accept_setup(rb_fde_t *F, rb_fde_t *new_F, struct sockaddr *st, rb_socklen_t addrlen);
void rb_get_ssl_info(char *buf, size_t length);
void rb_ssl_shutdown(rb_fde_t *F);
ssize_t rb_ssl_read(rb_fde_t *F, void *buf, size_t count);
ssize_t rb_ssl_write(rb_fde_t *F, const void *buf, size_t count);


#endif
