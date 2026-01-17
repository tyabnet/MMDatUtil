# -copysrc

No parameter.

Recreate the `-outmap` by copying all of the data from `-srcmap`. Implies `-overwrite`

If -outmap file exists all data in it is overwritten.

Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc
```
source.dat is rewritten as destination.dat. The file encoding will be the default of UTF8 with BOM.

If you have both `-srcmap` and `-outmap`, `-copysrc` is used in most cases except for the merge operations.


### Links
- [Back to Overview](../README.md)
