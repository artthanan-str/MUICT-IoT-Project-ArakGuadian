import serial
import requests
import RPi.GPIO as GPIO
import LiquidCrystalPi
import time as time
import json
import time
import microgear.client as microgear
import logging

# Variable Declaration
arduino_serial = serial.Serial('/dev/ttyACM0',9600)
# url = "http://itcs446-raspiproject.appspot.com/GetRPIData"
url = "<your-url-server>"

string_to_be_replaced = ["b", "'", "\\", "n", "r"]

humid_value = 0
temp_value = 0
flame_value = 0
isFired = False

# Set up LCD Display
GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)
 
LCD = LiquidCrystalPi.LCD(29, 31, 33, 35, 37, 38)
LCD.begin(16,2)

# Setup Microgear
appid = "ArakProject"
gearkey = "khPqOc3S0pQCbs7"
gearsecret =  "XCuPjEwK433zzN8C7JcmIVDGN"

microgear.create(gearkey,gearsecret,appid,{'debugmode': True})

def connection():
	print("Now I am connected with netpie")

def subscription(topic,message):
	print(topic+" "+message)

microgear.setalias("ArakBot-Netpie")
microgear.on_connect = connection
microgear.on_message = subscription
microgear.subscribe("/mails")

microgear.connect()

while True:
	read_serial = arduino_serial.readline()
	arduino_data = str(read_serial)

	for each_char in string_to_be_replaced:
		arduino_data = arduino_data.replace(each_char, '')

	arduino_data = arduino_data.split()
	print(arduino_data)
	humid_value = arduino_data[0]
	humid_value = humid_value[:-1]
	humid_value = humid_value[:-1]
	humid_value = humid_value[:-1]
	temp_value = arduino_data[1]
	temp_value = temp_value[:-1]
	temp_value = temp_value[:-1]
	temp_value = temp_value[:-1]
	flame_value = arduino_data[2]
	if arduino_data[3] == "1":
		isFired = True
	else:
		isFired = False
	
	if (isFired):
		string_line_1 = "Fired!!!"
		string_line_2 = "Call 199!!"
		data = {'type': 'rpi_data','humidity': humid_value,'temperature': temp_value,'flame': flame_value, 'isFired': 'Fired'}
	else:
		string_line_1 = "T: " + temp_value + "C, H: " + humid_value + "%" 
		string_line_2 = "FlameLV: " + flame_value
		data = {'type': 'rpi_data','humidity': humid_value,'temperature': temp_value,'flame': flame_value, 'isFired': 'no'}

	LCD.clear()
	LCD.home()
	LCD.write(string_line_1)
	LCD.nextline() 
	LCD.write(string_line_2)

	# Post data to nodejs server
	
	print(data)
	r = requests.post(url, data)
	print(r.content)

	# Publish data to Netpie
	try:
		microgear.publish("/temperature", temp_value)
		microgear.publish("/humidity", humid_value)
		microgear.publish("/flame", flame_value)
		print("Publish data to Netpie Sucessfully")
	except:
		print("Can't publish data to Netpie")
	time.sleep(1)
	