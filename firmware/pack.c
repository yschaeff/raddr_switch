/**
 * Reverse Addressable Binary Input
 * Slave implementation: "The Pack"
 **/

#include "wolf.h"

#define output PIN1
#define input PIN2

void statemachine(int bit):
{
    static int state = REST;
    static int bark_i = K;

    switch (state) {
        case REST:
            if (bit) { //growl
                state = ALERT;
                bark(1, output); //wake up next with growl
            }
            break;
        case ALERT:
            state = bit?HOWL:BARK;
            bark(0, output); //Yelp, so next will copy next frame
            break;
        case HOWL:
            for (int k=0; k<K, k++) {
                bark_full(SOME_INPUT..., input);
                sleep_ns(bit?T0L:T1L);//wait low time of bit
            }
            bark(1, output); //Howl, so next will also go to HOWL
            state = REST; //we are the last. Get some rest.
            break;
        case BARK:
            bark(bit, output); //Copy input to output
            if (!--bark_i) {
                state = ALERT;
                bark_i = K;
            }
            break;
    }
}

void interrupt_rising()
{
    sleep_ns((T0H+T1H)/2);
    bit = read(input);
    statemachine(bit);
}
