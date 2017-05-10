# IMU_by_CAN
Sending the IMU data by CAN

This code is set up to work with an Arduino. 

Currnetly this code will retrive the data from the 6 DOF IMU built into the Arduino and send it by CAN via a CAN
BUS Shield. 

float2byte is the code to retrive the IMU data and is set up on one arduino. 

recive_check would be uploaded to a second Arduino to recive and check the information.

The CAN_BUS_Shield-master library has been included as that is where I started working from, more indepth information can 
be found here: http://wiki.seeed.cc/CAN-BUS_Shield_V1.2/
