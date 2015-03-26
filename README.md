AntiBurglar-pi  
=====  
  
Home security system, based on raspberry-pi.  
System monitors two security zones, two temperature zones. Uses gnokii to send sms via gsm phone or modem, activates  
siren via relay. Also monitors status of powerline, reports if power is down  
Deactivation of armed system is done via SMS command. After sending "disarm password" (e.g disarm 1111) to abp GSM phone number, system deactivates within one minute.  
After sending "status password" (eg. status 1111) command, system responds with device uptime and temperature status within one minute.  
Alarm warning is sent do a predefined number in config.h file.  
  
Edit src/config.h to fit your needs.  
You need to install libgnokii development libraries, also wiringpi:  
http://wiringpi.com/download-and-install/  
Make sure to create /etc/gnokii.conf config file with configuration for your GSM modem or phone.  
Example of working gnokii.conf file with "Huawei Technologies Co., Ltd. E220 HSDPA Modem / E230/E270/E870 HSDPA/HSUPA" modem:  
  
[global]  
model = AT  
port = /dev/ttyUSB0  
connection = serial  

To build and install system  
make  
make install  

If using raspbian distribution, copy init script from debian_init folder  
to /etc/init.d/ folder. 
Start antiburglar-pi wit /etc/init.d/abp start  
  
  
v0.1a some fixes, init scripts.  
v0.1 Initial release  
