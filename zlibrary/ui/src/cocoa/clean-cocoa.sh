#!/bin/sh
rm -f o-files
rm -f o-files.a
cd library/FBReader
xcodebuild -alltargets clean
cd ../..
