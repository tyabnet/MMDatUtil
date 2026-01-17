# -defcrystal

Parameter: `integer`

Default crystal value for newly created tiles during merging or resizing. The default value is 0.  This modifies the map resource crystals: section.

Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -resizecol 54 -resizerow 56 -defcrystal 1
```
source.dat is rewritten as destination.dat and it is resized to be 54 tiles wide by 56 tiles tall. If this is larger than the size of source.dat, then the newly created tiles will each have one crystal. These crystal values are in the resource crystals: section

### Links
- [Back to Overview](../README.md)
