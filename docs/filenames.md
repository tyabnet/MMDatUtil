# All about Filenames and Encoding.

## Double Quotes

MMDatUtil accepts filenames that are enclosed in double quotes. That is the preferred method to use a filename - that way there is no confusion. But if a filename does not have spaces or other special characters, it does not have to be enclosed in double quotes. All characters of a filename must be valid for Windows. MMDatUtil detects most invalid files names and will report an error.

Example of filename with spaces
```
MMDatUtil -srcmap "C:\my maps\Time to Panic v 1.dat"
```


## Long File Names

MMDatUtil supports long paths and the \\\\?\ format - it imposes no limits on the length of filenames. It is up to the user to ensure input and output files names are correct. Incorrect filenames will usually generate an error.

MMDatUtil allows use of relative paths - they will be relative from the current directory where MMDatUtil was launched which does not have to be the directory where MMDatUtil itself is located.

>Windows does not allow the long file name pattern of \\\\?\\ to be used with relative paths.

>Manic Miners is not compatible with windows long path support. This means the directory that Manic Miners runs from cannot exceed the path limits of 255 characters including the subdirectories it references.

### Links
### Links
- [Back to Overview](README.md)
- [File Encoding](fileencoding.md)
