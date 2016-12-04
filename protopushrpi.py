import requests
import json

serverconfig = json.load(open('serverconfig.json'))
url = "http://" + serverconfig["server_public_ip"] + ":" + serverconfig["server_port"] + "/rpiminute"


def serverpush(serverpusharray):
    t = [x.split("::") for x in serverpusharray]     
    
    datavalues = {
                "timeseries_id": "1",
                "datavalues": [{
                    "datavalue_value": t[0][1],
                    "datavalue_datetime_local": t[0][0]
                }, {
                    "datavalue_value": t[1][1],
                    "datavalue_datetime_local": t[1][0]
                }]
    }
    
    
    headers = {'Content-type': 'application/json'}
    r = requests.post(url, data=json.dumps(datavalues), headers=headers)
