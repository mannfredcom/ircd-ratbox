
#include "librb-config.h"

#if !defined(HAVE_OPENSSL) && !defined(HAVE_GNUTLS) && !defined(HAVE_ARC4RANDOM)
extern void rb_arc4random_stir(void);
extern uint32_t rb_arc4random(void);
extern void rb_arc4random_addrandom(uint8_t *dat, int datlen);
#endif
