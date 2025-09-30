# Options
Command line options may begin with any of the following:
- Single dash `-`
- Double dash `--`
- Forward slash `/`

After every option at least one blank space is needed. Some options have parameters, but after a parameter there needs to be a blank space before the next option.

The order of options on the command line does not matter.

| Option | Parameter | Description |
|--------|-----------|-------------|
| [ -ansi ](options/ansi.md) |  | Output format is 8 bit windows current code page |
| [ -backup ](options/backup.md) |  |Backup the output file prior to overwriting it |
| [ -bom ](options/bom.md) |  |Include BOM with UTF output. This is the default. |
| [ -borderheight ](options/borderheight.md) | integer | Set default height value for new border tiles. Default is 0. |
| [ -briefing ](options/briefing.md) | filename | Contents of file replace briefing section. |
| [ -copysrc ](options/copysrc.md) |  | Output map will have the contents of the -srcmap. |
| [ -creator ](options/creator.md) | string | Set the map creator in the info section. |
| [ -deftile ](options/deftile.md) | integer | Provide default tile for newly created tiles. Default is ground (1). |
| [ -defheight ](options/defheight.md) | integer | Set default height value for new tiles. Default is 0. |
| [ -defcrystal ](options/defcrystal.md) | integer | Set default number of crystals for new tiles. Default is 0. |
| [ -defore ](options/defore.md) | integer | Set default number of ore for new tiles. Default is 0. |
| [ -flattenabove ](options/flattenabove.md) | values | Change heights larger than high to given value. |
| [ -flattenbelow ](options/flattenbelow.md) | values | Change heights smaller than low to given value. |
| [ -flattenbetween ](options/flattenbetween.md) | values | Change heights between low and high to given value. |
| [ -fix ](options/fix.md) |  | Fix incorrect source map heights, tiles, crystals, ore. |
| [ -help ](options/help.md) |  | Display help text |
| [ -mapname ](options/mapname.md) | string | Set the map name in the info section. |
| [ -mergecrystal ](options/mergecrystal.md)|  | Merge source map crystals with existing output map. |
| [ -mergeheight ](options/mergeheight.md) |  | Merge source map heights with existing output map. |
| [ -mergerect ](options/mergerect.md) | values | Define region in source map to merge into existing output map. |
| [ -mergetile ](options/mergetile.md) |  | Merge source map tiles with existing output map |
| [ -mergeore ](options/mergeore.md) |  | Merge source map ore with existing output map. |
| [ -nobom ](options/nobom.md) |  | Output UTF encoding will not have a BOM. |
| [ -outmap ](options/outmap.md) | filename | Output map filename. |
| [ -offsetcol ](options/offsetcol.md) | integer | defines offset width for merge operations. |
| [ -offsetrow ](options/offsetrow.md) | integer | defines offset height for merge operations. |
| [ -overwrite ](options/overwrite.md) |  | Allows output to be overwritten if it exists. |
| [ -resizerow ](options/resizerow.md) | integer | Resize source map height. |
| [ -resizecol ](options/resizecol.md) | integer | Resize source map width. |
| [ -script ](options/script.md) | filename | Contents of filename replaces script section. Enables script processing. |
| [ -sdatefmt ](options/sdatefmt.md) | text | String defining format for embedded dates in script files. |
| [ -sdefine ](options/sdefine.md) | macro | key=value macro for use in script files |
| [ -sfixspace ](options/sfixspace.md) |  | Invalid spaces in script automatically removed |
| [ -sincdirs ](options/sincdirs.md) | paths | semi-colon separated list of paths to search for script files. |
| [ -snocomment ](options/snocomment.md) |  | Output script will have most comments removed. |
| [ -soptnames ](options/soptnames.md) |  | Output script will have most event chain and variable names optimized. |
| [ -soptblank ](options/soptblank.md) |  | Output script will have non-essential blank lines removed. |
| [ -srcansi ](options/srcansi.md) |  | 8 bit input files without BOM are treated as Windows current code page. Default. |
| [ -srcmap ](options/srcmap.md) | filename | Source map to process. |
| [ -srcutf8 ](options/srcutf8.md) |  | 8 bit input files without BOM are treated as UTF8. |
| [ -success ](options/success.md) | filename | Contents of file replace briefingsuccess section. |
| [ -unicode ](options/unicode.md) |  | Output encoding is UTF16LE. |
| [ -utf16BE ](options/utf16be.md)  |  | Output encoding is UTF16BE. |
| [ -utf16LE ](options/utf16le.md) |  | Output encoding is UTF16LE. Same as -unicode. |
| [ -utf8 ](options/utf8.md) |  | Output format is UTF8. This is the default. |
| [ -utf32BE ](options/utf32be.md) |  | Output encoding is UTF32BE. |
| [ -utf32LE ](options/utf32le.md) |  | Output encoding is UTF32LE. |

### Links
- [Back to Overview](README.md)
