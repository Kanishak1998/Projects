#esp file run
from machine import Pin
import dht
class device:
  mode=0# Flag variable to stimulate on/off
  led=Pin(16,Pin.OUT)#led to blink whenever power on
  led1=Pin(14,Pin.OUT)#led to bink whenaver a data is read
  d=dht.DHT11(Pin(4,Pin.OUT))#make an instance for the pin 4 to work with sensor dht11
  """
  Name:Constutor for the class device
  desc:Initializae a variable 'client ' and connect to server and subscribe the topic
  param:self
  return:NULL
  """
  def __init__(self):
    global client_id, mqtt_server, topic_sub
    try:
      self.client = MQTTClient(client_id, mqtt_server)
      self.client.set_callback(self.sub_cb)
      self.client.connect()
      self.client.subscribe(topic_sub)
      print('Connected to %s MQTT broker, subscribed to %s topic' % (mqtt_server, topic_sub))
    except OSError as e:
      self.restart_and_reconnect()
  """
  Name:getData
  Desc:Measure data from sensor and send to server 
  Param:NULL
  Return:NULL
  """
  def getData(self):
    global topic_pub
    self.d.measure()
    t=self.d.temperature()#store temp
    h=self.d.humidity()#store humidity
    serialno="Serial no:1234      "#store serial no.
    msg1=('Temperature:{0:3.1f}*C     '.format(t))
    msg2=('Humiditty:{0:3.1f}%'.format(h))
    msg=serialno+msg1+msg2
    self.client.publish(topic_pub,msg)
  """
  Name:wifi
  Desc:To change from the connected wifi modules
  Param:NULL
  Return:NULL
  """
  def wifi(self):
    global ssid,password 
    station.active(True)
    station.connect(ssid, password)
    while station.isconnected() == False:
      pass
    print('Connection successful') 
    print(station.ifconfig())
    self.client=MQTTClient(client_id, mqtt_server)
    self.client.set_callback(self.sub_cb)
    self.client.connect()
    self.client.subscribe(topic_sub)
    print('Connected to %s MQTT broker, subscribed to %s topic' % (mqtt_server, topic_sub))
    self.client.publish(topic_pub,'Wifi_Credentials_changed')
    
  """
  Name:turnon
  Desc:turn the device on and stars reading fromthe server
  Param:NULL
  Return:NULL /Print out the Reading from DHT11
  """
  def turnon(self):
    global d,led1
    self.led1.on()
    time.sleep(1)
    self.led1.off()
    self.d.measure()
    t=self.d.temperature()#store temp
    h=self.d.humidity()#store humidity
    serialno="Serial_no:1234 "#store serial no.
    msg1=('Temperature:{0:3.1f}C '.format(t))
    msg2=('Humiditty:{0:3.1f}%'.format(h))
    msg=serialno+msg1+msg2
    #print(msg)#reading from DHT11
    #print(msg.decode("uft-8")) 
    self.client.publish(topic_pub,msg)
  """
  Name:sub_cb
  desc:interupt call this fucnction whenever there is an incomming msg on the subscibed topic this prints the msg and turn led on and off
  param:topic name and the message recieved
  return:NULL
  """
  def sub_cb(self,topic, msg):
    global ssid,password
    if topic == b'command' and msg == b'turnon':#turn on the Sensor
      print("Turning Device On")
      #while True
      self.mode=1
      self.client.publish(topic_pub,"Device_Turned_ON")
    elif topic == b'command' and msg == b'turnoff':#turn off the Sensor
      print("Turning decvice off")  
      self.mode=0
      self.client.publish(topic_pub,"Device_Turned_Off")
    elif topic == b'command'and msg[0:4] == b'wifi':#cahnge wifi credentials
      print("Changing wifi credential")  
      message=msg.decode("uft-8").split(' ')
      ssid = message[1]
      password =message[2]
      station.active(False)
      print(ssid)
      print(password)
      self.wifi()
    elif topic == b'command' and msg== b'data':#send the reading to the server
      self.getData()
    elif topic == b'command' and msg== b'status':#send the status for the server for the device
      if self.mode==1:
        self.client.publish(topic_pub,"Device_On")
      else:
        self.client.publish(topic_pub,"Device_Off")
  
  """
  Name:restart_and_reconnect
  desc:In case of OS error reset the machine.
  param:NULL
  return:NULL
  """
  def restart_and_reconnect(self):
    print('Failed to connect to MQTT broker. Reconnecting...')
    time.sleep(10)
    machine.reset()

  
  """
  Name:loop
  desc:Continously check msg on subscibed topic and handle on/off device mode
  param:NULL
  return:NULL
  """
  def loop(self):
    while True:
      try:
        while True:
          self.client.check_msg()#check for msg on sub topic
          time.sleep(0.2)
          if self.mode==1:
            self.turnon()
            time.sleep(1)
          else: 
            break
        #client1.publish(topic_pub,"recived") #publish the msg to the topic#Check is any msg if yes call the interupt which call the function sun_cb
        time.sleep(0.5)
      except OSError as e:
        self.restart_and_reconnect()

def start():
  cli=device()#init object to setup the device
  cli.loop()#start thr device

if __name__ == "__main__":
    start()

