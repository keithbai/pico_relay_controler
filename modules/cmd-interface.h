////////////////////////////////////////////////////////////////////////////////
//                       Command Interface State Machine                      //
////////////////////////////////////////////////////////////////////////////////
typedef enum {  CMD_STATE_READY=0,          // Default State for the cmd interface
                CMD_STATE_PENDING,          // cmd entry started but not completed
                CMD_STATE_AT_RECIEVED,      // An AT command was entred 
                CMD_STATE_WAIT_FOR_DATA,    // Recieved Cmd but waiting for associated data
                CMD_STATE_DATA_RECIEVED,    // A data frame was recieved
                CMD_STATE_EXECUTE_CMD,      // Execute the CMD
                CMD_STATE_aaaa,
                }cmdStatusEnum;

enum cmdEventsEnum{
    CMD_EV_NONE=0,  // No Event
    CMD_EV_CHAR_RECIEVED,
    CMD_EV_CR_RECIEVED
};


int cmdInitIf(void);
int cmdIfStateMachine(void);
cmdStatusEnum cmdStateReady(void);
cmdStatusEnum cmdStatePending(void);
cmdStatusEnum cmdStateATRecieved(void);
cmdStatusEnum cmdStateWaitForData(void);
cmdStatusEnum cmdStateDataRecieved(void);
cmdStatusEnum cmdStateExecuteCmd(void);

void cmdACK(void);
void cmdNACK(void);

////////////////////////////////////////////////////////////////////////////////
//                         Individual Command routines                        //
////////////////////////////////////////////////////////////////////////////////
void cmdGSN(void);              // Get Serial Number
void cmdGSV(void);              // Get Software Version
void cmdGRS(void);              // Get Relay State
void cmdSRS(void);              // Set Relay State
void cmdSWAON(void);            // Set All Relays ON
void cmdSWAOFF(void);           // Set All Relays OFF
void cmdRLON(void);             // Set Relay 01 ON
void cmdRLOFF(void);            // Set Relay 01 OFF
void cmdRLOFF(void);            // Set Relay 01 OFF
void cmdUnknown(char* cmd);     // Unknown Command Handler

typedef enum {  CMD_UNKNOWN=0,  // CMD Not recognised
                CMD_GSN=1,      // Get Serial Number
                CMD_GSV,        // Get Software Version
                CMD_GRS,        // Get Relay State
                CMD_SRS,        // Set Relay State
                CMD_SWAON,      // Set All Relays ON
                CMD_SWAOFF,     // Set All Relays OFF
                CMD_RL_ON,      // Set Relay 01 ON
                CMD_RL_OFF      // Set Relay 01 ON
                }exCommands;

#define DEVICE_SERIAL_NUMBER "10020360"
