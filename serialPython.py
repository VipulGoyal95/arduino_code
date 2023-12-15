#!/usr/bin/env python3
import serial
import paho.mqtt.client as paho
from paho import mqtt

if __name__ == '__main__':

    
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser2= serial.Serial('/dev/ttyACM1', 9600, timeout=1)
    ser.reset_input_buffer()
    ser2.reset_input_buffer()

    def on_connect(client, userdata, flags, rc, properties=None):
        print("CONNACK received with code %s." % rc)

# with this callback you can see if your publish was successful
    def on_publish(client, userdata, mid, properties=None):
        print("mid: " + str(mid))

# print which topic was subscribed to
    def on_subscribe(client, userdata, mid, granted_qos, properties=None):
        print("Subscribed: " + str(mid) + " " + str(granted_qos))

# print message, useful for checking if it was successful
    def on_message(client, userdata, msg):
        print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
        
    client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
    client.on_connect = on_connect

# enable TLS for secure connection
    client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
# set username and password
    client.username_pw_set("navneet-09", "Navneet@09")
# connect to HiveMQ Cloud on port 8883 (default for MQTT)
    client.connect("6dda378c8ff04fdba7b2dfe78949616b.s2.eu.hivemq.cloud", 8883)

# setting callbacks, use separate functions like above for better visibility
    client.on_subscribe = on_subscribe
    client.on_message = on_message
    client.on_publish = on_publish

    while True:
        if ser2.in_waiting > 0 and ser.in_waiting>0:
            
            line = ser.readline().decode('utf-8', errors="replace").strip()
            line2= ser2.readline().decode('utf-8', errors="replace").strip()
            #line="hello boys"
            print(line)
            newline=line+line2
            client.subscribe("dashboard/#", qos=0)

# a single publish, this can also be done in loops, etc.
            client.publish("dashboard/vehicle", payload=newline, qos=0)
            

