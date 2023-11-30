#include <stdio.h>
#include "pico/stdlib.h"
#include "usb-relay-controler.h"
#include "./modules/cmd-interface.h"

//  ----------  ToDo  ----------

int main(){
    stdio_init_all();                   // Inteface Initialisation Code
    gpio_init(LED_PIN);                 // Initialise GPIO - Green LED is connected to GPIO pin 25
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(RLY_0);                   // Initialise GPIO - Relay 0 is connected to GPIO PIN 0
    gpio_set_dir(RLY_0, GPIO_OUT);
    gpio_put(RLY_0, RLY_OFF); 
    gpio_init(RLY_1);                   // Initialise GPIO - Relay 1 is connected to GPIO PIN 1
    gpio_set_dir(RLY_1, GPIO_OUT);
    gpio_put(RLY_1, RLY_OFF);
    gpio_init(RLY_2);                   // Initialise GPIO - Relay 2 is connected to GPIO PIN 2
    gpio_set_dir(RLY_2, GPIO_OUT);
    gpio_put(RLY_2, RLY_OFF);
    gpio_init(RLY_3);                   // Initialise GPIO - Relay 3 is connected to GPIO PIN 3
    gpio_set_dir(RLY_3, GPIO_OUT);
    gpio_put(RLY_3, RLY_OFF);
    gpio_init(RLY_4);                   // Initialise GPIO - Relay 4 is connected to GPIO PIN 4
    gpio_set_dir(RLY_4, GPIO_OUT);
    gpio_put(RLY_4, RLY_OFF);
    gpio_init(RLY_5);                   // Initialise GPIO - Relay 5 is connected to GPIO PIN 5
    gpio_set_dir(RLY_5, GPIO_OUT);
    gpio_put(RLY_5, RLY_OFF);
    gpio_init(RLY_6);                   // Initialise GPIO - Relay 6 is connected to GPIO PIN 6
    gpio_set_dir(RLY_6, GPIO_OUT);
    gpio_put(RLY_6, RLY_OFF);
    gpio_init(RLY_7);                   // Initialise GPIO - Relay 7 is connected to GPIO PIN 7
    gpio_set_dir(RLY_7, GPIO_OUT);
    gpio_put(RLY_7, RLY_OFF);
     
    cmdInitIf();                        // Initialise Command Interface
    
     // Infinite Loop
    while (1){
        cmdIfStateMachine();
        }
}

