#!/bin/sh

usage() {
  echo "usage: install.sh [flags]"
  echo "flags:"
  echo "  -help               : this usage message"
  echo "  -apache <apachedir> : changes the apache dir"
  echo "  -apxs <apxspath>    : path to apxs"
}

DEBUG=
while [ "$#" -ne 0 ]; do
    case "$1" in

    # Documented arguments
    -h | -help)       SHOW_HELP=true; shift;;
    -apache) APACHE="$2"; shift 2;;
    -apxs) APXS="$2"; shift 2;;
    -debug) DEBUG="-Wc,-DDEBUG"; shift;;

    *)   break;
    esac
done

if test -z "${APACHE}"; then
  APACHE=/usr/local/apache
fi

echo "Using Apache installation in ${APACHE}"

if test ! -z "${APXS}"; then
  APACHE=$APXS -q PREFIX
elif ${APACHE}/sbin/apxs -q PREFIX >/dev/null 2>/dev/null; then
  APXS=${APACHE}/sbin/apxs
elif ${APACHE}/bin/apxs -q PREFIX >/dev/null 2>/dev/null; then
  APXS=${APACHE}/bin/apxs
elif ${APACHE}/src/support/apxs -q PREFIX >/dev/null 2>/dev/null; then
  APXS=${APACHE}/src/support/apxs
elif apxs -q PREFIX >/dev/null 2>/dev/null; then
  APXS=apxs
else
  echo "Cannot find valid Apache apxs in ${APACHE}"
  exit 1
fi

echo "Using apxs from ${APXS}"

$APXS -o mod_caucho.so -i -Wc,-Wall $DEBUG -Wc,-O2 -c mod_caucho.c registry.c config.c stream.c

if test $? != 0; then
  echo "Compilation failed, probably due to problems in"
  echo "$APXS.  See the Resin troubleshooting FAQ for information"
  echo "on fixing it."
  exit 1
fi
CONFDIR=`$APXS -q SYSCONFDIR`

if ! grep mod_caucho $CONFDIR/httpd.conf >/dev/null 2>/dev/null; then
  path=`type -p $0`
  dir=`dirname $path`

  if test -d ${APACHE}/htdocs; then
    DOCROOT=${APACHE}/htdocs
  elif test -d ${APACHE}/share/htdocs; then
    DOCROOT=${APACHE}/share/htdocs
  else
    DOCROOT=doc
  fi

#  sed "s#DOCROOT#$DOCROOT#" < $dir/srun.conf > $dir/../srun.conf

  cat >>$CONFDIR/httpd.conf <<EOF
#
# Caucho Servlet Engine Configuration
#
# Uncomment the following two lines to enable CSE

#LoadModule caucho_module libexec/mod_caucho.so
#AddModule mod_caucho.c
<IfModule mod_caucho.c>
  CauchoConfigFile $dir/../resin.conf

# Uncomment the following line to debug the connection setup
#  <Location /caucho-status>
#    SetHandler caucho-status
#  </Location>
#
# Uncomment the following for srun connection keepalive
# CauchoKeepalive on
#
# You can also configure CSE directly by commenting out CauchoConfigFile
# and adding the following:
#
# CauchoHost localhost
# CauchoPort 6802
# AddHandler caucho-request jsp
# <Location /servlets/*>
#    SetHandler caucho-request
# </Location>
</IfModule>
EOF
fi

