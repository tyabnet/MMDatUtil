# File Encoding

## Overview
Encoding describes how text itself is encoded inside of a map file. Map files are text files with the .DAT extension and the file contains sections with different meaning, but it is entirely textual data.  How each character is encoded in a text file is dependent on what that character is and the encoding format.

For example all 8 byte formats encode ANSI western characters exactly as they appear - one byte per character each with a value under 128. These are characters such as A-Z, 0-9, a-z, some special characters and so on. These are all of the characters one would find on a Windows English keyboard.

>If your map file and data only contain these characters and English is the language used by your maps, then everything will just work for you correctly and you don't need to use any special options.

However if you wish to include non-ANSI characters in your map, this section now become important. These would be the Windows Extended ANSI set (character values 128-255) and Unicode characters. 

Under windows, there are really only two ways text files work with text. 
- Windows current code page. This is the default format for text data under windows. It is one byte per character and it perfectly encodes the ANSI characters. It also has the upper 128-255 values defined - but these can change depending on what code page (language) windows is configured for.  These files CANNOT have any character encodings over 255 - they are NOT UTF8.
- Unicode. This is a two byte per character format that has extended characters to support all languages. This is UTF16 little-endian format.

> Windows has the concept of a current code page. This is how Windows deals with the language it is configured for and for the type of keyboard connected. The values of characters 128 and higher are unique per code page.

## UTF
UTF is an international standard on how to encode text so it is uniquely understood world wide. There are three major formats of UTF text.
- UTF8. One byte per character    (Android and Linux)
- UTF16. Two bytes per character  (Windows, IOS, MacOS)
- UTF32. Four bytes per character (IOS and MacOS)

UTF16 and UTF32 have themselves each two different formats, little-endian (LE) and big-endian (BE) which describes what byte comes first.

>All of the UTF formats may be converted between each other without losing any data.

All modern text editors support reading and writing UTF8 and UTF16 text files - this includes Windows Notepad.  Some editors also support UTF32. JSON and Python can easily convert between all formats.

UTF files usually have an optional BOM - Byte Order Marker. This is a sequence of bytes at the beginning of the file that uniquely identify the encoding of the file. The text file contents come after the BOM.  Any application that needs to work with text files needs to be able to deal with this initial optional data. It is always recommended to include a BOM if generating UTF files.

| BOM Encoding | Bytes Required |Byte 0 | Byte 1 | Byte 2 | Byte 3 |
|----------|----------------|-------|--------|--------|--------|
| UTF8 | 3 |0xEF | 0xBB | 0xBF | none |
| UTF16BE | 2 | 0xFE | 0xFF | none | none |
| UTF16LE | 2 | 0xFF | 0xFE | none | none |
| UTF32BE | 2 | 0x00 | 0x00 | 0xFE | 0xFF |
| UTF32LE | 2 | 0xFF | 0xFE | 0x00 | 0x00 |

If a BOM is missing - knowing how to process a text file requires a sequence of tests. The JSON file format publishes a specification that is used world wide on how to autodetect the format of a .json file without a BOM.

>MMDatUtil uses an enhanced series of tests that auto detects the encoding format for all text files if they do not have a BOM.

## Manic Miners Supported Formats.
Manic Miners supports the following formats for map files.

- ANSI 8 bit (Windows current code page) with no BOM.
- UTF8 with BOM.
- UTF16LE with BOM.
- UTF16BE with BOM.

> If there is no BOM, Manic Miners only supports Windows current code page and thus only ANSI and extended ANSI characters.

>UTF16 formats without BOM (both little endian and big endian) are NOT supported by Manic Miners. If using UTF16 the file must have a BOM.

>UTF32 formats (both little endian and big endian, with and without BOM) are NOT supported by Manic Miners.

The Manic Miner editor may create UTF16LE BOM or ANSI formats. Testing has shown that use of non-ANSI characters is one way that the engine will save the map in UTF16LE BOM format. It is unknown what other decisions the editor makes to decide what encoding to write a new map.

## MMDatUtil Supported Formats.
MMDatUtil will autodetect the encoding of all input files - maps, script, briefings.  If the input file is an 8 byte format and it does not have a BOM, it will use Windows current code page to be compatible with Manic Miners.

>During output, the default encoding is UTF8 with a BOM. If you want to remove support for all characters above 255, you may force the output to be Windows code page by using `-ansi` option. Doing so will lose all unicode characters and may change some of the extended ANSI characters.

MMDatUtil has the ability to output formats that Manic Miners will not be able to read. This is a feature so if the map developer is using more 3rd party map processing tools then there is no restriction on the output by MMDatUtil.

The supported formats are:
- 8 bit ANSI using Windows current code page. To output, use -ansi option.
- UTF8 without BOM. (input requires -srcutf8 option).
- UTF8 with BOM.   This is the default format for output unless options change it.
- UTF16LE without BOM.
- UTF16LE with BOM.
- UTF16BE without BOM.
- UTF16BE with BOM.
- UTF32LE without BOM.
- UTF32LE with BOM.
- UTF32BE without BOM.
- UTD32BE with BOM.

To output any UTF without a BOM, you need to use the -nobom option. Generally all UTF files should include the BOM.

### Manic Miner DAT File Format.

The Manic Miner Script documentation fully describes the .DAT text format for each section in the map file.

[Manic Miner File Format](https://manicminers.github.io/docs/#/_pages/DATFileFormat)

### Links
- [Back to Overview](README.md)
- [Filenames](filenames.md)
