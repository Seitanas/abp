/*
    antiburglar-pi
    Tadas Ustinavičius
    2015-03-31
*/


#include "sms.h"
#include <stdio.h>
#include <sys/sysinfo.h>
#include <locale.h>
#include "globals.h"
#include <wiringPi.h>
#include <pthread.h>
#include "syslog.h"
#include <stdlib.h>
#include "config.h"
int door=0, window=0, pwr=0,arm=0;
int door_sent=0, window_sent=0, pwr_sent=0, arm_sent=0;


void init_gpio(void){
    wiringPiSetup ();    
    pinMode (BUZZER, OUTPUT) ;
    pinMode (ARMED_LED, OUTPUT);
    pinMode(DOOR, INPUT);
    pinMode(WINDOW, INPUT);
    pinMode(POWER, INPUT);
    pinMode(ARM_BUTTON, INPUT);
    pinMode(SIREN, OUTPUT);
    digitalWrite (BUZZER, LOW) ;
    digitalWrite (ARMED_LED, LOW) ;
    digitalWrite (SIREN, LOW);
}


void read_sensors(void){ //read all GPIO inputs, except temperature.
    door=digitalRead(DOOR);
    window=digitalRead(WINDOW);
    pwr=digitalRead(POWER);
    arm=digitalRead(ARM_BUTTON);
}

void * armed_blink(){//blinks LED while system is armed.
    while (armed){
	digitalWrite (ARMED_LED, HIGH) ;
        usleep(200000);
        digitalWrite (ARMED_LED, LOW) ;
	usleep(200000);
    }
    digitalWrite(ARMED_LED,LOW); //reset signal to low
    pthread_exit(0);
}

void arm_beep(void){//beep for period of arming_seconds.
    int x=ARMING_SECONDS*2;
    while (x!=0){
	int pulse=0;
	while (pulse!=100){
	    ++pulse;
	    digitalWrite(BUZZER,HIGH);
	    usleep(650);
	    digitalWrite(BUZZER,LOW);
	    usleep(650);
	}
	usleep(400000);
	--x;
    }
    x=3;
    while (x!=0){//emit 3 short beeps to inform, that system is armed
	int pulse=0;
	while (pulse!=30){
	    ++pulse;
	    digitalWrite(BUZZER,HIGH);
	    usleep(300);
	    digitalWrite(BUZZER,LOW);
	    usleep(300);
	}
	usleep(200000);
	--x;
    }	
    digitalWrite(BUZZER,LOW); //reset signal to low
}

void disarm_beep(){
    int x=4;
    while (x!=0){
	int pulse=0;
	while (pulse!=30){
	    ++pulse;
	    digitalWrite(BUZZER,HIGH);
	    usleep(340);
	    digitalWrite(BUZZER,LOW);
	    usleep(300);
	}
	usleep(200000);
	--x;
    }	
    digitalWrite(BUZZER,LOW); //reset signal to low
}
int read_temp(char *device_path){
    FILE *fp;
    char tmp[80],tmp2[80];
    char junk[80],crc[5],crc2[5];
    int value=0;
    if ((fp = fopen(device_path,"r")) != (FILE *)NULL) {
	fgets(tmp,80,fp); /* Get first line w/ checksums */
        fgets(tmp2,80,fp); /* Get 2nd line w/ temp */
	sscanf(tmp,"%[^:]: %[^ ] %s\n",junk,crc,crc2);
        sscanf(tmp2,"%[^=]=%d\n",junk,&value);
	fclose(fp);
    }
    return value;
}
void daemon(){
        pid_t mypid;
        FILE *pid;
        mypid=fork();
        if (mypid){
                pid=fopen("/var/run/abp/abp.pid","w");
                fprintf(pid,"%i",mypid);
                exit (0);
        }
}
void system_exit(void){
    syslog_write("Stopping antiburglar-pi");
}
void call_led_thread(void){
    pthread_t tid;
    pthread_create(&tid,NULL,armed_blink,NULL);
    pthread_detach(tid);
}
int main (void){
    atexit (system_exit);
    syslog_write("Starting antiburglar-pi");
    init_gpio();
    disarm_beep(); //beep at system startup
    daemon();
    setlocale(LC_ALL, "");
    sleep (1); //wait for dust to settle
    armed=0;
    char temp_message[200];
    int door_sent=0, window_sent=0, power_sent=0;
    int timer=0;
    while (1){//main loop
        read_sensors();
	++timer;
        if (!armed&&!arm){//arm button pressed. gpio pin low
            arm_beep();
            read_sensors();
	    armed=1;
	    gnokii_send(RECIPIENT,"System armed.");
	    syslog_write("System armed.");
	    call_led_thread();
        }
	if (armed&&door&&!door_sent){//system is armed, door was opened, sms was not sent
	    gnokii_send(RECIPIENT,"Door is open, triggering alarm.");
	    digitalWrite(SIREN,HIGH);
	    door_sent=1;
	    syslog_write("Door is open, triggering alarm.");
	}
	if (armed&&window&&!window_sent){//system is armed, window was opened, sms was not sent
	    gnokii_send(RECIPIENT,"Window is open, triggering alarm");
	    digitalWrite(SIREN,HIGH);
	    window_sent=1;
	    syslog_write("Window is open, triggering alarm");
	}
	if (pwr&&!power_sent){
	    power_sent=1;
	    gnokii_send(RECIPIENT,"Power loss detected");
	    syslog_write("Power loss detected");
	}
	if (!pwr&&power_sent){
	    power_sent=0;
	    gnokii_send(RECIPIENT,"Power is back to normal");
	    syslog_write("Power is back to normal");
	}
	if (timer==300){//check inbox every minute 
    	    getsms();
    	    timer=0;
	}
        if (send_status){
	    float temp1=0,temp2=0;
	    temp1=read_temp(SENSOR1);
	    temp2=read_temp(SENSOR2);
    	    send_status=0;
    	    struct sysinfo info;
    	    sysinfo(&info);
	    int days = info.uptime / (60 * 60 * 24);
	    info.uptime -= days * (60 * 60 * 24);
	    int hours = info.uptime / (60 * 60);
	    info.uptime -= hours * (60 * 60);
	    int minutes = info.uptime / 60;
	    info.uptime -= minutes * 60;
	    char uptime[100];
    	    sprintf(uptime,"UP: %i days, %i hours, %i minutes, %i seconds. Load average: %.2f %.2f %.2f Temp1: %.2f Temp2: %.2f",days,hours,minutes, info.uptime,info.loads[0]/65536.0, info.loads[1]/65536.0, info.loads[2]/65536.0,temp1/1000,temp2/1000);
	    sprintf(temp_message,"Client %s is requesting status.",sms_sender);	    
	    syslog_write(temp_message);
	    sprintf(temp_message,"Replying with %s",uptime);
	    syslog_write(temp_message);
	    gnokii_send(sms_sender,uptime);
	}
	if (disarm){
	    disarm=0;
	    armed=0;
	    door_sent=0;
	    window_sent=0;
	    power_sent=0;
	    digitalWrite(SIREN,LOW);
	    syslog_write("System disarmed");
	    gnokii_send(sms_sender,"System disarmed");
	    disarm_beep();
	}
    usleep (200000);
    }
}

