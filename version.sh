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

GIT=git

if $GIT rev-parse HEAD >/dev/null 2>&1; then
    $GIT update-index --refresh > /dev/null || true
    GIT_REF="`$GIT rev-parse HEAD`"
    log_message "$GIT_REF"
    if [ "`$GIT diff-files`x`$GIT diff-index --cached HEAD`" = "x" ]; then
        GIT_VERSION="$GIT_REF/clean"
    else
        GIT_VERSION="$GIT_REF/dirty"
    fi
else
    log_message="unknown git"
    GIT_VERSION="unknown-git-version"
fi
log_message="git='$GIT_VERSION'"

echo "$DEBIAN_VERSION (git: $GIT_VERSION)"
