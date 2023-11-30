#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "cmd-interface.h"
#include "../usb-relay-controler.h"

// ToDo
// Text to ack & nak routines
// get real voltage from analog input
// add slow wakeup next phase (Reading sync pattern)

cmdStatusEnum cmdState;     // Holds the current state of the interface
enum cmdEventsEnum cmdEvent;    // Holds the current event
atCommands curCmd = 0;
char lastChar;                  // Holds the last character recieved

extern const char SwVer[];
char deviceId[13];
char cmdStr[20];
unsigned int cmdStrPtr=0;
unsigned int lastCmdStrPtr=0; 
float voltsOffset;



int cmdInitIf(void){                    // Initialises the command interface
    cmdState = CMD_STATE_READY;
    cmdEvent = CMD_EV_NONE;
    cmdStrPtr = 0;
    lastCmdStrPtr=0;
    cmdStr[0] = '\0';
    deviceId[0]='\0';
    //cmdD();
    voltsOffset=0;
    printf("%s\n", SwVer);
}

int cmdIfStateMachine(void){
    switch (cmdState)
    {
    case CMD_STATE_READY:
        cmdState=cmdStateReady();
        break;
    case CMD_STATE_PENDING:
        cmdState=cmdStatePending();
        break;
    case CMD_STATE_AT_RECIEVED:
        cmdState=cmdStateATRecieved();
        break;
    case CMD_STATE_DATA_RECIEVED:
        cmdState=cmdStateDataRecieved();
        break;
    case CMD_STATE_EXECUTE_CMD:
        cmdState=cmdStateExecuteCmd();
        break;
    default:
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//                          State management routines                         //
////////////////////////////////////////////////////////////////////////////////

cmdStatusEnum cmdStateReady(void){  
    cmdStatusEnum newState;
    switch (cmdEvent)
    {
    case CMD_EV_CHAR_RECIEVED:
        newState = CMD_STATE_PENDING;
        break;
    
    default:
        newState = CMD_STATE_READY;
        lastChar = getchar_timeout_us(1);
        if(lastChar != 255){
            cmdEvent = CMD_EV_CHAR_RECIEVED;    //Character recieved
        }
        break;
    }
    return newState;
}

cmdStatusEnum cmdStatePending(void){
    cmdStatusEnum newState;
    switch (cmdEvent)
    {
    case CMD_EV_CHAR_RECIEVED:
        if(lastChar == 13){                         // Carriage Return (Execute Command)
            if(cmdStrPtr==0){                       // Repeat last command
                cmdStrPtr=lastCmdStrPtr;
                printf("%s", cmdStr);
            }
            cmdStr[cmdStrPtr] = '\0';
            printf("\n");
            lastCmdStrPtr=cmdStrPtr;
            cmdEvent=CMD_EV_CR_RECIEVED;
        }else if(lastChar == 56){                    // Recieved "8" (Get Serial Number command)
            curCmd = CMD_GSN;                             // Get Serial Number command
            cmdEvent=CMD_STATE_EXECUTE_CMD;
        }else if(lastChar == 127){                   // Backspace
            if(cmdStrPtr>0){
                cmdStrPtr--;
                cmdStr[cmdStrPtr] = '\0';
                printf("%c", lastChar);
            }
            cmdEvent=CMD_EV_NONE;
        }
        else if((lastChar>31) && (lastChar<127)){   // Valid Charater
            cmdStr[cmdStrPtr] = lastChar;
            cmdStrPtr++;
            printf("%c", lastChar);
            cmdEvent=CMD_EV_NONE;               
        }
        else{                                       // Invalid Character do nothing
            printf("%c", 0);                    
            cmdEvent=CMD_EV_NONE;
        }
        newState = CMD_STATE_PENDING;
        break;

    case CMD_EV_CR_RECIEVED:
        cmdEvent=CMD_EV_NONE;
        newState = CMD_STATE_DATA_RECIEVED;
        switch (cmdStr[0]){
        case 'a':
        case 'A':
            switch (cmdStr[1]){
                case 't':
                case 'T':
                    newState = CMD_STATE_AT_RECIEVED;
                    break;
                default:
                    break;
            }
            break;
        default:
            // printf("DATA: 00 00 00\n");             // Data
            break;
        }
        break;

    default:
        newState = CMD_STATE_PENDING;
        lastChar = getchar_timeout_us(1);
        if(lastChar != 255){
            cmdEvent = CMD_EV_CHAR_RECIEVED;        //Character recieved
        }
        break;
    }
    return newState;
}

cmdStatusEnum cmdStateATRecieved(void){
    curCmd = CMD_UNKNOWN;
    if(cmdStr[2] = 32){        // Space 
        if((cmdStr[3]=='c')||(cmdStr[3]=='C')){
            if((cmdStr[4]=='v')||(cmdStr[4]=='V')){
            curCmd = CMD_CV;            // Calibrate Voltage Cmd
            }         
        }
        else if((cmdStr[3]=='d')||(cmdStr[3]=='D')){
            curCmd = CMD_D;             // set all parameters to default Values
        }             
        else if((cmdStr[3]=='g')||(cmdStr[3]=='G')){
            if((cmdStr[4]=='s')||(cmdStr[4]=='S')){
                if(cmdStr[5]=='\0'){
                    curCmd = CMD_GS;   // Get Interface Status
                }
            }
        }             
        else if((cmdStr[3]=='i')||(cmdStr[3]=='I')){
            if(cmdStr[4]=='\0'){
                curCmd = CMD_I;         // Version ID Cmd
            }
        }             
        else if((cmdStr[3]=='r')||(cmdStr[3]=='R')){
            if((cmdStr[4]=='v')||(cmdStr[4]=='V')){
            curCmd = CMD_RV;            // Read Voltage Cmd 
            }         
        }
        else if((cmdStr[3]=='s')||(cmdStr[3]=='S')){
            if((cmdStr[4]=='i')||(cmdStr[4]=='I')){
                if((cmdStr[5]=='m')||(cmdStr[5]=='M')){
                    if(cmdStr[6]=='\0'){
                        curCmd = CMD_SIM;   // Simulation Mode
                    }
                }
            }         
        }      
        else if(cmdStr[3]=='@'){
            if(cmdStr[4]=='1'){
                curCmd = CMD_AT1;       // Display the device description
            }
            else if(cmdStr[4]=='2'){
                curCmd = CMD_AT2;       // Display the device identifier
            }
            else if(cmdStr[4]=='3'){
                curCmd = CMD_AT3;       // Store the device identifier
            }
        }
    }
    return CMD_STATE_EXECUTE_CMD;
}

cmdStatusEnum cmdStateDataRecieved(void){
    cmdACK();
    printf("DATA: 00 00 00\n");             // Data
    cmdStrPtr=0;
    return CMD_STATE_READY;
}

cmdStatusEnum cmdStateExecuteCmd(void){
    switch (curCmd)
        {
        case CMD_GSN:   // Get Device Serial Number
            cmdGSN();
            break;
        case CMD_AT1:   // Display the device description
            cmdAt1();
            break;
        case CMD_AT2:   // Display the device identifier
            cmdAt2();
            break;
        case CMD_AT3:   // Store the device identifier
            cmdAt3();
            break;
        case CMD_CV:    // Calibrate Voltage
            cmdCV();
            break;
        case CMD_I:     // Interface Identification
            cmdI();
            break;
        case CMD_RV:    // Reav Voltage
            cmdRV();
            break;
        case CMD_SIM:   // Simulation Mode
            cmdSIM();
            break;
        case CMD_UNKNOWN:   // Unknown cmd
            cmdUnknown(cmdStr);
        default:
            break;
        }
        curCmd = CMD_UNKNOWN;
        cmdStrPtr=0;
        return CMD_STATE_READY;
}


////////////////////////////////////////////////////////////////////////////////
//                               Other routines                               //
////////////////////////////////////////////////////////////////////////////////

void cmdACK(void){                      // Command Acknowledged (ACK)
    gpio_put(LED_PIN, 0); 
    sleep_ms(150);
    gpio_put(LED_PIN, 1); 
}

void cmdNACK(void){                     // Command Not Acknowledged (NAK)
    gpio_put(LED_PIN, 0); 
    sleep_ms(150);
    gpio_put(LED_PIN, 1);
    sleep_ms(150);
    gpio_put(LED_PIN, 0);
    sleep_ms(150);
    gpio_put(LED_PIN, 1);
}

void cmdUnknown(char* cmd){             // Handles unknown AT commands
    cmd = cmd+3;
    printf("Unrecognised command '%s'\n", cmd);
    cmdNACK();
}

void cmdGSN(void){                     // Get device serial number
    cmdACK();
    printf(DEVICE_SERIAL_NUMBER);
}

void cmdAt1(void){                      // Display the device description
    if(cmdStrPtr==5){
        cmdACK();
        printf(DEVICE_DESCRIPTION);
    }
    else{
        printf("?\n");
        cmdNACK();
    }
}

void cmdAt2(void){                      // Display the device identifier
    if(cmdStrPtr==5){
        if(strlen(deviceId) == 12){
            cmdACK();
            printf("%s\n", deviceId);
            return;
        }
    }
    printf("?\n");
    cmdNACK();
}

void cmdAt3(void){                      // Store the device identifier
    if(cmdStrPtr==18){
        cmdACK();
        for (int i = 0; i < 12; i++)
        {
            deviceId[i] = cmdStr[i+6];
        }
        deviceId[12] = '\0';
        printf("OK\n");
    }
     else{
        printf("?\n");
        cmdNACK();
    }
}

void cmdCV(void){                       // Calibrate Voltage
    if(cmdStrPtr==10){
        cmdACK();
        char tmpStr[5];
        int tmpVal;
        tmpStr[0] = cmdStr[6];
        tmpStr[1] = cmdStr[7];
        tmpStr[2] = cmdStr[8];
        tmpStr[3] = cmdStr[9];
        tmpStr[4] = '\0';
        tmpVal = atoi(tmpStr);
        if(tmpVal==0){
            voltsOffset = 0;
        }
        else
        {
            voltsOffset = (float)tmpVal/100; 
        }
        printf("OK\n");
    }
    else{
        printf("?\n");
        cmdNACK();
    }
}

void cmdI(void){                        // Interface Identification
    if(cmdStrPtr==4){
        printf("%s\n", SwVer);
        cmdACK();
    }
    else{
        printf("?\n");
        cmdNACK();
    }
}

void cmdRV(void){                       // Read Voltage
    printf("%.2fV\n", voltsOffset);
}

void cmdSIM(void){                      // Simulation Mode
    if(cmdStrPtr==6){
        printf("Simulation Mode\n");
        cmdACK();
        // wakeUp5BaudRx();
    }
    else{
        printf("?\n");
        cmdNACK();
    }
}


