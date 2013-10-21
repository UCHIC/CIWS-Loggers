###This class manage writing/reading csv files 
##
##import csv, codecs, cStringIO
##
###with open('eggs.csv', 'wb') as csvfile:
##
##    
##
##
##
##class csvMANAGER:
##    def __init__():
##
##
##        pass
##
##    def __del__():
##
##        pass
##
##
##
##
##
##
##class UnicodeWriter:
##    """
##    A CSV writer which will write rows to CSV file "f",
##    which is encoded in the given encoding.
##    """
##
##    def __init__(self, f, dialect=csv.excel, encoding="utf-8", **kwds):
##        # Redirect output to a queue
##        self.queue = cStringIO.StringIO()
##        self.writer = csv.writer(self.queue, dialect=dialect, **kwds)
##        self.stream = f
##        self.encoder = codecs.getincrementalencoder(encoding)()
##
##    def writerow(self, row):
##        self.writer.writerow([s.encode("utf-8") for s in row])
##        # Fetch UTF-8 output from the queue ...
##        data = self.queue.getvalue()
##        data = data.decode("utf-8")
##        # ... and reencode it into the target encoding
##        data = self.encoder.encode(data)
##        # write to the target stream
##        self.stream.write(data)
##        # empty queue
##        self.queue.truncate(0)
##
##    def writerows(self, rows):
##        for row in rows:
##            self.writerow(row)
##
##
##def main():
##    test = UnicodeWriter('test.csv')
##    test.writerow('123')
##    pass
##
##main()
##


import csv

ifile  = open('test.csv', "r")
reader = csv.reader(ifile)

rownum = 0
for row in reader:
    # Save header row.
    if rownum == 0:
        header = row
    else:
        colnum = 0
        for col in row:
            print ('%-8s: %s' % (header[colnum], col))
            colnum += 1
            
    rownum += 1
    pass

ifile.close()
