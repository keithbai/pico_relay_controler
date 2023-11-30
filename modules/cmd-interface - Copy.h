typedef enum {  CMD_STATE_READY=0,          // Default State for the cmd interface
                CMD_STATE_PENDING,          // cmd entry started but not completed
                CMD_STATE_AT_RECIEVED,      // An AT cpommand was entred 
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
cmdStatusEnum cmdStateDataRecieved(void);
cmdStatusEnum cmdStateExecuteCmd(void);

// int cmdChk(void);
// int cmdOrData(char* cmd, int len);
// int cmdLookup(char str[], int len); 
// int cmdExecute(char cmd);
void cmdACK(void);
void cmdNACK(void);

////////////////////////////////////////////////////////////////////////////////
//                         Individual Command routines                        //
////////////////////////////////////////////////////////////////////////////////
void cmdGSN(void);
void cmdAt1(void);
void cmdAt2(void);
void cmdAt3(void);
void cmdD(void);
void cmdCV(void);
void cmdI(void);
void cmdGS(void);       // my routine for getting the state
void cmdIGN(void);
void cmdRV(void);
void cmdSI(void);
void cmdSIM(void);
void cmdUnknown(char* cmd);



typedef enum {  CMD_UNKNOWN=0,  // CMD Not recognised
                CMD_GSN=1,      // Get Serial Number
                CMD_AT1,        // Display the device description
                CMD_AT2,        // Display the device identifier
                CMD_AT3,        // Store the device identifier
                CMD_D,          // set all parameters to default Values
                CMD_CV,         // Calibrate Voltage
                CMD_GS,         // Get Interface state
                CMD_I,          // Print the Version ID
                CMD_IGN,        // Ignition Status
                CMD_IIA,        // ISO Protocol Slow Init Address
                CMD_RV,         // Read the Input Voltage
                CMD_SI,         // Slow Initialisation
                CMD_SIM         // Simulate
                }atCommands;


#define DEVICE_DESCRIPTION "OBDII to RS232 Interpreter 2\n"
#define DEVICE_SERIAL_NUMBER "12345678\n"