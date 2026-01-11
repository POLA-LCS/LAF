# LAF (List of Ascii Frames) v2.0.0

A simple `.laf` player files for Windows.

```laf
 __    _____ _____
|  |  |  _  |   __|
|  |__|     |   __|
|_____|__|__|__|
```

## File format

A LAF file is just a special format for storing ascii frames.
Images, videos, gifs or even bitmaps has it's own header to distinguish between them.
This in order to detect which file are you dealing with.

### Header

Numbers are represented in hexadecimal.

`LAFWWWHHHFF|` where:

- `LAF` the header tag saying "Heey, I'm a LAF file!"
- `WWW` is the width of each frame.
- `HHH` is the height of each frame.
- `FF` is the framerate (the default is 4 if 0 is provided).
- `|` is the end of the header.

### Flags

Flags are options that can be used to customize the behaviour of the LAF file.  
This flags goes before end of header and after framerate value.  
  
This flags are the default ones and can be changed by providing command line arguments (See `laf.exe --help`).  
  
- `F`: Sets the window console to fullscreen (maybe fails if using a special terminal).
- `R`: Repeats backwards after last frame.
- `S`: Stops when reached the end.

## Examples

Moving in

```
LAF00500207|
OOOOO
V|OOU

OOOOO
VO|OU

OOOOO
VOO|U

OOOOx
VOO/U

OOOxO
VOO|U

OOxOO
VO|OU

OxOOO
V|OOU

xOOOO
V\OOU
```

Note: Internally this is interpreted as `LAF00500207|OOOOOV|OOUOOOOOVO|OUOOOOOVOO|UOOOOxVOO/UOOOxOVOO|UOOxOOVO|OUOxOOOV|OOUxOOOOV\OOU`
