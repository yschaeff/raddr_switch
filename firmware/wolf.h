#ifndef WOLF_H
#define WOLF_H

#include <stdbool.h>

#define T0L 850
#define T0H 400
#define T1L 450
#define T1H 800

#define K 8

#define GROWL 1

#define BARK 0
#define HOWL 1

enum states {
    S_REST,       //do nothing
    S_ALERT,      //listen for howl
    S_HOWL,       //transmit frame
    S_BARK        //copy frame
};

extern void gpio_set(int);
extern void sleep_ns(int);
extern int gpio_get(void);
extern bool SOME_INPUT[K];

static inline void bark(int bit)
{
    gpio_set(1);
    sleep_ns(bit?T1H:T0H);
    gpio_set(0);
}

static inline void bark_full(int bit)
{
    bark(bit);
    sleep_ns(bit?T0L:T1L);//wait low time of bit
}

#endif
