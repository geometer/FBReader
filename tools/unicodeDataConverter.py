#!/usr/bin/python

import sys, csv

class unicode_doc(csv.excel):
	delimiter = ';'

csv.register_dialect("unicode_doc", unicode_doc)

print "<unicode>"
reader = csv.reader(sys.stdin, "unicode_doc")
i = 0
for data in reader:
	if data[2] == "Lu" or data[2] == "Ll":
		str =	"	<symbol code=\"0x" + data[0] + "\" type=\"" + data[2] + "\""
		if data[13] != "":
			str += " lower=\"0x" + data[13] + "\""
		if data[14] != "":
			str += " upper=\"0x" + data[14] + "\""
		str += "/>"
		print str
print "</unicode>"
