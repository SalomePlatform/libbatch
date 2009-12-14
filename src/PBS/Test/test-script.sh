#!/bin/sh

source seta.sh
source setb.sh

c=`expr $a "*" $b`

echo "MYENVVAR = $MYENVVAR" > result.txt
echo "c = $c" >> result.txt
