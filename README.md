# pebble-graph
`pebble-graph` is a minimal graphing library for the pebble watch

![Bar Graph](img/bar.png)
![Line Graph](img/line.png)

## Quick start

Copy and paste both `pebble-graph.h` and `pebble-graph.c` into your project, then `#include "pebble-graph.h"` as you would any other header

Project Examples are in `examples/`.
```
cd examples/line
pebble build
pebble install --emulator basalt
```
## Known issues

- This project was not built for round devices, so information will be hidden on those devices
- Devices that don't support colors could hide information if light colors are chosen on the graph