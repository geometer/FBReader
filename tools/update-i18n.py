#! /usr/bin/python

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

import sys

from elementtree.ElementTree import Element, SubElement, ElementTree

SPACES = '\t'

def fix_it(elem):
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
        tempo = {}

        root = Element('resources')
        root.tail = '\n'

        tempo[()] = root
        
        for key in self._resources_order:
            for i in range(1, len(key)+1):
                if key[0:i] not in tempo:
                    parent = tempo[key[0:i-1]]

                    value = self._resources.get(key[0:i], None)

                    if value is None:
                        elem = SubElement(parent, 'node', name=key[i-1])
                    else:
                        localized = value.get(lang, None)
                        english = value.get('en', None)

                        if english is None:
                            print >> sys.stderr, 'English file does not have the string for', key[0:i]
                            print >> sys.stderr, '  entry is marked as obosolete.'

                            elem = SubElement(parent, 'node', name=key[i-1], value=localized, obsolete='true')
                        elif localized is not None:
                            elem = SubElement(parent, 'node', name=key[i-1], value=localized)
                        else:
                            elem = SubElement(parent, 'node', name=key[i-1], value=english, toBeTranslated='true')

                    parent.text = elem.tail = '\n' + i*SPACES
                    tempo[key[0:i]] = elem

        fix_it(root)

        print >> output, '<?xml version="1.0" encoding="UTF-8"?>'
        ElementTree(root).write(output, 'utf-8')

def main():
    if len(sys.argv) == 1:
        print 'Usage: update.py <lang>'
        sys.exit(1)

    coll = Collector()

    coll.process('en')
    coll.process(sys.argv[1])
    coll.dump(sys.stdout, sys.argv[1])

if __name__ == '__main__':
    main()

# vim:ts=4:sw=4:et
