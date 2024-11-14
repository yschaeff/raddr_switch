#include <stdio.h>
#include <assert.h>
#include "../wolf.h"

int EXPECT;

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
            printf("\trabi: 0\n");
            if (EXPECT >= 0) {
                assert(EXPECT == 0);
                EXPECT = -1;
            }
            break;
        case T1H:
            printf("\trabi: 1\n");
            if (EXPECT >= 0) {
                assert(EXPECT == 1);
                EXPECT = -1;
            }
    }
}

bool SOME_INPUT[K] = {true, true};

extern void statemachine(int);

void output(bool bit, int answer)
{
    printf("Neighbor: %d\n", bit);
    EXPECT = answer;
    statemachine(bit);
}

void test_n_neighbours(int n)
{
    printf("%d NEIGHBOUR TEST\n", n);
    output(GROWL, GROWL);
    while (n--) {
        printf("NEIGHBOUR -%d BARK\n", n);
        output(BARK, BARK);
        for (int k = 0; k<K; k++) {
            output(1, 1);
        }
    }
    printf("HOWL\n");
    output(HOWL, BARK);
}

int main(int argc, char **argv)
{
    test_n_neighbours(0);
    test_n_neighbours(1);
    test_n_neighbours(1);
    test_n_neighbours(4);
}
