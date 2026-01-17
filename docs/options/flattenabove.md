# -flattenabove

Parameters: `height,newheight`

`height` and `newheight` are signed integers.

Set heights equal to or greater than `height` to `newheight`.  This is applied to the entire map.

This option is the opposite of `-flattenbelow`.

>-flattenrect will constrict changes to a region.

This option is useful to fix large upward spikes in elevation to a given value. For example, if you have a large mountain and want to shave off the top creating a flat area.

Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -flattenabove 1000,800
```
source.dat is rewritten as destination.dat. If there are heights in the map greater than or equal to 1000, change them to 800. This is applied to the entire map.

> The map border heights are never changed by any flatten option.


### Links
- [Back to Overview](../README.md)
