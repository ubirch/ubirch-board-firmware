# FRDM-K82F board specific implementation

This is the abstraction for the [FRDM-K82F](http://www.nxp.com/products/software-and-tools/run-time-software/kinetis-software-and-tools/ides-for-kinetis-mcus/freescale-freedom-development-platform-for-kinetis-k82-k81-and-k80-mcus:FRDM-K82F)
board. It has several peripherals including an RGB LED which is controlled using three different GPIO pins.

- programmed via USB (attached disk), just copy the bin to it
- serial debug console is also available via USB

> If you want to debug using SWD but use the USB serial,
> set both jumpers ```J6``` and ```J7``` to off.

![FRDM-K82F](http://cache.nxp.com/files/graphic/block_diagram/FRDM-K82-800x480-BD.jpg)
