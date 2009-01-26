#!/bin/sh
echo 'Make sure you had run toplevel make first!'
find ../../../../fbreader/src/ -name *.o > o-files
ar -rsu o-files.a $(cat o-files)
cd library/FBReader
xcodebuild -configuration Debug && echo 'Execute "open library/FBReader/build/Debug/FBReader.app" to run FBReader'
cd ../..
