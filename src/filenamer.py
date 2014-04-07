import re
from os.path import exists, join, isdir

class Filenamer(object):
    def __init__(self, filename_format='%08d.dat'):
        self._format = filename_format
        self._directories_dict = {} # starts out empty
        self._next_number = -1 # self.next() will change this to zero

    def next_filename(self):
        self._next_number += 1
        if re.search('%[0-9]*d', self._format):
            while any([
                    exists(join(d, self._format % self._next_number))
                    for d in self.directories
                    ]):
                self._next_number += 1
            return self._format % self._next_number
        else:
            return self._format

    def add_directory(self, directory):
        self._directories_dict[directory] = None

    directories = property(lambda self: self._directories_dict.keys())
    format = property(lambda self: self._format)

    # make this an iterator
    def __iter__(self):
        return self

    def next(self):
        return self.next_filename()

if __name__ == '__main__':
    f = Filenamer()
    f.add_directory('.')
    f.add_directory('..')

    for i, filename in enumerate(f):
        if i == 9: break
        print filename

