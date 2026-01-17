# -briefing

Parameter: `filename`

Replace the briefing section of the output map with the file contents. If the filename is an empty file, the briefing section will also be empty.

This file should not have the opening closing braces.

Example:

Contents of MyBriefing.txt
```
Cadet this is going to be a hard mission. Planet tremors and lava flows sealed off a base and moved it to an unknown location near a lake of lava. You need to transport down and find that base. Once you find the base, you need to collect 35 energy crystals.
```
Command line:
```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -briefing "MyBriefing.txt" -copysrc
```
The contents of source.dat are copied to destination.dat with the contents of the file MyBriefing.txt replacing the text in the briefing section.

The encoding of the briefing file is automatically detected. If it has 8 bit encoding and no BOM, it is treated as Windows current code page unless changed by the `-srcutf8` option.

The keyword `Cadet` will be replaced by the game engine with your player name.


### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
