#!/bin/sh

set -e

LOGFILE=/tmp/version.git.pwd
log_message() {
    #echo "$@" >> $LOGFILE
    true
}

log_message # empty line
log_message `date`

cd "`dirname $0`"
log_message "pwd=`pwd` bd=`dirname $0` args='$@'x"

DEBIAN_VERSION=$(head -n 1 debian/changelog | sed -e 's/^[a-z_0-9-]\+ (\([0-9]\+\.[a-zA-Z0-9_.~]\+\)-[^-)]\+).*$/\1/')
log_message "debian='$DEBIAN_VERSION'"

echo "$DEBIAN_VERSION"
