# File Encoding

## Overview
Encoding describes how text itself is encoded inside of a map file. Map files are text files with the .dat extension and the file contains sections with different meaning, but it is entirely textual data.  How each character is encoded in a text file is dependent on what that character is and the encoding format.

Under Windows there are two main formats:
- Windows Current Code Page (depends on language and keyboard)
- UTF (Unicode which is really UTF16LE)

Windows Current Code Page is a one byte per character format and has exactly 255 characters/symbols defined. The first 127 characters are all the same - these are the characters one finds on a Windows English keyboard. The upper 128 symbols are somewhat unique per language.

Unicode is a two byte per character format that follows the UTF 16 bit LE format.

There are technically three one byte formats that may be used.
- ANSI7. This is just the values 0-127 and perfectly represents the keys found on a Windows English keyboard such as A-Z, a-z, 0-9, space, tab, control values, and other special characters.
- ANSI8 (also called Extended ANSI). This is the Windows Current Code Page and it is ANSI7 plus the remaining 128 characters. The upper 128 characters can be different depending on your Windows language and keyboard settings.
- UTF8. This is a one byte version of UTF, and it represents the same range of characters as Unicode. The first 127 characters is identical to ANSI7, but the upper 128 values have a unique encoding that matches the UTF8 specification. These values are not binary compatible with ANSI8.

>If your map file and data only contain ANSI7 characters and English is the language used by your maps, then everything will just work for you correctly and you don't need to use any special options. MMDatUtil defaults are set to work perfectly with Manic Miners.

>Windows Extended ANSI may be perfectly converted to UTF without loss. But it is not possible to perfectly convert all UTF back to Windows Current Code Page. Why? UTF supports 1.1 million characters. Windows Extended ANSI only supports 255 characters.

## UTF
UTF is an international standard on how to encode text so it is uniquely understood world wide. It supports every known language including ancient ones, and every glyph/symbol for all of those languages. The spec allows over 1.1 million unique characters. At the time of this writing, Unicode v17.0.0 had defined over 150,000 characters. There are three major formats of UTF text.
- UTF8. One byte per character    (common on Android and Linux)
- UTF16. Two bytes per character  (common on Windows, IOS, MacOS)
- UTF32. Four bytes per character (common IOS and MacOS)

UTF16 and UTF32 have themselves each two different formats, little-endian (LE) and big-endian (BE) which describes what byte comes first.

>All of the UTF formats may be perfectly converted between each other without losing any data.

All modern text editors support reading and writing UTF8 and UTF16 text files - this includes Windows Notepad. `Notepad++` and `Visual Studio Code` are both free Windows editors that are very powerful.

Most text editors do not support UTF32 even on MacOS. If you need to work with UTF32 take a look at:
- Windows: BabelPad
- MacOS: BBEdit or Emacs

UTF files usually have a BOM - Byte Order Marker. This is a sequence of bytes at the beginning of the file that uniquely identify the encoding of the file. The text file contents come after the BOM.  Any application that needs to work with text files needs to be able to deal with this initial optional data. It is always recommended to include a BOM if generating UTF files. 

| BOM Encoding | Bytes Required |Byte 0 | Byte 1 | Byte 2 | Byte 3 |
|----------|----------------|-------|--------|--------|--------|
| UTF8 | 3 |0xEF | 0xBB | 0xBF | none |
| UTF16BE | 2 | 0xFE | 0xFF | none | none |
| UTF16LE | 2 | 0xFF | 0xFE | none | none |
| UTF32BE | 4 | 0x00 | 0x00 | 0xFE | 0xFF |
| UTF32LE | 4 | 0xFF | 0xFE | 0x00 | 0x00 |

If a BOM is missing - knowing how to process a text file requires a sequence of tests. The JSON file format publishes a specification that is used world wide on how to autodetect the format of a .json file without a BOM.

>MMDatUtil uses an enhanced series of tests that auto detects the encoding format for text files if they do not have a BOM. By default, 8 bit files without a BOM will be treated as Windows Current Code Page.

## Manic Miners Supported Formats.
Manic Miners supports the following formats for map files.

- ANSI 8 bit (Windows Current Code Page). This also includes ANSI7.
- UTF8 with BOM.
- UTF16LE with BOM.
- UTF16BE with BOM.

> If there is no BOM, Manic Miners only supports Windows Current Code Page and thus only ANSI7 and extended ANSI characters.

>UTF16 formats without BOM (both little endian and big endian) are NOT supported by Manic Miners. If using UTF16 the file must have a BOM.

>UTF32 formats (both little endian and big endian, with and without BOM) are NOT supported by Manic Miners.

The Manic Miner editor may create UTF16LE BOM or ANSI formats. Testing has shown that use of non-ANSI7 characters is one way that the engine will save the map in UTF16LE BOM format. It is unknown what other decisions the editor makes to decide what encoding to write a new map.

## MMDatUtil Supported Formats.
MMDatUtil will autodetect the encoding of all input files - maps, script, briefings.  If the input file is an 8 byte format and it does not have a BOM, it will use Windows Current Code Page to be compatible with Manic Miners.

>During output, the default encoding is UTF8 with a BOM. If you want to remove support for all characters above 255, you may force the output to be Windows code page by using `-ansi` option. Doing so will lose all unicode characters.

MMDatUtil has the ability to output formats that Manic Miners will not be able to read. This is a feature so if the map developer is using more 3rd party map processing tools then there is no restriction on the output by MMDatUtil.

The supported formats are:
- ANSI7 (warning if ANSI8 values detected)
- 8 bit ANSI using Windows current code page. To output, use -ansi option.
- UTF8 without BOM. (input requires -srcutf8 option).
- UTF8 with BOM.   This is the default output format.
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
