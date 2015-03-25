#include <syslog.h>


void syslog_write(char * msg){
        setlogmask (LOG_UPTO (LOG_NOTICE));
        openlog ("antiburglar-pi", LOG_CONS | LOG_NDELAY, LOG_LOCAL1);
        syslog (LOG_NOTICE, "%s",msg);
        closelog ();
    
}
