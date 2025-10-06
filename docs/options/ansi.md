# -ansi

Forces the output map to be encoded as windows current code page.  This option also implies -nobom.

Usage examples:

```
MMDatUtil -srcmap "source.DAT" -outmap "destination.DAT" -copysrc -ansi
```
source.DAT is rewritten as destination.DAT using windows current code page format.

> If your input files have Unicode characters - those will be lost saving in a non UTF format.

### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
