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


### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
