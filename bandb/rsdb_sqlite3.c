/* src/rsdb_sqlite.h
 *   Contains the code for the sqlite database backend.
 *
 * Copyright (C) 2003-2006 Lee Hardy <leeh@leeh.co.uk>
 * Copyright (C) 2003-2026 ircd-ratbox development team
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
#include "rsdb.h"

#include <sqlite3.h>


#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#if (((__GNUC__ * 100) + __GNUC_MINOR__) >= 406)
#pragma GCC diagnostic push
#endif
static void
mlog(rsdb_conn_t *dbconn, const char *errstr, ...)
{
	if(dbconn != NULL && dbconn->error_cb != NULL)
	{
		char buf[256];
		va_list ap;
		va_start(ap, errstr);
		vsnprintf(buf, sizeof(buf), errstr, ap);
		va_end(ap);
		dbconn->error_cb(buf, dbconn->error_cb_data);
	}
	else
		exit(1);
}
#if (((__GNUC__ * 100) + __GNUC_MINOR__) >= 406)
#pragma GCC diagnostic pop
#endif

rsdb_conn_t *
rsdb_init(const char *dbpath, rsdb_error_cb * ecb, void *data)
{
	rsdb_conn_t *conn;
	conn = rb_malloc(sizeof(rsdb_conn_t));


	if(sqlite3_open(dbpath, &conn->ptr) != SQLITE_OK)
	{
		mlog(conn, "Unable to open sqlite database: %s", sqlite3_errmsg(conn->ptr));
		rb_free(conn->ptr);
		rb_free(conn);
		return NULL;
	}
	if(access(dbpath, W_OK))
	{
		mlog(conn, "Unable to open sqlite database for write: %s", strerror(errno));
		rb_free(conn->ptr);
		rb_free(conn);
		return NULL;			
	}
	conn->error_cb = ecb;
	conn->error_cb_data = data;
	return conn;
}

void
rsdb_shutdown(rsdb_conn_t *dbconn)
{
	if(dbconn->ptr != NULL)
		sqlite3_close(dbconn->ptr);
}


static int
rsdb_callback_func(void *cbfunc, int argc, char **argv, char **colnames)
{
	rsdb_callback cb = (rsdb_callback)((uintptr_t)cbfunc);
	(cb) (argc, (const char **)(uintptr_t)argv);
	return 0;
}

void
rsdb_exec(rsdb_conn_t *dbconn, rsdb_callback cb, const char *format, ...)
{
	char buf[IRCD_BUFSIZE*4];
	va_list args;
	char *errmsg;
	unsigned int i;
	int j;
	char *retval;

	va_start(args, format);
	retval = sqlite3_vsnprintf(sizeof(buf), buf, format, args);
	va_end(args);

        if(strlen(retval) >= sizeof(buf)-1)
        {
                mlog(dbconn, "fatal error: length problem with compiling sql");
        }

	if((i = sqlite3_exec(dbconn->ptr, buf, (cb ? rsdb_callback_func : NULL), (void *)((uintptr_t)cb), &errmsg)))
	{
		switch (i)
		{
		case SQLITE_BUSY:
			for(j = 0; j < 5; j++)
			{
				rb_sleep(0, 500000);
				if(!sqlite3_exec
				   (dbconn->ptr, buf, (cb ? rsdb_callback_func : NULL), (void *)((uintptr_t)cb), &errmsg))
					return;
			}

			/* failed, fall through to default */
			mlog(dbconn, "fatal error: problem with db file: %s", errmsg);
			break;

		default:
			mlog(dbconn, "fatal error: problem with db file: %s", errmsg);
			break;
		}
	}
}

void
rsdb_exec_fetch(rsdb_conn_t *dbconn, struct rsdb_table *table, const char *format, ...)
{
	char buf[IRCD_BUFSIZE * 4];
	va_list args;
	char *errmsg;
	char **data;
	int pos;
	int retval;
	char *p;
	int i, j;

	va_start(args, format);
	p = sqlite3_vsnprintf(sizeof(buf), buf, format, args);
	va_end(args);

	if(strlen(p) >= sizeof(buf) - 1)
	{
		mlog(dbconn, "fatal error: length problem with compiling sql");
	}

	if((retval =
	    sqlite3_get_table(dbconn->ptr, buf, &data, &table->row_count, &table->col_count, &errmsg)))
	{
		int success = 0;

		switch (retval)
		{
		case SQLITE_BUSY:
			for(i = 0; i < 5; i++)
			{
				rb_sleep(0, 500000);
				if(!sqlite3_get_table
				   (dbconn->ptr, buf, &data, &table->row_count, &table->col_count,
				    &errmsg))
				{
					success++;
					break;
				}
			}

			if(success)
				break;

			mlog(dbconn, "fatal error: problem with db file: %s", errmsg);
			break;

		default:
			mlog(dbconn, "fatal error: problem with db file: %s", errmsg);
			break;
		}
	}

	/* we need to be able to free data afterward */
	table->arg = data;

	if(table->row_count == 0)
	{
		table->row = NULL;
		return;
	}

	/* sqlite puts the column names as the first row */
	pos = table->col_count;
	table->row = rb_malloc(sizeof(char **) * table->row_count);
	for(i = 0; i < table->row_count; i++)
	{
		table->row[i] = rb_malloc(sizeof(char *) * table->col_count);

		for(j = 0; j < table->col_count; j++)
		{
			table->row[i][j] = data[pos++];
		}
	}
}

void
rsdb_exec_fetch_end(rsdb_conn_t *dbconn, struct rsdb_table *table)
{
	int i;

	for(i = 0; i < table->row_count; i++)
	{
		rb_free(table->row[i]);
	}
	rb_free(table->row);

	sqlite3_free_table((char **)table->arg);
}

void
rsdb_transaction(rsdb_conn_t *dbconn, rsdb_transtype type)
{
	if(type == RSDB_TRANS_START)
		rsdb_exec(dbconn, NULL, "BEGIN TRANSACTION");
	else if(type == RSDB_TRANS_END)
		rsdb_exec(dbconn, NULL, "COMMIT TRANSACTION");
}
