# -srcansi

Treat 8 bit files without a BOM as Windows current code page format. This applies to all input files: Maps, Scripts, and Briefings.

This is the default format for all input files that autodetect to be 8 byte and do not have a BOM and is provided for compatibility only.

Example:

```
MMDatUtil.exe -srcmap "source.dat" -outmap "destination.dat" -briefing "MyBriefing.txt" -copysrc -srcansi
```

If the input map source.dat does not have a BOM and it autodetects to be 8 bit input will be treated as Windows current code page. This option also applies to MyBriefing.txt.  The output map destination.dat will be generated from source.dat combined with the briefing text from MyBriefing.txt


### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
