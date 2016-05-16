# ubirch #1 (r0.2) board firmware

This repository contains the firmware implementation for our own board. The toolchain
is built in a way so it may also run on other boards, like the [FRDM-K82F](http://www.nxp.com/products/software-and-tools/run-time-software/kinetis-software-and-tools/ides-for-kinetis-mcus/freescale-freedom-development-platform-for-kinetis-k82-k81-and-k80-mcus:FRDM-K82F)
or the [FRDM-KL82Z](http://www.nxp.com/products/software-and-tools/hardware-development-tools/freedom-development-boards/freedom-development-board-for-kinetis-ultra-low-power-kl82-mcus:FRDM-KL82Z?fsrch=1&sr=1&pageNum=1)*.
You will have to select the specific board when running ```cmake``` (see below).

See the board specific, including pin settings found here:

* [ubirch#1 r0.2](board/ubirch1r02)
* [FRDM-K82F](board/frdm_k82f)
* [FRDM-KL82Z](board/frdm_kl82z)\*

> \* Work in progress, the Kinetis SDK 2.0 is not available for the K82Z.

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
