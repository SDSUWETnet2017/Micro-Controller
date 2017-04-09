# -*- coding: utf-8 -*-
"""
Created on Sun Apr  9 12:00:57 2017

Weather Engineering Team 2017

A program to simulate transmission from the Subnodes to Supernode

Run on Windows Machine

Pogram needs to be run as administrator

@author: Stephen West
"""


import serial
from time import sleep
print("\nPress crtl+c to end program")
port = serial.Serial(port='COM4', baudrate=9600)

count = 0
num = 2
msg = 'X '+ str(num) + ' 652D 40B7 029EU'

while True:
    port.write(str.encode(msg))
    sleep(1)# wait 1 sec to send another msg
    num += 1
    msg ='X ' + str(num) + ' 652D 40B7 029EU'
    if num == 6:
        #port.write(str.encode('X END 7FFF 7FFF 7FFF 7FFFU'))
        num = 2
        msg ='X '+ str(num) + ' 652D 40B7 029EU'
        sleep(10) #wait 5s to after end cycle
        
