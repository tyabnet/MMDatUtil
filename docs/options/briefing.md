# -briefing

Parameter: `filename`

Replace the briefing section of the output map with the file contents. If the filename is an empty file, the briefing section will also be empty.

This file should not have the opening closing braces.

Example:

Contents of MyBriefing.txt
```
CADET this is going to be a hard mission. Planet tremors and lava flows have sealed off a base and moving it to an unknown location near a lake of lava. You need to transport down and find that base. Once you find the base, you need to collect 35 energy crystals.
```
Command line:
```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -briefing "MyBriefing.txt" -copysrc
```
The contents of source.dat are copied to destination.dat with the contents of the file MyBriefing.txt replacing the text in the briefing section.


### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
