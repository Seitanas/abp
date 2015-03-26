all:
	gcc -o abp src/main.c src/sms.c src/globals.c src/syslog.c -lgnokii -lwiringPi -lpthread
install:
	install -m 0777 abp /usr/local/bin