##from xml.dom.minidom import parse
##
##file = open('bar.xml', 'r')
##dom = parse(file)
##x = dom.createElement("foo")  # creates <foo />
##txt = dom.createTextNode("hello, world!")  # creates "hello, world!"
##x.appendChild(txt)  # results in <foo>hello, world!</foo>
##dom.childNodes[1].appendChild(x)  # appends at end of 1st child's children
##print (dom.toxml())



from xml.dom.minidom import getDOMImplementation

impl = getDOMImplementation()

newdoc = impl.createDocument(None, "some_tag", None)
top_element = newdoc.documentElement
text = newdoc.createTextNode('Some textual content.')
top_element.appendChild(text)
file = open('bar.xml', 'w')
file.write(top_element.toxml())
file.flush()
file.close()
