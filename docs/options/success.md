# -success

Parameter: `filename`

Replace the briefingsuccess section of the output map with the file contents. If the filename is an empty file, the briefingsuccess section will also be empty.

This file should not have the opening closing braces.

Example:

Contents of MySuccess.txt
```
CADET - you did it!. You found the missing base and recovered 35 energy crystals. Well done!

Prepare for your next away mission!
```
Command line:
```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -success "MySuccess.txt" -copysrc
```
The contents of source.dat are copied to destination.dat with the contents of the file MySuccess.txt replacing the text in the briefingsuccess section.

The encoding of the success file is automatically detected. If it has 8 bit encoding and no BOM, it is treated as Windows current code page unless changed by the `-srcutf8` option.

The keyword `Cadet` will be replaced by the game engine with your player name.


### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
