#ifndef WOLF_H
#define WOLF_H

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

void bark(bit, output)
{
    write(output, 1);
    sleep_ns(bit?T1H:T0H);
    write(output, 0);
}

void bark_full(bit, output)
{
    bark(bitm output);
    sleep_ns(bit?T0L:T1L);//wait low time of bit
}

#endif
