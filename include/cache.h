/*  */
#ifndef INCLUDED_CACHE_H
#define INCLUDED_CACHE_H

#define CACHELINELEN	81
#define CACHEFILELEN	30
/* two servernames, a gecos, three spaces, ":1", '\0' */
#define LINKSLINELEN	(HOSTLEN + HOSTLEN + REALLEN + 6)

#define HELP_USER	0x001
#define HELP_OPER	0x002

struct cachefile
{
	rb_dlink_list contents;
	char name[CACHEFILELEN];
	int flags;
};

struct cacheline
{
	rb_dlink_node linenode;
	char data[CACHELINELEN];
};

void init_cache(void);
struct cachefile *cache_file(const char *, const char *, int);
void cache_links(void *unused);

void send_links_cache(struct Client *source_p);


void free_cachefile(struct cachefile *);
const char *cache_user_motd_updated(void);


void load_help(void);
void clear_help(void);
void send_user_motd(struct Client *);
void cache_user_motd(void);

void send_oper_motd(struct Client *);
void cache_oper_motd(void);

#endif
