# -fix

No parameter.

Attempt to missing tile, height, crystal, and ore values when writing a map. Will also remove extra values not needed. 
 
Requires use of the -outmap option.

Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -fix
```
source.dat is rewritten as destination.dat. If the output would have an invalid tiles, heights, crystals, or ore sections, values will be added or removed as needed.

If values are missing, then `-defore`, `-defcrystal`, `-deftiles`, `-defheight` will provide the missing values.

### Links
- [Back to Overview](../README.md)
