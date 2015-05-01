AntiBurglar-pi  
=====  
  
Home security system, based on raspberry-pi.  
System monitors two security zones, two temperature zones. Uses gnokii to send sms via gsm phone or modem, activates 
siren via relay.  
Also monitors status of powerline, reports if power is down.  
System control is done via SMS commands:  
  
<b>arm password</b> (e.g arm 1111) - system is armed instantly.  
<b>disarm password</b> - armed system deactivates after receiving SMS.  
<b>status password</b> - system responds with device uptime and temperature status after receiving SMS.  
<b>siren on password</b> - siren is turned on.  
<b>siren off password</b> - siren is turned off.  
  
When triggered, alarm warning is sent do a predefined number in config.h file.  
  
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
Start antiburglar-pi with /etc/init.d/abp start  
  
More information about this project can be found at:  
http://www.neblogas.lt/2015/03/31/antiburglar-pi/  
  
  
  
v0.1c extented status remport info.  
v0.1b some additional features.    
v0.1a some fixes, init scripts.  
v0.1 Initial release  
