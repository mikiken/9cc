#!/bin/bash
cat << EOT >> tmp.c
#include <stdlib.h>
#include <stdio.h>
EOT
cc -c tmp.c

for file in test/*.c
do
  ./9cc ${file%.*}.c > ${file%.*}.s
  cc -c ${file%.*}.s
done
cc -o tmp *.o
./tmp
rm tmp *.o