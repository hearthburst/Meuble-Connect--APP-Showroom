#install mqtt packages
#pip install paho.mqtt

from email import message
from operator import indexOf
import paho.mqtt.client as mqtt
import json

# Successful Connection Callback
def on_connect(client, userdata, flags, rc):
    print('Connected with result code '+str(rc))
    client.subscribe('/showroom/canap')

# add new values to a list, delete old ones if too many values
def append_limit(value, list):
    nb_valeur=15
    if(len(list)>=nb_valeur):
        del list[0]
    list.append(value)

# moyennes des capteurs en paramètres 
# Capteurs : PIR1, PIR2, PIR3, PIR4, PIR5, PIR6, PIR7, PIR8, PIR9
def moys(list, capteurs = []):
    sums = {}
    for capteur in list:
        (capt_test, v) = capteur[0]
        if capt_test not in capteurs:
            continue
        sum = 0
        name = ''
        for k in capteur:
            (capt, val) = k
            name = capt
            sum+=val
        sums[name] = sum/len(capteur)
    return sums

capteurs_datas = []
# Message delivery callback
def on_message(client, userdata, msg):
    
    message_json = json.loads(msg.payload.decode("utf-8"))

 

        
    print(msg.topic+" "+str(msg.payload))
    a=message_json[0].values()
    print(a)
    

client = mqtt.Client()

# Set callback handler
client.on_connect = on_connect
client.on_message = on_message

# Set up connection
client.connect('10.100.100.117', 443, 60)
# Publish message
client.publish('/showroom/canap',payload='SUR',qos=0)

client.loop_forever()
