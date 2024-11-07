/**
 * Reverse Addressable Binary Input
 * Slave implementation: "The Pack"
 **/

#include "wolf.h"

#define output PIN1
#define input PIN2



void statemachine(int bit)
{
    static int state = S_REST;
    static int bark_i = K;

    switch (state) {
        case S_REST:
            if (bit == GROWL) { //growl
                state = S_ALERT;
                bark(GROWL, output); //wake up next with growl
            }
            break;
        case S_ALERT:
            state = bit?S_HOWL:S_BARK;
            bark(BARK, output); //Yelp, so next will copy next frame
            break;
        case S_BARK:
            bark(bit, output); //Copy input to output
            if (!--bark_i) {
                state = S_ALERT;
                bark_i = K;
                //maybe check parity? Go to S_REST on parity fail?
            }
            break;
        case S_HOWL:
            for (int k=0; k<K, k++) {
                bark_full(SOME_INPUT[k], output);
                sleep_ns(bit?T0L:T1L);//wait low time of bit
            }
            //Maybe include parity bit?
            bark(HOWL, output); //Howl, so next will also go to S_HOWL
            state = S_REST; //we are the last. Get some rest.
            break;
    }
}

void interrupt_rising()
{
    sleep_ns((T0H+T1H)/2);
    bit = read(input);
    statemachine(bit);
}
