# -ansi

No parameter.

Forces the output map to be encoded as windows current code page.  This option also implies -nobom.

Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -ansi
```
source.dat is rewritten as destination.dat using windows current code page format.

> If your input files have Unicode characters - those characters that are not a part of the Windows current code page will be lost and converted into the default unknown character for that code page. Usually that is the "?"  character.

>This is one of the two formats that Manic Miners will save maps as. The other is UTF16LE BOM.

### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
