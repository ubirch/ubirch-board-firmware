# ubirch board firmware

This repository contains the firmware implementation for our own board. The toolchain
is built in a way so it may also run on other boards, like the [FRDM-K82F](http://www.nxp.com/products/software-and-tools/run-time-software/kinetis-software-and-tools/ides-for-kinetis-mcus/freescale-freedom-development-platform-for-kinetis-k82-k81-and-k80-mcus:FRDM-K82F)
or the [FRDM-KL82Z](http://www.nxp.com/products/software-and-tools/hardware-development-tools/freedom-development-boards/freedom-development-board-for-kinetis-ultra-low-power-kl82-mcus:FRDM-KL82Z?fsrch=1&sr=1&pageNum=1)*.
You will have to select the specific board when running ```cmake``` (see below).

See the board specific, including pin settings found here:

* [ubirch#1 r0.3](board/ubirch1r03) (and [r0.3-k](board/ubirch1r03k01))
* [ubirch#1 r0.2](board/ubirch1r02)
* [FRDM-K82F](board/frdm_k82f)
* [FRDM-KL82Z](board/frdm_kl82z)

## Testing

The firmware project contains some tests that check that the required implementation
for the different boards is working as expected. Check the directory [test](test) and flash
the `test_firmware` target.

Test output will be displayed on the serial debug console and should look like this:

```
Testing Board and Firmware: FRDM-KL82Z

- I2C: 0x1c: unknown device detected
I2C            : OK
TIMER          : OK
RTC            : OK
Test finished.
```

The default is that tests display `TEST: OK` or, if failed, just `ASSERT ERROR` with some
additional information what went wrong. Informational messages are prefixed with a `-`.

> Please allow for a little time as some tests require waiting for events to happen. An example is
> the RTC test, that schedules an interrupt 2s in the future and waits for
> three seconds to check it has happened.

## License

If not otherwise noted in the individual files, the code in this repository is

__Copyright &copy; 2016 [ubirch](http://ubirch.com) GmbH, Author: Matthias L. Jugel__

```
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
