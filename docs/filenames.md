# All about Filenames and Encoding.

## Double Quotes

MMDatUtil accepts filenames that are enclosed in double quotes. That is the preferred method to use a filename - that way there is no confusion. But if a filename does not have spaces or other special characters, it does not have to be enclosed in double quotes. All characters of a filename must be valid for Windows. MMDatUtil detects most invalid files names and will report an error.

Example of filename with spaces
```
MMDatUtil -srcmap "C:\my maps\Time to Panic v 1.dat"
```

## Relative Paths

MMDatUtil allows use of relative paths - they will be relative from the current directory where MMDatUtil was launched which does not have to be the directory where MMDatUtil itself is located.

## Long File Names
Windows has two kinds of long paths.
- Legacy \\\\?\\ format. This sequence of characters may be used at the start of a long path.
- Win 10/11 native long paths. This requires that a registry entry be enabled in order for windows itself to support native long paths without the special characters.

MMDatUtil supports both formats and is a long path aware application. It imposes no limits on the length of filenames. It is up to the user to ensure input and output files names are correct. Incorrect filenames will usually generate an error.

>Windows does not allow the long file name pattern of \\\\?\\ to be used with relative paths.

To enable native long path support in win 10/11 use this statement:
```
reg add "HKLM\SYSTEM\CurrentControlSet\Control\FileSystem" /v LongPathsEnabled /t REG_DWORD /d 1 /f
```
Changing this setting requires a reboot.

## Manic Miners is Not long path compatible

Manic Miners is not compatible with either Windows long path formats. This means the directory and files that Manic Miners runs from cannot exceed the path limits of 259 characters including the subdirectories it references.

### Links
- [Back to Overview](README.md)
- [File Encoding](fileencoding.md)
