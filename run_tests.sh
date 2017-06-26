#!/bin/bash
DIR=$(basedir $@)
echo "DIR is $DIR"
TESTS=./build/test/test*.bin
for i in $TESTS; do
    echo "Executing $i tests..."
    "./$i"
done
