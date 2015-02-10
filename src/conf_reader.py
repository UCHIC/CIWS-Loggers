from xml.dom import minidom

xmldoc = minidom.parse('conf.xml')
itemlist = xmldoc.getElementsByTagName('item') 


#print len(itemlist)

#print itemlist[0].attributes['name'].value
for s in itemlist :
    print s.attributes['name'].value, s.attributes['value'].value 

