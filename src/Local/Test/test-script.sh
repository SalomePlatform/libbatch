#!/bin/sh

source copied-seta.sh
source copied-setb.sh

c=`expr $a "*" $b`

echo "c = $c" > orig-result.txt
