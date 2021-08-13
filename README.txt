Terminal 6502 simulator

Work in progress 6502 cpu simulator, requires cc65 to be installed.

Download a release from https://github.com/threecreepio/t65/releases

Usage:
With t65 in your path, you can write:
```
t65 example.s
```

This will attempt to build example.s using ca65, then runs the result.

The program will run until a CLD, BRK or undocumented instruction is executed.
To end execution, press Ctrl-C.

Intallation:
Copy the files from the release to a location on your path.

If building from source, use cmake:
```
mkdir build
cd build
cmake ..
```

Then either `make` if available, or use visual studio.


