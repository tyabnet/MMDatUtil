# -flattenbetween

Parameters: `low,high,value`

`low`, `high`, `value` are signed integers.

Set heights in the inclusive range `[low, high]` to `value`.  This is applied to the entire map.

>-flattenrect will constrict changes to a region.

This option is useful to fix change an area of uneven terrain to be flat.

Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -flattenbetween -200,300,100
```
source.dat is rewritten as destination.dat. If there are heights in the map greater than or equal to -200 and less than or equal to 300, change them to 100. This is applied to the entire map.

> The map border heights are never changed by any flatten option.

### Links
- [Back to Overview](../README.md)
