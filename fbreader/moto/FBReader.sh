#!/bin/bash
this=`basename $0`
this=`echo $0 | sed -e 's/\/'$this'$//'`
cd $this/..
. /home/native/.profile
export HOME=`pwd`
export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH
exec bin/FBReader -lang `bin/language_detector` $2
