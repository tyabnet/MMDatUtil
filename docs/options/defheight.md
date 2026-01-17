# -defheight

Parameter: `integer`

Default height value for newly created tiles during merging or resizing. The default value is 0.  This modifies the map resource heights section.

Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -resizecol 54 -resizerow 56 -defheight 100
```
source.dat is rewritten as destination.dat and it is resized to be 54 tiles wide by 56 tiles tall. If this is larger than the size of source.dat, then the newly created tiles will have every corner set to a height of 100. These height values are in the heights section.

This will also modify any newly created border heights. You may also use `-borderheight 0` to reset all border heights back to 0.

### Links
- [Back to Overview](../README.md)
