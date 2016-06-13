# ubirch#1 r0.2 board specific implementation

This is the ubirch#1 board revision 0.2. It has built in a single-color LED, an extra [Atmel ECC](http://www.atmel.com/products/security-ics/cryptoauthentication/ecc-256.aspx)
chip as well as a cell phone chip ([SIM800H/SIM800C](http://simcomm2m.com/En/module/detail.aspx?id=74)

* currently programmable via USB or JTAG (SWD, JLink Debug Probe)
* serial debug console is configured on PTB16 (RX) and PTB17 (TX)

## Flashing

Entering bootloader mode:
1. __Initial__ (necessary once): The board must be reset into bootloader mode. This is done by removing the USB/Power, then pressing
the button on-board (keep pressed) and re-inserting the USB cable. Now the board is in bootloader mode.
2. __Firmware__: After flashing using the latest firmware entering the bootloader mode is just a
matter of pressing the button once.

If you use the [cmake build toolchain](https://github.com/ubirch/ubirch-arm-toolchain), the `-flash` target for your
project will be configured to use the `blhost` tool coming with the toolchain to flash directly via USB.

The commands it executes are:

- `blhost -u -- flash-erase-all`
- `blhost -u -- write-memory 0x0 target-executable.bin`
- `blhost -u -- reset`

These commands, erase flash, write the binary and reset the board.

## Connectors

Debug console output can be found on pins 6 (RX) and 7 (TX). If the SIM800H chip is on the bottom right
corner of the board, the pins are as follows:

```
2  4  6  8  10 12 14 16 18 20 22 24 26
o  o  o  o  o  o  o  o  o  o  o  o  o
⚀  o  o  o  o  o  o  o  o  o  o  o  o
1  3  5  7  9  11 13 15 17 19 21 23 25
```

The pin configuration is as follows:

```
 1 = A1         PTB2   |  2 = A2          PTB3
 3 = D1         PTB9   |  4 = I2C_SCL     PTB10
 5 = I2C_SDA    PTB11  |  6 = UART_RX     PTB16
 7 = UART_TX    PTB17  |  8 = T1          PTB18
 9 = GND               | 10 = switchable  VDD
11 = T2         PTB19  | 12 = SPI2_CS0    PTB20
13 = SPI2_SCK   PTB21  | 14 = SPI2_MOSI   PTB22
15 = SPI2_MISO  PTB23  | 16 = UART2_RTS   PTC1
17 = SPI0_CS4   PTC0   | 18 = UART2_CTS   PTC2
19 = UART2_RX   PTC3   | 20 = UART2_TX    PTC4
21 = SPI0_SCK   PTC5   | 22 = SPI0_MOSI   PTC6
23 = SPI0_MISO  PTC7   | 24 = permanent   VDD
25 = GND               | 26 = switchable  VDD
```

* external I2C is the I2C2
* internal connection to the ATECC chip is I2C0

![ubirch#1 r0.2 front](ubirch1r02-front.jpg)
![ubirch#1 r0.2 back](ubirch1r02-back.jpg)

# R.I.P.

Here is a list of devices that died in the process of constructing the firmware:

1. ubirch#1 r0.2 `(01)` - died of a wrongly attached battery. Apparently the battery socket has some direction
   and not checking that it is attached the right way before attaching the battery while having USB attached
   as well kills the power management unit. I should have taken a look at the marks on the board before
   attaching. <br/>
   *What I learned: Do not expect that the board manufacturer always does it right :-)*

2. ubirch#1 r0.2 `(02)` - died of a power surge due to too many power eating LEDs. The MCU's pins can just take
   a certain amount of current. Attaching too many of them (in line) kills the MCU. This one almost died of
   cause #1 when I tried to attach a battery which had it's power lines wrongly connected.<br/>
   *What I learned: Read the specs and don't just mindlessly attach stuff to pins :-)*

3. ubirch#1 r0.2 `(03)` - died of potential Potential difference. Having the UART connected to a different
   host is not a good idea, esp. not connecting `GND`. In this case, the MCU heated up quite a bit :(<br/>
   *What I learned: Learn some more about electronics and how current flows. Always at least connect ground
   to level out the potential.*
