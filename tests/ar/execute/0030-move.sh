#!/bin/sh



set -e

trap "rm -f file* $tmp1 $tmp2" 0 2 3

tmp1=`mktemp`
tmp2=`mktemp`

############################################################################
#move 1st at the end

cp master.a file.a
ar -mv -a file3 file.a file1
ar -t file.a > $tmp1

cat <<EOF > $tmp2
file2
file3
file1
EOF

cmp $tmp1 $tmp2
