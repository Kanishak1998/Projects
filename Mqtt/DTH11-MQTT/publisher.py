#run on pc's
import time
import paho.mqtt.client as paho
import smtplib
import MySQLdb
conn=MySQLdb.connect(host='localhost',user='root',passwd='test',db='logfiles')
cursor = conn.cursor()

broker="13.126.163.240"
class publisher:
  """
  Name:(Constutor for the class pubisher)
  desc:Initializae a variable 'client ' and connect to server using the variable subcribe to topic
  param:self
  return:NULL
  """
  def __init__(self):
    self.client= paho.Client("client-001")
    self.client.on_message=self.on_message
    print("connecting to broker ",broker)
    self.client.connect(broker)#connect
    self.client.loop_start() #start loop to process received messages
    print("subscribing ")
    self.client.subscribe(b'output')#subscribe
  """
  name:create_table
  desc:create table for the newly registered device.
  param:name of the table
  return:NULL
  """
  def create_table(self,name):
    global conn,cursor
    query='create table %s(Serial_no int(10),Temperature int(10),Humidity int(10))'%(name)
    cursor.execute(query)
    print('New table created')
    conn.commit()
  """
  name:on_message
  desc:interupt call this fucnction whenever there is an incomming msg on the subscibed topic this prints the msg in list 
  param:topic name and the message recieved
  return:NULL
  """
  def on_message(self,client, userdata, message):
    global conn,cursor
    time.sleep(5 )
    received_data =(str(message.payload.decode("utf-8"))).split(' ')
    length=len(received_data)
    print(received_data)
    if(length==3):
      query='SELECT * from device Where serial_no=%s'%(received_data[0][10:])
      result=cursor.execute(query)
      if result==0:
        self.create_table('serial'+received_data[0][10:]) 
        query1='INSERT INTO device values(%s)'%received_data[0][10:];
        l=cursor.execute(query1)
        conn.commit()
      command='INSERT INTO serial%s values(%s,%s,%s);'%(received_data[0][10:],received_data[0][10:],received_data[1][12:16],received_data[2][10:14])
      cursor.execute(command)
      conn.commit()
      time.sleep(5)
      print('Database Updated')
  """
  Name:send_email()
  desc:send notification to the user email id about device on off
  param:subject and msg for the mail
  return:NULL
  """
  def send_email(self,subject, msg):
    try:
        server = smtplib.SMTP('smtp.gmail.com:587')
        server.ehlo()
        server.starttls()
        server.login('kanishkkt3@gmail.com','kani37528311')
        message = 'Subject: {}\n\n{}'.format(subject, msg)
        server.sendmail('kanishkkt3@gmail.com','kanishak@yopmail.com', message)
        server.quit()
        print("Success: Email sent!")
    except:
        print("Email failed to send.")

  """
  Name:run
  desc:Continously check msg on subscibed topic and handle commands from the stdin input
  return:NULL
  """
  def run(self):
    global conn
    while True:
      print('\n1.Turnon\n2.Turnoff\n3.Get Data\n4.Change wifi Configration\n5.Status \n')
      choice=input()
      if(choice==1):
        self.client.publish(b'command',b'turnon')#publish
        subject = "Device Status"
        msg = "Device is on"
        self.send_email(subject, msg)
      elif(choice==2):
        self.client.publish(b'command',b'turnoff')
        subject = "Device Status"
        msg = "Device is off"
        self.send_email(subject, msg)
      elif(choice==3):
        self.client.publish(b'command',b'data')
      elif(choice==4):
        m='wifi'
        ssid=input("Enter the ssid:")
        password=input("Enter the password:")
        msg=m+' '+ssid+' '+password
        print(msg)
        self.client.publish(b'command',msg)
      elif(choice==5):
        self.client.publish(b'command',b'status')
      time.sleep(4)

def start():
  cli=publisher()#init object to setup the device
  cli.run()#start thr device

if __name__ == "__main__":
    start()

