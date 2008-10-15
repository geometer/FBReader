#! /usr/bin/python

'''Simple script to extract localizeable strings from FBReader resource files'''

__copyright__ = '''\
Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
Street, Fifth Floor, Boston, MA 02110-1301, USA.
'''

import sys, os, csv

from elementtree.ElementTree import ElementTree

SPACES = '\t'

def fix_it(elem):
    '''fix last text string length for better formatting'''

    if len(elem):
        for child in elem:
            fix_it(child)

        last = elem.getchildren()[-1]

        last.tail = last.tail[:-len(SPACES)]

class Collector:
    def __init__(self):
        self._resources = {}
        self._resources_order = []

    def process(self, lang):
        assert len(lang) == 2, 'Language name must be two letters long'

        doc = ElementTree(file='%s.xml' % lang)

        root = doc.getroot()

        if root.tag == 'resources':
            for child in root:
                self.walk(child, (child.get('name'),), lang)

    def walk(self, element, prefix, lang):
        value = element.get('value', None)

        if value is not None and element.get('toBeTranslated', None) is None:
            if prefix not in self._resources:
                tempo = {}
                self._resources_order.append(prefix)
            else:
                tempo = self._resources[prefix]

            tempo[lang] = value
            self._resources[prefix] = tempo

        for child in element:
            self.walk(child, prefix + (child.get('name'), ), lang)

    def dump(self, output, lang):

        # from pprint import pprint
        # pprint(self._resources)

        tempo = []

        for key in sorted(self._resources.keys()):
            value = self._resources[key].get(lang, None)
            if value is not None:
                tempo.append(('/'.join(key), value.encode('utf-8')))

        writer = csv.writer(output)
        writer.writerows(tempo)

def main():
    '''main worker'''

    if len(sys.argv) == 1:
        print >> sys.stderr, 'Usage: extract_i18n.py <lang> > <file.csv>'
        print >> sys.stderr, ' (to be run from the directory where en.xml is located)'
        sys.exit(1)

    coll = Collector()

    coll.process('en')
    # coll.process(sys.argv[1])
    coll.dump(sys.stdout, sys.argv[1])

if __name__ == '__main__':
    main()

# vim:ts=4:sw=4:et
