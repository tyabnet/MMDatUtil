# -failure

Parameter: `filename`

Replace the briefingfailure section of the output map with the file contents. If the filename is an empty file, the briefingfailure section will also be empty.

This file should not have the opening closing braces.

Example:

Contents of MyFailure.txt
```
CADET - you failed the mission. You did not find the missing base and collect the 35 energy crystals.

Review your mistakes and hopefully Chief will approve another attempt.
```
Command line:
```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -failure "MyFailure.txt" -copysrc
```
The contents of source.dat are copied to destination.dat with the contents of the file MyFailure.txt replacing the text in the briefingfailure section.

The encoding of the failure file is automatically detected. If it has 8 bit encoding and no BOM, it is treated as Windows current code page unless changed by the `-srcutf8` option.

The keyword `Cadet` will be replaced by the game engine with your player name.


### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
