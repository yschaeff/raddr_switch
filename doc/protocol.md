# Reverse Addressable Binary Input - Protocol specification

I'm entirely unsure what reasonable timings are for our chip. Therefore I will
just grab the WS2812B specs and initially based it on that.

## Timings

Data transfer time (TH+TL = 1.25us ±600ns):

|   Symbol   |          Description          |     Timing       |
| ---------- | ----------------------------- | ---------------- |
|    T0L     |   0 code,  low voltage time   |   0.85us ±150ns  |
|    T0H     |   0 code, high voltage time   |   0.4us  ±150ns  |
|    T1L     |   1 code,  low voltage time   |   0.45us ±150ns  |
|    T1H     |   1 code, high voltage time   |   0.8us  ±150ns  |
|    RES     |   low voltage time            |   > 50us         |

Sequence chart:

```
    +-----+         +
    | T0H |   T0L   |       Send 0 bit
    +     +---------+

    +---------+     +
    |   T1H   | T1L |       Send 1 bit
    +         +-----+

    +               +
    |      RES      |       Time between reads.
    +---------------+
```

## Cascade method

I think it is conceivable to use only one pin on the MCU. That might depend
on how fast the chip is able to switch from input to output. Individual RABI's
connected together serially will for a 'pack'.


```
    +---------------------------------------+
    |                                       |
    |   +-----+   +-----+   +-----+         |   +-----+
    +---|Di Do|---|Di Do|---|Di Do|----->|--+---|io   |
        |     |   |     |   |     |  (diode)    |     |
        |RABI0|   |RABI2|   |RABI2|             | MCU |
        +-----+   +-----+   +-----+             +-----+
```

I recon this setup would be easier:

```
    +----------------------------------------------------+
    |                                                    |
    |   +-----+   +-----+   +-----+             +-----+  |
    +---|Di Do|---|Di Do|---|Di Do|-------------|Di Do|--+
        |     |   |     |   |     |             |     |
        |RABI0|   |RABI2|   |RABI2|             | MCU |
        +-----+   +-----+   +-----+             +-----+
```

## Data description

At REST each RABI in the pack will listen for the signal from the pack leader
ALPHA, a role played by our MCU. This signal is called a GROWL. When hearing a
GROWL a RABI will wake up and be on ALERT.

While on ALERT, when HAULED at a RABI will produce a HOWL itself causing the
next RABI to HOWL (if any left). Otherwise it will copy the HAULS of its
neighbours by a series of BARKS.

To trigger the rest of the pack ALPHA will emit a GROWL to the highest ranked
RABI (RABI0) followed by a HOWL. Then, ALPHA will listen to the lowest ranked
RABI (RABIw) which will BARK all HAULS from the higher ranked RABIES and
finally HAULS itself.

The amount of BARKS per HOWL is 1+K (HOWL + BARKS needed to represent the
data). K=1 for just one switch. But we could decide to bump up K so we can
support more complex inputs like rotary encoders or analog devices. The total
number of BARKS (bits) needed per cycle is:

    1(growl) + W*( 1(hual) + K(bark) )
    Where W is the size of the pack and K is the data size

Lets assume for now K=8. So for 80 keys we need to send 721 bits. Which should
take 50us + 721 * 1.25us + = 951us. So around 1K updates per second.

NOTE: If we would assume W=101 and K=32 we would have an update frequency of
237Hz which still is way faster than what we need. Might be nice for future
proving / over engineering?

Choosing 8 data bits will have us send 9 bits per switch. Giving us an excuse
to dub this the K-NINE protocol. =)

## State Machine

If we would represent the above as a state machine we would get the following:

```
      +---+
      |   |
      +->REST<----------+
          |             |
   (growl)|             |
          |             |
          v    (howl)   |
     +->ALERT-------->HOWL
     |    |
     |    | (bark)
     |    v
     |  BARK_1
     |    v (bark)
     |   ...
     |    v (bark)
     +--BARK_K
```

```
- REST:
    Wait for RAISE, GROWL=read(at 600ns).
    if GROWL
        goto ALERT.
    else:
        # noise/error
        goto REST

- ALERT:
    write(GROWL)
    Wait for RAISE, HOWL=read(at 600ns).
    if HOWL:
        write(not HOWL)
        for k=1..K:
            write(data[k])
        write(HOWL)
        goto REST
    else:
        write(HOWL)
        goto BARK_1

- BARK_1:
    Wait for RAISE, BARK=read(at 600ns).
    write(BARK)
    goto BARK_2
...
- BARK_K:
    Wait for RAISE, BARK=read(at 600ns).
    write(BARK)
    goto ALERT

```

So a cycle will look like this.

Send by ALPHA:

```
    [GROWL|HOWL]
```

Recieved by ALPHA (W=3)

```
    [GROWL|!HOWL|BARK|BARK|BARK|BARK|BARK|BARK|BARK|BARK|!HOWL|BARK|BARK|BARK|BARK|BARK|BARK|BARK|BARK|!HOWL|BARK|BARK|BARK|BARK|BARK|BARK|BARK|BARK|HOWL]
```

or:

```
              RABI 0              RABI 1              RABI 2
        _________________   _________________   _________________
    [1| 0|x|x|x|x|x|x|x|x | 0|x|x|x|x|x|x|x|x | 0|x|x|x|x|x|x|x|x | 1]
```
