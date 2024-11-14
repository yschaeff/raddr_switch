/**
 * Reverse Addressable Binary Input
 * Slave implementation: "The Pack"
 **/

#include <stdio.h>
#include "wolf.h"

#define DBG 0

void statemachine(int bit)
{
    static int state = S_REST;
    static int bark_i = K;

    switch (state) {
        case S_REST:
            if (bit == GROWL) { //growl
                if (DBG) printf("received growl going in ALERT\n");
                state = S_ALERT;
                bark(GROWL); //wake up next with growl
            } else {
                if (DBG) printf("whut? staying in rest\n");
            }
            break;
        case S_ALERT:
            if (DBG) printf("in alert\n");
            bark(BARK); //Yelp, so next will copy next frame
            if (bit) {
                if (DBG) printf("howling my state\n");
                for (int k=0; k<K; k++) {
                    bark_full(SOME_INPUT[k]);
                }
                //Maybe include parity bit?
                bark_full(GROWL); //wake up next with growl
                bark(HOWL); //Howl, so next will also go to S_HOWL
                state = S_REST; //we are the last. Get some rest.
                if (DBG) printf("going to REST\n");
            } else {
                if (DBG) printf("going to BARK\n");
                state = S_BARK; //
            }
            break;
        case S_BARK:
            bark(bit); //Copy input to output
            if (DBG) printf("BARK %d!\n", bark_i);
            if (!--bark_i) {
                state = S_ALERT;
                if (DBG) printf("going to REST\n");
                bark_i = K;
                //maybe check parity? Go to S_REST on parity fail?
            }
            break;
    }
}

void interrupt_rising()
{
    sleep_ns((T0H+T1H)/2);
    int bit = gpio_get();
    statemachine(bit);
}
