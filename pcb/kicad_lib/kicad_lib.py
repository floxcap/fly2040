#!/usr/bin/env python3
import argparse
from io import TextIOWrapper
from zipfile import ZipFile

class libItem:
    def __init__(self):
        self.hasStart = False
        self.hasName = False
        self.hasEndHeader = False
        self.hasEnd = False
        self.data = []
        self.name = ""

    def append(self, line):
        l = line.strip()
        if not self.hasStart:
            # look for first empty '#' line
            if l == '#':
                #print("Start:" + l)
                self.hasStart = True
                self.data.append(l)
        elif self.hasStart and not self.hasEndHeader:
            if not self.hasName:
                if '#' in l and len(l) > 1:
                    #print("Name:" + l)
                    self.hasName = True
                    self.name = l[2:]
                    self.data.append(l)

            elif l == '#':
                #print("EndHeader:" + l)
                self.hasEndHeader = True
                self.data.append(l)
        elif self.hasStart and self.hasEndHeader and not self.hasEnd:
            if l == '#' or l == '':
                #print("End:" + l)
                self.hasEnd = True
            else:
                self.data.append(l)

    def __lt__(self, obj):
        return ((self.name.lower()) < (obj.name.lower()))

    def __gt__(self, obj):
        return ((self.name.lower()) > (obj.name.lower()))

    def __le__(self, obj):
        return ((self.name.lower()) <= (obj.name.lower()))

    def __ge__(self, obj):
        return ((self.name.lower()) >= (obj.name.lower()))

    def __eq__(self, obj):
        return (self.name.lower() == obj.name.lower())

    def __hash__(self):
        return hash(self.name)

    def done(self):
        return self.hasEnd

    def print(self):
        for l in self.data:
            print(l)

    def write(self, f):
        for l in self.data:
            f.write(l)
            f.write('\n')
        f.write('\n')


def main(source):
    # Process local lib file.
    lib_items = []
    item = libItem()
    with open('kicad_lceda.lib') as f:
        while True:
            line = f.readline()
            if not line:
                break
            item.append(line)
            if item.done():
                lib_items.append(item)
                item = libItem()

    # Process zip file.
    with ZipFile(source, 'r') as zip:
        for z in zip.filelist:
            if not z.is_dir():
                if 'kicad_lceda.lib' in z.filename:
                    item = libItem()
                    for l in TextIOWrapper(zip.open(z.filename)):
                        item.append(l)
                        if item.done():
                            print(item.name)
                            lib_items.append(item)
                            item = libItem()
                elif 'kicad_lceda.pretty' in z.filename:
                    zip.extract(z)
                elif 'kicad_lceda.3dshapes' in z.filename:
                    zip.extract(z)

    lib_items = list(dict.fromkeys(lib_items))
    lib_items.sort()
    with open('kicad_lceda.lib', "w") as f:
        f.write('EESchema-LIBRARY Version 2.4\n#encoding utf-8\n')
        for item in lib_items:
            item.write(f)
        f.write('#\n#End Library')


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--source', required=True, help='Input zip file.')
    args = parser.parse_args()
    main(args.source)
