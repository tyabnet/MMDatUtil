# -utf8

The output map will use UTF8 encoded. If no other output encoding option is provided, UTF8 is used by default.

By default, all UTF output formats will include a BOM unless you use the `-nobom` option.

Example:

```
MMDatUtil -srcmap "source.DAT" -outmap "destination.DAT" -copysrc -utf8
MMDatUtil -srcmap "source.DAT" -outmap "destination.DAT" -copysrc
```
source.DAT is rewritten as destination.DAT using UTF8 with BOM format.
Both of the above are identical, by default -utf8 is assumed if no other output format is used.

### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
