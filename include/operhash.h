/* 
 *  ircd-ratbox: A slightly useful ircd
 *  operhash.h: irc operator hash  
 */

#ifndef INCLUDED_operhash_h
#define INCLUDED_operhash_h

void init_operhash(void);
const char *operhash_add(const char *name);
void operhash_delete(const char *name);
void operhash_count(size_t * number, size_t * mem);


#endif
