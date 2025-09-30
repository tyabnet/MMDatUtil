# -utf16BE

output map will be written using UTF16 big-endian output encoding.

By default, all UTF output formats will include a BOM unless you use the `-nobom` option.

>Manic Miners cannot read UTF16BE files without a BOM.

Example:

```
MMDatUtil -srcmap "source.DAT" -outmap "destination.DAT" -copysrc -utf16BE
```
source.DAT is rewritten as destination.DAT using UTF16BE with BOM format.


### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
