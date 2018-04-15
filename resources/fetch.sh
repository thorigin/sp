
#!/bin/bash

cd "$(dirname "$0")"
echo -n "Checking for resources..."

WGETOPTIONS=" -N --quiet"

FILES="train-images-idx3-ubyte.gz train-labels-idx1-ubyte.gz t10k-images-idx3-ubyte.gz t10k-labels-idx1-ubyte.gz"

mkdir -p mnist

DISPFETCH=0

cd mnist
for d in $FILES; do
    if [[ ! -e $d ]]; then
      if [[ $DISPFETCH -eq 0 ]]; then
        echo -n " Fetching..."
        DISPFETCH=1
      fi
      wget "http://yann.lecun.com/exdb/mnist/$d" $WGETOPTIONS
    fi
done

for f in *.gz; do
 base=$(basename "${f}" .gz)
 if [[ ! -e $base ]]; then
    gunzip -k $f
 fi
done

echo " Done!"
