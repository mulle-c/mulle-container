#! /bin/sh -x

. mulle-bootstrap-functions.sh

[ ! -z "$3" ] || internal_fail "dir parameter is empty"
[ -d "$3" ]   || fail  "directory \"$3\" is empty (`pwd -P`)"

( cd "$3" ; ./configure )

