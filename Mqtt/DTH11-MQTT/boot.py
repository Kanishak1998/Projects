import time
from umqtt import MQTTClient
import ubinascii
import machine
import micropython
import network
import esp
esp.osdebug(None)
import gc
gc.collect()

ssid = 'I'#wifi hostname
password = 'qweasdzxc'#wifi password
mqtt_server = '13.126.163.240'

client_id = ubinascii.hexlify(machine.unique_id())
topic_sub = b'command'
topic_pub = b'output'

#wifi Connection
station = network.WLAN(network.STA_IF)
station.active(True)
station.connect(ssid, password)
while station.isconnected() == False:
  pass
print('Connection successful')
print(station.ifconfig())
