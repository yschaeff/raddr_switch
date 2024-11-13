#include <stdio.h>
#include "../wolf.h"

void gpio_set(int bit) {
    printf("Setting bit to %d\n", bit);
}

static bool tgpio = false;

int gpio_get(void) {
    return tgpio;
}

void sleep_ns(int tmo) {
    //Nehh, Wolfs do not sleep, but they should remember that the line was low/high
    printf("Sleeping for %d\n", tmo);
}

bool SOME_INPUT[K] = {false};

extern void statemachine(int);

int main(int argc, char **argv)
{
    /* Basic test: Toggle the input and see what happens. */
    for(int i=0;i<2*K;i++) {
        tgpio = !tgpio;
        statemachine(tgpio);
    }
}
