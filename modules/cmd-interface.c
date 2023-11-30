#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "cmd-interface.h"
#include "../usb-relay-controler.h"

// ToDo

// Bugs
//  1: Cant send hex 0xff via USB serial interface so setting the state of all 8 relays to on using the 0x5C set relay states command is not working

cmdStatusEnum cmdState;         // Holds the current state of the interface
enum cmdEventsEnum cmdEvent;    // Holds the current event
exCommands curCmd = 0;
char lastChar;                  // Holds the last character recieved

extern const char SwVer[];
char cmdStr[20];
unsigned int cmdStrPtr=0;
unsigned int lastCmdStrPtr=0; 

int cmdInitIf(void){                    // Initialises the command interface
    cmdState = CMD_STATE_READY;
    cmdEvent = CMD_EV_NONE;
    cmdStrPtr = 0;
    lastCmdStrPtr=0;
    cmdStr[0] = '\0';
    printf("Hello\n");
    cmdACK();
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
    case CMD_STATE_WAIT_FOR_DATA:
        cmdState=cmdStateWaitForData();
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
    case CMD_EV_NONE:
    default:
        newState = CMD_STATE_READY;
        lastChar = getchar_timeout_us(1);
        if(lastChar != 255){
            cmdEvent = CMD_EV_CHAR_RECIEVED;    //Character recieved
        }
        else
        {
            cmdEvent = CMD_EV_NONE;
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
        if(lastChar == 13){                             // Carriage Return (Execute Command)
            if(cmdStrPtr==0){                       // Repeat last command
                cmdStrPtr=lastCmdStrPtr;
                printf("%s", cmdStr);
            }
            cmdStr[cmdStrPtr] = '\0';
            printf("\n");
            lastCmdStrPtr=cmdStrPtr;
            cmdEvent=CMD_EV_CR_RECIEVED;
            newState = CMD_STATE_PENDING;
        }
        else if(lastChar == 56){                        // Recieved "56" (Get serial Number)
            if(cmdStrPtr==0){                       // Only valid cmd if first charachter
                curCmd = CMD_GSN;                             // Get serial Number command
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_EXECUTE_CMD;
            }
            else{
                cmdStr[cmdStrPtr] = lastChar;
                cmdStrPtr++;
                printf("%c", lastChar);
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_PENDING;
            }
        }
        else if(lastChar == 90){                        // Recieved "90" (Get software version)
            if(cmdStrPtr==0){                       // Only valid cmd if first charachter
                curCmd = CMD_GSV;                             // Get software version command
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_EXECUTE_CMD;
            }
            else{
                cmdStr[cmdStrPtr] = lastChar;
                cmdStrPtr++;
                printf("%c", lastChar);
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_PENDING;
            }
        }
        else if(lastChar == 91){                        // Recieved "91" (Get Relay State)
            if(cmdStrPtr==0){                       // Only valid cmd if first charachter
                curCmd = CMD_GRS;                             // Get Relay State command
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_EXECUTE_CMD;
            }
            else{
                cmdStr[cmdStrPtr] = lastChar;
                cmdStrPtr++;
                printf("%c", lastChar);
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_PENDING;
            }
        }
        else if(lastChar == 92){                        // Recieved "92" (Set Relay State)
            if(cmdStrPtr==0){                       // Only valid cmd if first charachter
                newState = CMD_STATE_WAIT_FOR_DATA;
            }
            else{
                printf("%c", lastChar);
                newState = CMD_STATE_PENDING;
            }
            cmdStr[cmdStrPtr] = lastChar;
            cmdStrPtr++;
            cmdEvent=CMD_EV_NONE;
        }
        else if(lastChar == 100){                       // Recieved "100" (Set ALL relays to ON)
            if(cmdStrPtr==0){                       // Only valid cmd if first charachter
                curCmd = CMD_SWAON;                             // Get Relay State command
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_EXECUTE_CMD;
            }
            else{
                cmdStr[cmdStrPtr] = lastChar;
                cmdStrPtr++;
                printf("%c", lastChar);
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_PENDING;
            }
        }
        else if((lastChar > 100) && (lastChar < 109)){  // Recieved "101 - 108" (Set Specific Relay to ON)
            if(cmdStrPtr==0){                           // Only valid cmd if first charachter
                curCmd = CMD_RL_ON;                            // Get Relay State command
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_EXECUTE_CMD;
            }
            else{
                cmdStr[cmdStrPtr] = lastChar;
                cmdStrPtr++;
                printf("%c", lastChar);
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_PENDING;
            }
        }
        else if(lastChar == 110){                       // Recieved "110" (Set ALL relays to OFF)
            if(cmdStrPtr==0){                       // Only valid cmd if first charachter
                curCmd = CMD_SWAOFF;                            // Get Relay State command
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_EXECUTE_CMD;
            }
            else{
                cmdStr[cmdStrPtr] = lastChar;
                cmdStrPtr++;
                printf("%c", lastChar);
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_PENDING;
            }
        }
        else if((lastChar > 110) && (lastChar < 119)){  // Recieved "111 - 118" (Set Specific Relay to OFF)
            if(cmdStrPtr==0){                           // Only valid cmd if first charachter
                curCmd = CMD_RL_OFF;                            // Get Relay State command
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_EXECUTE_CMD;
            }
            else{
                cmdStr[cmdStrPtr] = lastChar;
                cmdStrPtr++;
                printf("%c", lastChar);
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_PENDING;
            }
        }
        else if(lastChar == 127){                       // Backspace
            if(cmdStrPtr>0){
                cmdStrPtr--;
                cmdStr[cmdStrPtr] = '\0';
                printf("%c", lastChar);
            }
            cmdEvent=CMD_EV_NONE;
            newState = CMD_STATE_PENDING;
        }
        else if((lastChar>31) && (lastChar<127)){       // Valid Charater
            cmdStr[cmdStrPtr] = lastChar;
            cmdStrPtr++;
            printf("%c", lastChar);
            cmdEvent=CMD_EV_NONE; 
            newState = CMD_STATE_PENDING;              
        }
        else{                                           // Invalid Character do nothing                  
            cmdEvent=CMD_EV_NONE;
            newState = CMD_STATE_PENDING;
        }
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
        
    }
    return CMD_STATE_EXECUTE_CMD;
}

cmdStatusEnum cmdStateWaitForData(void){
    cmdStatusEnum newState;
    switch (cmdEvent)
    {
    case CMD_EV_CHAR_RECIEVED:
        cmdStr[cmdStrPtr] = lastChar;
        cmdStrPtr++;
        switch(cmdStr[0]){
            case 92:
                if(cmdStrPtr == 2){
                    curCmd = CMD_SRS;                             // Set Relay State command
                    cmdEvent=CMD_EV_NONE;
                    newState = CMD_STATE_EXECUTE_CMD;
                }
                break;
            default:
                cmdEvent=CMD_EV_NONE;
                newState = CMD_STATE_WAIT_FOR_DATA;
        }
        break;

    default:
        newState = CMD_STATE_WAIT_FOR_DATA;
        lastChar = getchar_timeout_us(1);
        if(lastChar != 255){
            cmdEvent = CMD_EV_CHAR_RECIEVED;        //Character recieved
        }
        //PICO_ERROR_TIMEOUT
        break;
    }
    return newState;
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
        case CMD_GSN:       // Get Serial Number
            cmdGSN();
            break;
        case CMD_GSV:       // Get Software Version
            cmdGSV();
            break;
        case CMD_GRS:       // Get Relay States
            cmdGRS();
            break;
        case CMD_SRS:       // Set Relay States
            cmdSRS();
            break;
        case CMD_SWAON:     // Set All Relays to ON
            cmdSWAON();
            break;
        case CMD_SWAOFF:    // Set All Relays to OFF
            cmdSWAOFF();
            break;   
        case CMD_RL_ON:     // Set Relay 01 ON
            cmdRLON();
            break;   
        case CMD_RL_OFF:     // Set Relay 01 ON
            cmdRLOFF();
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

void cmdGSN(void){                      // Get serial number
    cmdACK();
    printf(DEVICE_SERIAL_NUMBER);
}

void cmdGSV(void){                      // Get software version
    cmdACK();
    putchar_raw(10);
    putchar_raw(3);
}

void cmdGRS(void){                      // Get relay states
    cmdACK();
    //gpio_get_all() & 0x0000000F
    //gpio_get(RLY_0),
    //putchar_raw(254);
    putchar_raw(gpio_get_all() & 0x000000FF);
}

void cmdSRS(void){                      // Set relay states
    cmdACK();
    if((cmdStr[1] & 0x01)){             // Relay 0
        gpio_put(RLY_0, RLY_ON); 
    }
    else{
        gpio_put(RLY_0, RLY_OFF); 
    }
    
    if((cmdStr[1] & 0x02)){             // Relay 1
        gpio_put(RLY_1, RLY_ON); 
    }
    else{
        gpio_put(RLY_1, RLY_OFF); 
    }
    
    if((cmdStr[1] & 0x04)){             // Relay 2
        gpio_put(RLY_2, RLY_ON); 
    }
    else{
        gpio_put(RLY_2, RLY_OFF); 
    }
    
    if((cmdStr[1] & 0x08)){             // Relay 3
        gpio_put(RLY_3, RLY_ON); 
    }
    else{
        gpio_put(RLY_3, RLY_OFF); 
    }
    
    if((cmdStr[1] & 0x10)){             // Relay 4
        gpio_put(RLY_4, RLY_ON); 
    }
    else{
        gpio_put(RLY_4, RLY_OFF); 
    }
    
    if((cmdStr[1] & 0x20)){             // Relay 5
        gpio_put(RLY_5, RLY_ON); 
    }
    else{
        gpio_put(RLY_5, RLY_OFF); 
    }
    
    if((cmdStr[1] & 0x40)){             // Relay 6
        gpio_put(RLY_6, RLY_ON); 
    }
    else{
        gpio_put(RLY_6, RLY_OFF); 
    }
    
    if((cmdStr[1] & 0x80)){             // Relay 7
        gpio_put(RLY_7, RLY_ON); 
    }
    else{
        gpio_put(RLY_7, RLY_OFF); 
    }

}

void cmdSWAON(void){                    // Set all relays to ON
    cmdACK();
    gpio_put(RLY_0, RLY_ON);
    gpio_put(RLY_1, RLY_ON);
    gpio_put(RLY_2, RLY_ON);
    gpio_put(RLY_3, RLY_ON);
    gpio_put(RLY_4, RLY_ON);
    gpio_put(RLY_5, RLY_ON);
    gpio_put(RLY_6, RLY_ON);
    gpio_put(RLY_7, RLY_ON);
    
}

void cmdSWAOFF(void){                   // Set all relays to OFF
    cmdACK();
    gpio_put(RLY_0, RLY_OFF);
    gpio_put(RLY_1, RLY_OFF);
    gpio_put(RLY_2, RLY_OFF);
    gpio_put(RLY_3, RLY_OFF);
    gpio_put(RLY_4, RLY_OFF);
    gpio_put(RLY_5, RLY_OFF);
    gpio_put(RLY_6, RLY_OFF);
    gpio_put(RLY_7, RLY_OFF); 
}

void cmdRLON(void){                     // Set Single relay ON
    cmdACK();
    switch (lastChar)
        {
        case 101:
            gpio_put(RLY_0, RLY_ON);
            break;
        case 102:
            gpio_put(RLY_1, RLY_ON);
            break;
        case 103:
            gpio_put(RLY_2, RLY_ON);
            break;
        case 104:
            gpio_put(RLY_3, RLY_ON);
            break;
        case 105:
            gpio_put(RLY_4, RLY_ON);
            break;
        case 106:
            gpio_put(RLY_5, RLY_ON);
            break;
        case 107:
            gpio_put(RLY_6, RLY_ON);
            break;
        case 108:
            gpio_put(RLY_7, RLY_ON);
            break;
        default:
            break;
        }
}

void cmdRLOFF(void){                     // Set Single relay OFF
    cmdACK();
    switch (lastChar)
        {
        case 111:
            gpio_put(RLY_0, RLY_OFF);
            break;
        case 112:
            gpio_put(RLY_1, RLY_OFF);
            break;
        case 113:
            gpio_put(RLY_2, RLY_OFF);
            break;
        case 114:
            gpio_put(RLY_3, RLY_OFF);
            break;
        case 115:
            gpio_put(RLY_4, RLY_OFF);
            break;
        case 116:
            gpio_put(RLY_5, RLY_OFF);
            break;
        case 117:
            gpio_put(RLY_6, RLY_OFF);
            break;
        case 118:
            gpio_put(RLY_7, RLY_OFF);
            break;
        default:
            break;
        }   
}

