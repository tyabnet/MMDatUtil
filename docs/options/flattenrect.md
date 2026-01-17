# -flattenrect

Parameters: `rowStart,colStart,rowEnd,colEnd`

`rowStart`,`colStart`,`rowEnd`,`colEnd` are positive integers.

This defines an inclusive region from [`rowStart`,`colStart`] to [`rowEnd`,`colEnd`] in the map to constrict all flatten operations to. 

This affects `-flattenabove`, `-flattenbelow`, `-flattenbetween`

>The region supplied is clipped to valid map space.

Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -flattenbetween -200,300,100 -flattenabove 1000,800 -flattenbelow -1000,-800 -flattenrect 11,12,21,22
```
- source.dat is rewritten as destination.dat.
- If there are heights in the map greater than or equal to -200 and less than or equal to 300, change them to 100.
- If there are heights greater than or equal to 1000, change them to 800.
- If there are heights less than or equal to -1000, change them to -800.
- Only do the above changes to a portion of the map defined by [11,12] to [21,22]


> The map border heights are never changed by any flatten option.

### Links
- [Back to Overview](../README.md)
