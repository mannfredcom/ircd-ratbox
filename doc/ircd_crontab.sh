#!/bin/sh
#
# IRCD-RATBOX Crontab Script
# Copyright (C) 2009-2026 ircd-ratbox development team

# You will want to change this to the root directory your IRCD runs from
IRCD_PATH="/home/user/ircd-ratbox"

# The location of your pid file, relative to IRCD_PATH (you can probably leave this alone)
PID_FILE="etc/ircd-ratbox.pid"


# run: crontab -e
#
# To have the script run every minute, type this:
#
#      * * * * * /path/to/ircd_crontab.sh
#
# To have the script run every 5 minutes, type this:
#      */5 * * * * ~/path/to/ircd_crontab.sh
#


# Leave the rest alone.

test -x "${IRCD_PATH}/bin/ircd-ratbox" || exit 1

if [ -f "${IRCD_PATH}/${PID_FILE}" ] ; then
	PID=`cat "${IRCD_PATH}/${PID_FILE}"`
	if kill -0 $PID >/dev/null 2>&1 ; then
		exit 0;
	fi
	rm -f "${IRCD_PATH}/${PID_FILE}"
fi

${IRCD_PATH}/bin/ircd-ratbox -conftest &>/dev/null && ${IRCD_PATH}/bin/ircd-ratbox
exit $?;
