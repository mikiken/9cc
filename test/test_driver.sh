#!/bin/bash
cd `dirname $0`
for file in *.c
do
  ../9cc ${file%.*}.c > ${file%.*}.s
  cc -c ${file%.*}.s
done
cc -o tmp *.o
./tmp
rm tmp *.o