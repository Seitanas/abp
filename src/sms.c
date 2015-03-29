#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gnokii.h>
#include <signal.h>
#include "main.h"
#include <sys/stat.h>
#include "globals.h"
#include "config.h"
#include <wiringPi.h>
/*
 * 
 */

static struct gn_statemachine *state=NULL;

void bus_terminate(void){
    gn_lib_phone_close(state);
    gn_lib_phoneprofile_free(&state);
}

void bus_init(void){
    gn_lib_phoneprofile_load_from_file("/etc/gnokii.conf",NULL,&state);
    gn_lib_phone_open(state);

}

gn_error gnokii_send(char *nr,char *msg){
    static gn_sms_message_center msg_center;
    gn_data *data;
    gn_sms sms;
    if(!state){
	bus_init();
        data=&state->sm_data;
	data->sms=&sms;
	gn_sms_default_submit(data->sms);
        data->message_center=&msg_center;
	data->message_center->id=1;
	gn_sm_functions(GN_OP_GetSMSCenter, data,   state);
    }
    data=&state->sm_data;
    data->sms=&sms;
    gn_sms_default_submit(data->sms);
    snprintf(sms.smsc.number, sizeof(sms.smsc.number), "%s",msg_center.smsc.number);
    sms.smsc.type=msg_center.smsc.type;
    snprintf(sms.remote.number,sizeof(sms.remote.number), "%s", nr);
    sms.remote.type=GN_GSM_NUMBER_Unknown;
    sms.user_data[0].type=GN_SMS_DATA_Text;
    sprintf(sms.user_data[0].u.text, "%.918s", msg);
    sms.user_data[0].length=strlen(sms.user_data[0].u.text);
    sms.user_data[1].type=GN_SMS_DATA_None;
    gn_sms_send(data, state);
    bus_terminate();
    state=NULL;
}

void getsms(void){
    gn_data *data;
    gn_error error;
    bus_init();
    data = &state->sm_data;
    char sms_command[100];

    gn_sms_folder folder;
    gn_sms_folder_list folderlist;
    gn_sms message;
    message.memory_type = GN_MT_IN;
    message.number = 0;
    message.remote.type = GN_GSM_NUMBER_International;
    folder.folder_id = 0;
    data->sms = &message;
    data->sms_folder = &folder;
    data->sms_folder_list = &folderlist;
    memset(&message, 0, sizeof(gn_sms));
    gn_sms_get(data, state);
    sms_sender=malloc(sizeof(message.remote.number));
    sprintf(sms_sender,"%s",message.remote.number);
	if(strlen(message.user_data[0].u.text)!=0){
		char temp_message[1024];
	        sprintf(temp_message,"Got SMS from %s. Stating that: %s",sms_sender,message.user_data[0].u.text);
	        syslog_write(temp_message);
		gn_sms_delete(data, state);
	}
	strcpy(sms_command,"arm ");
        strcat(sms_command,PASSWORD);
	bus_terminate();
        if (strcmp(sms_command,message.user_data[0].u.text)==0){
	    if (!armed){
		armed=1;
		gnokii_send(sms_sender,"System armed.");
        	syslog_write("System armed.");
		call_led_thread();
	    }
	}
	strcpy(sms_command,"status ");
        strcat(sms_command,PASSWORD);
        if (strcmp(sms_command,message.user_data[0].u.text)==0){
	    send_status=1;
	}
	strcpy(sms_command,"disarm ");
        strcat(sms_command,PASSWORD);
        if (strcmp(sms_command,message.user_data[0].u.text)==0){
	    disarm=1;
	}
	strcpy(sms_command,"siren on ");
        strcat(sms_command,PASSWORD);
        if (strcmp(sms_command,message.user_data[0].u.text)==0){
	    digitalWrite(SIREN,HIGH);
	    gnokii_send(sms_sender,"Turning siren on.");
    	    syslog_write("Turning siren on.");
	}
	strcpy(sms_command,"siren off ");
        strcat(sms_command,PASSWORD);
        if (strcmp(sms_command,message.user_data[0].u.text)==0){
	    digitalWrite(SIREN,LOW);
	    gnokii_send(sms_sender,"Turning siren off.");
    	    syslog_write("Turning siren off.");
	}



}
