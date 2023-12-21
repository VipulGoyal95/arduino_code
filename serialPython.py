#!/usr/bin/env python3
import serial
#import sys
#sys.path.append('/path/to/pyrebase')

#import pyrebase

import json

import secrets
import string

import threading
from flask import Flask, render_template

import csv
from datetime import datetime
import os

from datetime import datetime

from cryptography.fernet import Fernet
#from cryptography.fernet import Fernet
#from Crypto.Random import get_random_bytes
import base64


from flask_socketio import SocketIO
app = Flask(__name__)

socketio = SocketIO(app)

import paho.mqtt.client as paho
from paho import mqtt

from pymongo import MongoClient
import time

mongo_uri = "mongodb+srv://navneetraj3695:GLB1NP2bWr7S312I@cluster0.xrk0vw6.mongodb.net/"
mongo_db_name = "hivemqData"
mongo_collection_name = "hiveMQCollection"

hiveMQUser="navneet-09"
hiveMQPassword="Navneet@09"

mongo_client = MongoClient(mongo_uri)
mongo_db = mongo_client[mongo_db_name]
mongo_collection = mongo_db[mongo_collection_name]


@app.route('/')
def index():
    #image_url = "static/flask_image.png"  # Update with the correct image path
    return render_template('index.html')

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser2= serial.Serial('/dev/ttyACM1', 9600, timeout=1)
    ser.reset_input_buffer()
    ser2.reset_input_buffer()
    
    firebaseConfig = {
        "apiKey": "AIzaSyATPZ_mYSJYPdqFqiVPiEylDDSeZzi_OfA",
        "authDomain": "gprs-component.firebaseapp.com",
        "databaseURL": "https://gprs-component-default-rtdb.firebaseio.com",
        "projectId": "gprs-component",
        "storageBucket": "gprs-component.appspot.com",
        "messagingSenderId": "31102542",
        "appId": "1:31102542:web:5f3ccc83ec70a2ff46151f",
      };
    #firebase=pyrebase.initialize_app(firebaseConfig)
    #db=firebase.database()
    
    def generate_key():
        return Fernet.generate_key()

    def encrypt_with_fernet(data, key):
        cipher_suite = Fernet(key)
        encrypted_data = cipher_suite.encrypt(data.encode())
        return encrypted_data

    def encrypt_with_aes(data, key):
        cipher = AES.new(key, AES.MODE_CBC)
        ciphertext = cipher.encrypt(pad(data.encode(), AES.block_size))
        return base64.b64encode(cipher.iv + ciphertext)
    
    def process_and_append_to_csv(input_string, folder_name, filename):
    # Create the folder if it doesn't exist
        folder_path = os.path.join(os.getcwd(), folder_name)
        os.makedirs(folder_path, exist_ok=True)
        timestamp = int(time.time())
        pairs = input_string.split(',')
        columns = ['timestamp']
        values = [timestamp]

        for pair in pairs:
            parts = pair.split(':')
            if len(parts) != 2:
                continue
            key, value = parts
            column_name = key.strip()
        # Append column names and values to their respective lists
            column_name = key.strip()
            if column_name == "eed":
                column_name = "speed"
            if column_name not in columns:
                columns.append(column_name)
                values.append(value)
            else:
        # If the column already exists, find its index and update the corresponding value
                index = columns.index(column_name)
                values[index] = value

    # Create the full file path
        file_path = os.path.join(folder_path, filename)
        file_exists = os.path.exists(file_path)
    # Check if the file exists
        try:
        # Read existing data from CSV file
            with open(file_path, 'r') as csv_file:
                csv_reader = csv.reader(csv_file)
                existing_data = list(csv_reader)
        except FileNotFoundError:
        # If the file doesn't exist, create a new list for data
            existing_data = []

    # Add the header row if the file is empty or has different columns
        if not existing_data and not file_exists:
            existing_data = [columns] + existing_data

    # Add the values to the data list
        if values:
            existing_data.append(values)

    # Write the updated data to the CSV file
        with open(file_path, 'w', newline='') as csv_file:
            csv_writer = csv.writer(csv_file)
            csv_writer.writerows(existing_data)

    
    
    def store_in_mongodb(data):
        #print(data)
        current_timestamp = int(time.time())
        key = Fernet.generate_key()
        
        latestDataToJump={"data":data,"time":current_timestamp}
        data_string = json.dumps(data)
        encrypted_data = Fernet(key).encrypt(data_string.encode('utf-8'))
        mongo_collection.insert_one({"data":encrypted_data,"user_id":hiveMQUser})
        print("Data stored in MongoDB:", encrypted_data)
    
    
    def encryption_before_sending_Data(data, userPassword):
        message = data
        key = Fernet.generate_key()
        message_bytes = message.encode('utf-8')
        fernet = Fernet(key)
        encrypted_message = fernet.encrypt(message_bytes)
        return encrypted_message;
    
    def check_internet_connection():
        try:
            response = requests.get('http://www.google.com', timeout=5)
            return True
        except requests.ConnectionError:
            return False
        
    def delete_data_from_csv(file_path):
        with open(file_path, 'w', newline='') as csv_file:
            csv_writer = csv.writer(csv_file)
            csv_writer.writerows([])
            
        def upload_to_server(data):
        # Replace 'YOUR_SERVER_ENDPOINT' with the actual server endpoint
            try:
                #current_timestamp = int(time.time())
                #key = Fernet.generate_key()
        
        #latestDataToJump={"data":data,"time":current_timestamp}
        #data_string = json.dumps(data)
        #encrypted_data = Fernet(key).encrypt(data_string.encode('utf-8'))
                mongo_collection.insert_many(data)
                print("Data stored in MongoDB:", data)
               
            except requests.RequestException as e:
                print(f"Error uploading data to server: {e}")
                return False
            
    def read_data_from_csv(file_path):
        try:
            with open(file_path, 'r') as csv_file:
                csv_reader = csv.reader(csv_file)
                data = list(csv_reader)
            return data
        except FileNotFoundError:
            return []

    def CSV_to_Server(file_path):
        data = read_data_from_csv(file_path)
        if data:
            print(data);
            if check_internet_connection():
                upload_success = upload_to_server(data)
                if upload_success:
                    
                    # If upload is successful, delete data from the CSV file
                    delete_data_from_csv(file_path)
                    
            time.sleep(300)
        
# Example usage
#data = "Spd:0Speed:0.00,lat:12.279872,long:76.640663,"
#result = parse_data(data)

    def parse_data(data):
    # Split the data into key-value pairs
        encoder="0"
        payload="0"
        fuel_level="0"
        voltage="0"
        temperature="0"
        speed="0"
        lat="0"
        long="0"
        pairs = data.split(',')

    # Initialize variables
        #print(data)
   
        print(pairs)
    # Loop through each key-value pair and extract values
        for pair in pairs:
            if len(pair) == 2:
                key, value = pair.split(':')
                if key == 'encoder':
                    encoder = value
                elif key == 'payload':
                    payload = value
                elif key == 'FuelLevel':
                    fuel_level = value
                elif key == 'Voltage':
                    voltage = value
                elif key == 'temperature':
                    temperature = value
                elif key == 'Speed':
                    speed = value
                elif key == 'lat':
                    lat = value
                elif key == 'long':
                    long = value
        #print(voltage,temperature)
                dataPOSITION={"LAT":lat,"LONG":long}
                #db.update(dataPOSITION)
        socketio.emit('update_data', {'data': data})
        #print(pairs)
        store_in_mongodb(pairs)

    # Return a dictionary with the extra
    
    #Make CSV FOR OFFLINE QUERY


    def on_connect(client, userdata, flags, rc, properties=None):
        print("CONNACK received with code %s." % rc)

# with this callback you can see if your publish was successful
    def on_publish(client, userdata, mid, properties=None):
        print("mid: " + str(mid))
#encoder:0,payload:2,FuelLevel:1.00,Voltage:0.00,temperature:24.23,Speed:0.00,lat:12.279953,long:76.640838,
# print which topic was subscribed to
    def on_subscribe(client, userdata, mid, granted_qos, properties=None):
        print("Subscribed: " + str(mid) + " " + str(granted_qos))

# print message, useful for checking if it was successful
    def on_message(client, userdata, msg):
        print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
        
    client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
    client.on_connect = on_connect
                    
# enable TLS for secure co        lnnection
    client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
# set username and password
    client.username_pw_set(hiveMQUser, hiveMQPassword)
# connect to HiveMQ Cloud on port 8883 (default for MQTT)
    client.connect("6dda378c8ff04fdba7b2dfe78949616b.s2.eu.hivemq.cloud", 8883)

# setting callbacks, use separate functions like above for better visibility
    client.on_subscribe = on_subscribe
    client.on_message = on_message
    client.on_publish = on_publish
    #app.run(debug=True)
    def mqtt_thread():
        while True:
            if ser2.in_waiting > 0 and ser.in_waiting>0:
                line = ser.readline().decode('utf-8', errors="replace").strip()
                line2= ser2.readline().decode('utf-8', errors="replace").strip()
                newline=line+line2
                
                internet_connected = os.system("ping -c 1 google.com") == 0
                #if True:
                    #write_to_csv(newline)
                print(newline)
                encryptedData=encryption_before_sending_Data(newline, hiveMQPassword)
                print(encryptedData)
                #CSV_to_Server(os.path(os.getcwd(),"OFFLINECSV","latest.csv"));
                process_and_append_to_csv(newline,"OFFLINECSV","latest.csv")
                parse_data(newline)
                
                
                client.subscribe("dashboard/#", qos=0)

    # a single publish, this can also be done in loops, etc.
                client.publish("dashboard/vehicle", payload=encryptedData, qos=0)
    flask_thread = threading.Thread(target=socketio.run, kwargs={'debug': True})
    flask_thread.start()
    mqtt_thread = threading.Thread(target=mqtt_thread)
    mqtt_thread.start()
    

