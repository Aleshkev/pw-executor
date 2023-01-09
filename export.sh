#!/usr/bin/env fish

set album ja438241

rm $album.zip -f
zip $album.zip *.h *.cpp *.txt

# sprawdzamy czy działa

rm $album -rf
unzip -u $album.zip -d $album
cd $album && mkdir build && cd build && cmake .. && make
echo "run echo Sanity check successful
sleep 300
out 0" | ./executor
