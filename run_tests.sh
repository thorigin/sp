#!/bin/bash
cd "$(dirname "$0")"
TESTS=./build/test/test*.bin
for i in $TESTS; do
    echo "Executing $i tests..."
    "./$i"
done
