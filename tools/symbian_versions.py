#! /usr/bin/env python
# -*- coding: utf-8 -*-

import sys
#import optparse
import re
import os
import random
import time
import alsaaudio

def transform(version):
	#4.07.1 => 00040701
	#0.99.0 => 00009900
	#0.99.2.35 => 00009902

	versions = version.split('.')[:3]
	result = "00"
	for ver in versions:
		if len(ver) > 2:
			ver = ver[:2]
		if len(ver) < 2:
			ver = "0"*(2-len(ver)) + ver
		result += ver
	return result

def transform_pkg(version):
	#4.07.1 => 4,7,1
	#0.99.0 => 0,99,0
	#0.99.2.35 => 0,99,2
	versions = [int(v) for v in version.split('.')[:3]]
	return ",".join( [str(v) for v in versions] )
	

def tests_pkg():
	examples = [ ("4.07.1" , "4,7,1"),  
			     ("0.99.0" , "0,99,0"),
			     ("0.99.2" , "0,99,2"),
			     ("0.99.2.3" , "0,99,2"),
			   ]
	for initial, out in examples:
		if transform_pkg(initial) != out:
			return False
	return True

def tests():
	examples = [ ("4.07.1" , "00040701"),  
			     ("0.99.0" , "00009900"),
			     ("0.99.2" , "00009902"),
			     ("0.99.2.3" , "00009902"),
			   ]
	for initial, out in examples:
		if transform(initial) != out:
			return False
	return True
    
if __name__ == "__main__":
	if ( len(sys.argv) < 2):
		print "usage: VERSION [PKG|"
		print "where PKG means generate version for .pkg file (if not setted, generates for elf2e32 by default)"
		sys.exit(1)
	version = str(sys.argv[1])
	
	if (len(sys.argv) == 3 and sys.argv[2] == "PKG"):
		print transform_pkg(version)
	else:
		print transform(version)
	

#	print	tests()
#	print	tests_pkg()



    



