# -creator

Parameter: `string`

Set the creator metadata value saved in the `-outmap` info section.

Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -creator "Tyab Actual"
```
source.dat is rewritten as destination.dat and its info creator: field set to `Tyab Actual`

```
MMDatUtil -outmap "destination.dat" -overwrite -creator "Tyab Actual"
```
destination.dat will be rewritten and have the info creator: value set to `Tyab Actual`


### Links
- [Back to Overview](../README.md)
