/*  */
#ifndef INCLUDED_rsdb_h
#define INCLUDED_rsdb_h

/* error handler callback */
typedef void rsdb_error_cb(const char *, void *data);

typedef int (*rsdb_callback) (int, const char **);

typedef enum rsdb_transtype
{
	RSDB_TRANS_START,
	RSDB_TRANS_END
}
rsdb_transtype;

struct rsdb_table
{
	char ***row;
	int row_count;
	int col_count;
	void *arg;
};

typedef struct _rsdb_conn
{
	struct sqlite3 *ptr;
	rsdb_error_cb *error_cb;
	void *error_cb_data;

} rsdb_conn_t;

rsdb_conn_t *rsdb_init(const char *path, rsdb_error_cb *, void *data);
void rsdb_shutdown(rsdb_conn_t *);


void rsdb_exec(rsdb_conn_t *, rsdb_callback cb, const char *format, ...);
void rsdb_exec_fetch(rsdb_conn_t *, struct rsdb_table *data, const char *format, ...);
void rsdb_exec_fetch_end(rsdb_conn_t *,struct rsdb_table *data);

void rsdb_transaction(rsdb_conn_t *, rsdb_transtype type);


#endif
