# -srcansi7

Input files are checked for any non-ANSI characters and a warning is generated. This is done regardless of the input encoding.

Some editors such as MSWord will inject extended ANSI characters automatically as part of their auto assist features. For example forward and backwards double quotes, extended dashes, symbols and other characters that sometimes appear similar to the ANSI versions. Using these can lead to incorrect display depending on the users code page language.

Setting this option is a way to detect these characters. For each input line the first extended character is displayed as a warning.

If your goal is to have a 100% standard ANSI map, this setting will ensure that you are aware that some data in your output map contains extended ANSI data giving you the opportunity to correct the input files.

>If all of your input files are UTF on purpose and you want to use symbols and non-western ANSI characters then do not use this option, you may have many warnings.

Example:

```
MMDatUtil.exe -srcmap "source.dat" -outmap "destination.dat" -briefing "MyBriefing.txt" -success "MySuccess.txt" -script "MyScript.txt" -copysrc -srcansi -srcansi7
```

All input files are checked for any character over the value of 127 (extended ANSI) and warnings are displayed. The output map will still be created.

### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
