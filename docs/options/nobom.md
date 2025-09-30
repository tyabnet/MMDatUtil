# -nobom

Do not include a Byte Order Marker (BOM) in the output file. By default a BOM is included in the output map.

This switch is the opposite of `-bom`. Both are not allowed at the same time.

MMDatUtil default output is UTF8 with BOM.

> Manic Miners cannot read UTF16 files without a BOM. UTF8 files without a BOM will be treated as Windows current code page 8 bit files.

Example:
```
MMDatUtil -srcmap "source.DAT" -outmap "destination.DAT" -copysrc -nobom -utf8
```
source.DAT will be written as destination.DAT using utf8 with no BOM. Manic Miners will treat this file as windows default code page output. Any unicode characters will be lost during output.

```
MMDatUtil -srcmap "source.DAT" -outmap "destination.DAT" -copysrc -nobom
```
source.DAT will be written as destination.DAT using windows current code page format. This form is identical to use the `-ansi` option




### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
