# FRDM-KL82Z board specific implementation

This is the abstraction for the [FRDM-KL82Z](http://www.nxp.com/products/software-and-tools/hardware-development-tools/freedom-development-boards/freedom-development-board-for-kinetis-ultra-low-power-kl82-mcus:FRDM-KL82Z)
board. It has several peripherals including an RGB LED which is controlled using three different GPIO pins.

- programmed via USB (attached disk), just copy the bin to it
- serial debug console is also available via USB

> If you want to debug using SWD but use the USB serial,
> set both jumpers ```J6``` and ```J7``` to off.

![FRDM-KL82Z](http://cache.nxp.com/files/graphic/block_diagram/FRDM-KL82Z-BD.jpg)
