# FRDM-KL82Z board specific implementation

This is the abstraction for the [FRDM-KL82Z](http://www.nxp.com/products/software-and-tools/hardware-development-tools/freedom-development-boards/freedom-development-board-for-kinetis-ultra-low-power-kl82-mcus:FRDM-KL82Z)
board. It has several peripherals including an RGB LED which is controlled using three different GPIO pins.

- programmed via USB (attached disk), just copy the bin to it
- serial debug console is also available via USB

> If you want to debug using SWD but use the USB serial,
> set both jumpers ```J6``` and ```J7``` to off.

## Board Pins

Please see page 2 of the [Quickstart Guide](http://cache.nxp.com/files/32bit/doc/quick_start_guide/FRDM-KL82Z-QSG.pdf) for a description of the pins location.

- `I2C`
   - `SCL` - `PTB0`
   - `SDA` - `PTB1`
- Cellphone UART connection
   - `PWRKEY` - `PTC5`
   - `Status` - `PTC7`
   - `Reset` - `PTC6`
   - `RX` - `PTC3`
   - `TX` - `PTC4`

![FRDM-KL82Z](http://cache.nxp.com/files/graphic/block_diagram/FRDM-KL82Z-BD.jpg)
 Board Photo &copy; [NXP](http://www.nxp.com)
