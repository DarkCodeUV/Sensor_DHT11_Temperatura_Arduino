import paho.mqtt.client as paho
import sys

#broker="192.168.0.19"
broker="35.239.161.254"
port=1883
def on_publish(client,userdata,result):             #create function for callback
    print("data published \n")
    pass

client1= paho.Client("Temperature-Sensor")                           #create client object
client1.on_publish = on_publish                          #assign function to callback
client1.connect(broker,port)
ret= client1.publish(sys.argv[1],sys.argv[2]) 
print(ret)
