# -bom

No parameter.

Include a Byte Order Marker (BOM) in the output file. By default a BOM is included in the output map for all UTF formats. Since this is the default, this is just provided for compatibility with earlier version of this program.

This switch is the opposite of `-nobom`. Both are not allowed at the same time.

Example:
```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -bom
```
source.dat will be written as destination.dat using UTF8 BOM format.

### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
