# -defore

Parameter: `integer`

Default ore value for newly created tiles during merging or resizing. The default value is 0.  This modifies the map resource ore: section.

Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -resizecol 54 -resizerow 56 -defore 1
```
source.dat is rewritten as destination.dat and it is resized to be 54 tiles wide by 56 tiles tall. If this is larger than the size of source.dat, then the newly created tiles will each have one ore. These ore values are in the resource ore: section

### Links
- [Back to Overview](../README.md)
