AntiBurglar-pi  
=====  
  
Home security system, based on raspberry-pi.  
System uses gnokii to send sms via gsm phone or modem.
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
