
# **IRCD-RATBOX** #
![logo](http://www.ratbox.org/logo.jpg)

## Important ##
Notes for those among you, who don't bother reading docs:
 * Your install is likely to fail unless you read this document. Completely.
 * Reading INSTALL is now a must, as the old DPATH is now specified
   when configure is run.
 * You now need to `./configure --prefix="/path/to/install/ircd"`
 * The old config format **WILL NOT WORK**.  Please see [example.conf](file://doc/example.conf) !
 * The old kline format **WILL NOT WORK**.  Please use bantool which
   will be installed along-side your ircd!
 * Run bantool after each upgrade to ensure your database is in a current format.
   Failure to do so will result in weird, unexplained crashes.
 * It is _highly_ recommended that you `make clean` or even better `make distclean`
   in your current source tree before running `./configure`


## ![markdown logo](https://github.com/adam-p/markdown-here/raw/master/src/common/images/icon48.png) About this document ##

This document is written in **MARKDOWN**. You may, as you likely are now, read
it as-is, or you can view it formatted in a viewer such as the ATOM IDE,
Google Chrome with the Markdown Plus extension, or in ReText.
There are other viewers, such as for the console, but this will be left as an
exercise for the reader.

## Features & Requirements ##
### A short introduction ###
ircd-ratbox-3.x now has several major changes over previous version that you
will notice right away.

  - Built-in RBL checks for connecting clients
  - Storage of bans in a database, versus the old flat-files.
  - SSL Client support.
  - SSL Only Channel support.
  - Adminwall (think Operwall, but for admins only).
  - Force Nick Change (FNC).
  - Support for global CIDR limits.
  - Connection Throttling.
  - Please see [whats-new-3.1.txt](file://doc/whats-new-3.1.txt) for more detailed changes.

### Necessary Requirements ###
  - A supported platform (look below)
  - A working dynamic load library, unless
    compiling as static, without module
    support.
  - A working lex.  Solaris /usr/ccs/bin/lex
    appears to be broken, on this system flex
    should be used.

### Feature Specific Requirements ###
  - For SSL Clients, SSL Challenge controlled OPER feature, and encrypted server links,
    a working SSL library. Though OpenSSL is still supported, LibreSSL is recommended.
  - For encrypted oper and (optional) server passwords, a working DES, MD5, or SHA library.


## Supported Distributions
Known to build and run cleanly on:
- Ubuntu 22.04 LTS, 24.04 LTS
- AlmaLinux 8, AlmaLinux 9
- FreeBSD 13.x, 14.x, 15.x

Other POSIX-y systems with autotools, OpenSSL or LibreSSL, and a
recent C99 compiler should also build; reports welcome.

## For More Information....
- To report bugs in ircd-ratbox, send the bug report to ircd-ratbox@lists.ratbox.org

- Known bugs are listed in the BUGS file

- See the INSTALL document for info on configuring and compiling
  ircd-ratbox.

- Please read doc/index.txt to get an overview of the current documentation.

- Old Hybrid 5/6 configuration files are no longer supported.  Config files from
  previous ircd-ratbox versions will need some changes.  The ircd -conftest option
  is your friend here. Old kline/xline/dline.conf files will have to be converted to
  the new database format.  A config import utility is provided and installed
  as bin/bantool.

- If you are wondering why config.h is practically empty, its because many
  things that were once in config.h are now specified in the 'general'
  block of ircd.conf.  Look at example.conf for more information about
  these options.

- The files, /etc/services, /etc/protocols, and /etc/resolv.conf, MUST be
  readable by the user running the server in order for ircd to start.
  Errors from adns causing the ircd to refuse to start up are often related
  to permission problems on these files.

- Please read doc/whats-new-3.1.txt for information about what is in this release

- Other files recommended for reading: BUGS, INSTALL

--------------------------------------------------------------------------------
