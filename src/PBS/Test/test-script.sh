#!/bin/sh

source seta.sh
source setb.sh

c=`expr $a "*" $b`

echo "c = $c" > result.txt
