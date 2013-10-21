import csv

print ('Testing')
with open('eggs.csv', 'w', newline='') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=',',
                            quotechar='"', quoting=csv.QUOTE_MINIMAL)
    spamwriter.writerow(['Spam'] * 5 + ['Baked Beans'])
    spamwriter.writerow(['"Spam"', '"Lovely Spam"', '"Wonderful Spam"'])

class watertrace():
    def __init__():


        

        pass

    def __del__():

        pass
