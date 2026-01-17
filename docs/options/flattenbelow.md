# -flattenbelow

Parameters: `height,newheight`

`height` and `newheight` are signed integers.

Set heights less than or equal to `height` to `newheight`. This is applied to the entire map.

This option is the opposite of `-flattenabove`.

>`-flattenrect` will constrict changes to a region.

This option is useful to fix large holes in elevation to a given value. For example, if you have a large hole and want a flat area at the bottom of it.

Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -flattenbelow -1000,-800
```
source.dat is rewritten as destination.dat. If there are heights in the map less then or equal to -1000, change them to -800. This is applied to the entire map.

> The map border heights are never changed by any flatten option.


### Links
- [Back to Overview](../README.md)
