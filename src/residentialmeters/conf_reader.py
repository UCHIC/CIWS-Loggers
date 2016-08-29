from xml.dom import minidom


try:
    xmldoc = minidom.parse('../../conf.xml')
    itemlist = xmldoc.getElementsByTagName('item') 
    Datalogger_name = itemlist[0].attributes['value'].value
    Site_name = itemlist[1].attributes['value'].value
    Port1_name = itemlist[2].attributes['value'].value
    Port2_name = itemlist[3].attributes['value'].value
    Port3_name = itemlist[4].attributes['value'].value
    Port4_name = itemlist[5].attributes['value'].value
    Site_description = itemlist[6].attributes['value'].value
    Time_support = itemlist[7].attributes['value'].value
    Data_directory = itemlist[8].attributes['value'].value
    
    #This section is used for debug
    #print Port3_name
except:

    print "Error or no configuration file found. Please create one following the default format and try again."



#Other temp settings

Conv_factor1 = 0.01
Conv_factor2 = 0.01
Conv_factor3 = 0.01
Conv_factor4 = 0.01

#print 'Configuration done.'
