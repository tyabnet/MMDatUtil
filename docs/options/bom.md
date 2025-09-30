# -bom

Include a Byte Order Marker (BOM) in the output file. By default a BOM is included in the output 
map. Since this is the default, this is just provided for compatibility with earlier version of this program.

This switch is the opposite of `-nobom`. Both are not allowed at the same time.

Example:
```
MMDatUtil -srcmap "source.DAT" -outmap "destination.DAT" -copysrc -bom
```
source.DAT will be written as destination.DAT using UTF8 BOM format.

### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
