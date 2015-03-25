all:
	gcc -o abp src/main.c src/sms.c src/globals.c src/syslog.c -lgnokii -lwiringPi -lpthread
