#include <stdio.h>
#include "../wolf.h"

void gpio_set(int bit) {
    /*printf("Setting bit to %d\n", bit);*/
}

static bool tgpio = false;

int gpio_get(void) {
    return tgpio;
}

void sleep_ns(int tmo) {
    //Nehh, Wolfs do not sleep, but they should remember that the line was low/high
    switch(tmo) {
        case T0H:
            printf("\trabi 0\n");
            break;
        case T1H:
            printf("\trabi 1\n");
    }
}

bool SOME_INPUT[K] = {false};

extern void statemachine(int);

void output(bool bit)
{
    printf("Akela %d\n", bit);
    statemachine(bit);
}

void test_no_neighbours()
{
    printf("NO NEIGHBOUR TEST\n");
    output(GROWL);
    output(HOWL);
}
void test_one_neighbour()
{
    printf("ONE NEIGHBOUR TEST\n");
    output(GROWL);
    output(BARK);
    for (int k = 0; k<K; k++) output(1);
    output(HOWL);
}

int main(int argc, char **argv)
{
    test_no_neighbours();
    test_one_neighbour();
    test_one_neighbour();

    /* Basic test: Toggle the input and see what happens. */
    /*for(int i=0;i<2*K;i++) {*/
        /*tgpio = !tgpio;*/
        /*statemachine(tgpio);*/
    /*}*/
}
