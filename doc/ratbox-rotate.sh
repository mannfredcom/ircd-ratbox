#!/bin/sh
#
#  ratbox-rotate.sh - Rotate your IRCD's logs
#  Copyleft (C) 2016-2026 ircd-ratbox development team.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along
#  with this program; if not, write to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
###

# run: crontab -e
#
# To have the script run every month, type this:
#
#     @monthly test -x /path/to/ircd/bin/ratbox-rotate.sh && /path/to/ircd/bin/ratbox-rotate.sh >/dev/null 2>&1
#
#


ircd_logdir="%logdir%"

compress=0;

if [ "${ircd_logdir}" = "%logdir%" ]; then
  echo "Please configure $0" >&2
  exit 1
fi

if hash xz 2>/dev/null; then
  compress="xz"
elif hash lzma 2>/dev/null; then
  compress="lzma"
elif hash bzip2 2>/dev/null; then
  compress="bzip2"
elif hash gzip 2>/dev/null; then
  compress="gzip"
else
  echo "No file comperssor found in your path?" >&2
  exit 1
fi

for x in `dir -1 "${ircd_logdir}"`; do
  log="${x%%.log}";
  log="${log%%log}-`date +%F`.log";
  cp "${ircd_logdir}/${x}" "${ircd_logdir}/${log}" && echo -n > "${ircd_logdir}/${x}";
  nice ${compress} "${ircd_logdir}/${log}"
done
