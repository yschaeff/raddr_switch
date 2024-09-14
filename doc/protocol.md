# Reverse Addressable Binary Inputs (RABIES) - Protocol specification (CANINE)

This document describes the protocol called CANINE that's spoken between
ALPHA and (between) individual RABIES.

## Timings

I'm entirely unsure what reasonable timings are for our chip. Therefore I will
just grab the WS2812B specs and initially base it on that.

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
    |      RES      |       Time between cycles.
    +---------------+
```

## Cascade method

I think it is conceivable to use only one pin on the MCU. That might depend on
how fast the chip is able to switch from input to output. Individual RABIES
connected together serially will form a 'pack'. The RABI closed to the output
of ALPHA (RABI0) is considered the highest rank and messaged will be relayed
through lower ranked RABIES. The message from RABI0 will arrive first at ALPHA


```
    +---------------------------------------+
    |                                       |
    |   +-----+   +-----+   +-----+         |   +-----+
    +---|Di Do|---|Di Do|---|Di Do|----->|--+---|io   |
        |     |   |     |   |     |  (diode)    |     |
        |RABI0|   |RABI2|   |RABI2|             | MCU | (ALPHA)
        +-----+   +-----+   +-----+             +-----+
```

I recon this setup would be easier:

```
    +----------------------------------------------------+
    |                                                    |
    |   +-----+   +-----+   +-----+             +-----+  |
    +---|Di Do|---|Di Do|---|Di Do|-------------|Di Do|--+
        |     |   |     |   |     |             |     |
        |RABI0|   |RABI2|   |RABI2|             | MCU | (ALPHA)
        +-----+   +-----+   +-----+             +-----+
```

## Data description

At REST each RABI in the pack will listen for the signal from the pack leader
ALPHA, a role played by our MCU. This signal is called a GROWL. When hearing a
GROWL a RABI will wake up and be on ALERT.

While on ALERT, when HAULED at, a RABI will communicate its state and produce a
HOWL itself causing the next RABI to HOWL (if any left). Otherwise it will copy
the HAULS of its neighbours by a series of BARKS.

To trigger the rest of the pack, ALPHA will emit a GROWL to the highest ranked
RABI (RABI0) followed by a HOWL. Then, ALPHA will listen to the lowest ranked
RABI (RABIW) which will BARK all HAULS from the higher ranked RABIES and
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
to dub this the CANINE/K-NINE protocol. =)

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
    Wait for RAISE, IS_GROWL=read(at 600ns).
    if IS_GROWL
        write(1) # growl
        goto ALERT.

- ALERT:
    Wait for RAISE, IS_HOWL=read(at 600ns).
    write(0) ## not a howl
    if IS_HOWL:
        goto HOWL
    else:
        goto BARK_1

- HOWL:
    for k=1..K:
        BARK=data[k]
        write(BARK)
    write(1) # howl
    goto REST

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

Received by ALPHA (W=3)

```
    [GROWL|!HOWL|BARK|BARK|BARK|BARK|BARK|BARK|BARK|BARK|!HOWL|BARK|BARK|BARK|BARK|BARK|BARK|BARK|BARK|!HOWL|BARK|BARK|BARK|BARK|BARK|BARK|BARK|BARK|HOWL]
```

or:

```
              RABI 0              RABI 1              RABI 2
        _________________   _________________   _________________
    [1| 0|x|x|x|x|x|x|x|x | 0|x|x|x|x|x|x|x|x | 0|x|x|x|x|x|x|x|x | 1]
```
