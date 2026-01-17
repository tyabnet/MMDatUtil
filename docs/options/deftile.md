# -deftile

Parameter: `integer` (1..175)

Default tile for newly created tiles during merging or resizing. The default value is 1 (ground).

Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -resizecol 54 -resizerow 56 -deftile 1
```
source.dat is rewritten as destination.dat and it is resized to be 54 tiles wide by 56 tiles tall. If this is larger than the size of source.dat, then the newly created tiles will all be tile id 1 which is ground. These tile values are in the tiles section.

>This TileID will not be used for newly created border values. Border tiles will always be set to Solid Rock Regular (38).

### Links
- [Back to Overview](../README.md)
- [Manic Miners Tile IDs](https://manicminers.github.io/docs/#/_pages/DATTileReference)

