import paho.mqtt.client as mqtt
import time

client = mqtt.Client("guarden_client")
flag_connected = 0 # check if client is connected to MQTT broker
