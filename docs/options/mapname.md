# -mapname

Parameter: `string`

Set the info section levelname: value saved in the output map info section. This is the name of the map.

Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -mapname "Rocky Horror"
```
source.dat is rewritten as destination.dat and its info levelname: value set to `Rocky Horror`

```
MMDatUtil -outmap "destination.dat" -overwrite -mapname "Rocky Horror"
```
destination.dat will be rewritten and have the info levelname: field set to `Rocky Horror`


### Links
- [Back to Overview](../README.md)
