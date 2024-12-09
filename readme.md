# LAF (List of Ascii Frames)
LAF is a simple format for storing a sequence of ASCII frames.  
That you can reproduce with `laf.exe`

## Future features
- [X] Easy
- [ ] Add support for gif convertion into LAF file.
- [ ] Turn format into byte code.
- [ ] COLORS!!!.

## Header
Images, videos or even gifs or bitmaps has it's own header file.  
This in order to detect which file are you dealing with.  

### Format
`LAFWWWHHHFF|` where:
- WWW is the width of each frame.
- HHH is the height of each frame.
- FF is the framerate (if 0, 4)
- | is the end of the header.
Values are represented in hexadecimal.

### Flags
Flags are options that can be used to customize the behaviour of the LAF file.  
This flags goes between frame rate value and end of the header.  
- `F` : when reproducing a LAF the console switches into fullscreen.
- `R` : the LAF will repeat backwards after last frame.

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