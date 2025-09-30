# -unicode

Alias for `-utf16LE`. Output map will have UTF16LE output encoding.

By default, all UTF output formats will include a BOM unless you use the `-nobom` option.

Example:
```
MMDatUtil -srcmap "source.DAT" -outmap "destination.DAT" -copysrc -unicode -bom
```
source.DAT will be written as destination.DAT using UTF16LE format with a BOM. 

### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
