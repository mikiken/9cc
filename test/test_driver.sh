#!/bin/bash
cat << EOT >> tmp.c
#include <stdlib.h>
#include <stdio.h>
EOT
cc -c tmp.c

./9cc test/test.c > test.s
cc -c test.s
cc -o tmp test.o tmp.o
./tmp
rm tmp test.o tmp.c tmp.o