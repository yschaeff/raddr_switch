/**
 * Reverse Addressable Binary Input
 * Master implementation: "Akela"
 **/

#include "wolf.h"

#define output PIN1
#define input PIN2

#define W 100
int rabies_A[W];
int rabies_B[W];
int *rabies[W]; //points to A or B

/* Statemachine state */
int state;
int rabi_i;
int bark_i;

void reset_statemachine()
{
    state = S_REST;
    rabi_i = 0;
    bark_i = K; //in case we go out of sync reset this too
}

void statemachine(int bit)
{
    switch (state) {
        case S_REST:
            if (bit == GROWL) { //growl
                state = S_ALERT;
            }
            break;
        case S_ALERT:
            state = bit?S_HOWL:S_BARK;
            (*rabies)[rabi_i] = 0;
            break;
        case S_BARK:
            (*rabies)[rabi_i] << 1;
            (*rabies)[rabi_i] |= bit;

            if (!--bark_i) {
                state = S_ALERT;
                bark_i = K;
                rabi_i++;
            }
            break;
        case S_HOWL: //stay put so main can check state
            break;
    }
}

void interrupt_rising()
{
    sleep_ns((T0H+T1H)/2);
    bit = read(input);
    statemachine(bit);
}

void wakeup_pack()
{
    bark_ful(GROWL);
    bark_ful(HOWL);
}

void flip()
{
    rabies = (rabies == &rabies_A)? &rabies_B : &rabies_A;
}

void report(packsize)
{
    int *prev[W] = (rabies == &rabies_A)? &rabies_B : &rabies_A;
    int *curr[W] = rabies;

    //No need to go to W, packsize is the actual size of the pack
    for (int w = 0; w<packsize; w++) {
        if ((*prev)[w] != (*curr)[w]) {
            //event!
        }
    }
}

void main()
{
    //TODO set input to INPUT, attach interrupt rising edge

    rabies = &rabies_A;

    while (1) {
        reset_statemachine();
        wakeup_pack();
        sleep_ms(1);
        if (state == S_HOWL)
            report(rabi_i)
            flip()
        else {
            //help! out of sync?
        }
        sleep_ms(9);
    }
}
