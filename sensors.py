#!/usr/bin/env python

# Sensor code is taken from here: 
# http://learn.adafruit.com/adafruits-raspberry-pi-lesson-12-sensing-movement

# pexpect is used to start and stop wave file - 
# http://pexpect.sourceforge.net/pexpect.html

# Install package gpac in order to convert .h264 to .mp4

# Email sending code is taken from here: 
# http://stackoverflow.com/questions/10147455/trying-to-send-email-gmail-as-mail-provider-using-python

import time, os, pexpect, subprocess, shlex, smtplib, serial
import RPi.GPIO as gpio
gpio.setmode(gpio.BCM)

pir_pin = 18
door_pin = 23

systemArmed = False

validCode = '010004F7B2'

gpio.setup(pir_pin, gpio.IN)
gpio.setup(door_pin, gpio.IN, pull_up_down=gpio.PUD_UP)

#time.sleep(10)

def send_email():
        SMTP_SERVER = ''
        SMTP_PORT = 587
        
        sender = ''
        password = ''
        recipient = ''
        subject = 'INTRUDER ALERT'
        body = 'An intruder has been detected. <br/> Date and Time: ' + time.strftime("%Y-%m-%d_%H:%M:%S", time.localtime()) + '<br/> Go to http://192.168.1.7/latestvideo.php to view video surveillance.'
        
        # Sends an e-mail to the specified recipient.
        
        body = "" + body + ""
        
        headers = ["From: " + sender,
                   "Subject: " + subject,
                   "To: " + recipient,
                   "MIME-Version: 1.0",
                   "Content-Type: text/html"]
        headers = "\r\n".join(headers)
        
        server = smtplib.SMTP(SMTP_SERVER, SMTP_PORT)
        
        server.ehlo()
        server.starttls()
        server.ehlo
        server.login(sender, password)
        #server.set_debuglevel(1)
        server.sendmail(sender, recipient, headers + "\r\n\r\n" + body)
        server.quit()
        print "Email has been sent"

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=2)
print ser.readline()
ser.write("PI READY")

while True:
    dirlist = subprocess.Popen(['ls', '/var/www/'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = dirlist.communicate()
    if out.find('takepicture.txt')!=-1:
        print "Taking picture"
        currenttime = time.strftime("%Y-%m-%d_%H:%M:%S", time.localtime())
        command = 'raspistill -vf -hf -o /var/www/camera/' + currenttime + '.jpg'
        args = shlex.split(command)
        subprocess.Popen(args)
        os.system('rm /var/www/takepicture.txt')

    elif out.find('servoleft.txt')!=-1:
	print "Moving servo left"
	ser.write('SERVO LEFT')
	os.system('rm /var/www/servoleft.txt')
	time.sleep(.5)
    
    elif out.find('servoright.txt')!=-1:
	print "Moving servo right"
	ser.write('SERVO RIGHT')
	os.system('rm /var/www/servoright.txt')
	time.sleep(.5)

    text = ser.readline()
    if systemArmed and text.find("DOOR OPENED")!=-1: # Wait for door to open when system is activated
        # Print out message and play siren sound until door is closed again
        print "DOOR ALARM!"
        time.sleep(2)
        currenttime = time.strftime("%Y-%m-%d_%H:%M:%S", time.localtime())
        a = pexpect.spawn('mpg321 --loop 0 /home/pi/sounds/siren.mp3')
        # take 30 second video of incident and convert .h264 to .mp4
        b = pexpect.spawn('raspivid -t 30000 -vf -hf -o /var/www/camera/video.h264')
        waiting = True
        while waiting:
            text = ser.readline()
            if text.find(validCode)!=-1:
                ser.write("DEACTIVATE")
                a.close()
                b.sendcontrol('c')
                waiting = False
                print "Authenticated - System Deactivated"
                print ""
                systemArmed = False
            elif text.find("DOOR CLOSED")!=-1:
                a.close()
                b.sendcontrol('c')
                waiting = False
                print "Door has been closed"
            elif text.find("CODE: ")!=-1:
                ser.write("WRONG CODE")
                
        command = 'MP4Box -add video.h264 output.mp4'
        args = shlex.split(command)
        p = subprocess.Popen(args, cwd='/var/www/camera/')
        p.communicate()
        os.system('mv /var/www/camera/output.mp4 /var/www/camera/' + currenttime + '.mp4')
        if text.find("DOOR CLOSED")!=-1:
            send_email()
        b.close()
        
    elif text.find(validCode)!=-1:
        if systemArmed:
            ser.write("DEACTIVATE")
            systemArmed = False
        else:
            ser.write("ARM SYSTEM")
            systemArmed = True
    elif text.find("CODE: ")!=-1:
        ser.write("WRONG CODE")
    
    time.sleep(0.5)
