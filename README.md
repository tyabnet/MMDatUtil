Tyab's Manic Miners .DAT utility (MMDatUtil)
===========================================

A command-line utility for viewing, merging and manipulating Manic Miner .DAT level files.

This repository contains the source for `MMDatUtil` (C++), a small tool for merging data from one .DAT map into another, editing map metadata, processing Script, and converting text encodings of .dat files.





Contents
--------
- `MMDatUtil.cpp`, `MMDatUtil.h` - main program and headers
- `MMScript.h` - script engine used to parse/process TyabScript
- `MMScript.h` - script engine used to parse/process TyabScript

This `README.md` is the official documentation for MMDatUtil. The previous PDF/DOCX user manual has been retired and the documentation has been consolidated here.

Quick usage
-----------
Run the utility from a command prompt. Options start with `-` (or `--` or `/`). Parameters containing spaces should be quoted.

  MMDatUtil.exe -help

Options
-------
(Extracted from the built-in help)

- -help           display this help
- -srcmap         file name of a source merge .DAT
- -outmap         file name of a destination .DAT
- -overwrite      allow changing existing outmap
- -copysrc        outmap is recreated from srcmap, implies -overwrite
- -mergeheight    merge height values from srcmap into outmap
- -mergecrystal   merge crystals values from srcmap into outmap
- -mergeore       merge ore values from srcmap into outmap
- -mergetile      merge tile values from srcmap into outmap
- -mergerect      startrow,startcol,endrow,endcol for merge
- -offsetrow      add row offset when merging/copying srcmap into outmap
- -offsetcol      add col offset when merging/copying srcmap into outmap
- -resizerow      resize outmap rows for tiles,height,resources
- -resizecol      resize outmap cols for tiles,height,resources
- -deftile        value for invalid tiles or resize, default 1
- -defheight      value for invalid heights or resize, default 0
- -defcrystal     value for invalid crystals or resize, default 0
- -defore         value for invalid ore or resize, default 0
- -mapname        levelname: value saved in outmap info section
- -creator        creator: value saved in outmap info section
- -fix            fix invalid/missing tile, height, crystal, ore values
- -script         filename of script file to replace outmap's script
- -sincdirs       ; separated list of paths to search for script includes
- -sfixspace      automatically remove spaces where not allowed in scripts
- -snocomment     remove all comments in script except #.
- -sdefine        name=value   define script substitution
- -sdatefmt       format for TyabScript{Inc}Date, default "y.m.d"
- -soptnames      Optimize script variable and event chain names
- -soptblank      Remove script blank lines
- -flattenabove   height, newheight. Heights > height set to newheight
- -flattenbelow   height, newheight. Heights < height set to newheight
- -flattenbetween low, high, value. low <= Heights <= high set to value
- -borderheight   force height borders to this value
- -unicode        output .dat is UTF16LE format (default UTF8)
- -utf16LE        same as -unicode
- -utf16BE        output .dat is UTF16BE format (default UTF8)
- -utf32LE        output .dat is UTF32LE format (default UTF8)
- -utf32BE        output .dat is UTF32EE format (default UTF8)
- -bom            output .dat has BOM Byte Order Marker (default none)
- -srcansi        UTF8 input files without BOM assume ANSI codepage
- -briefing       filename. Will replace the briefing message in output
- -success        filename. Will replace the success message in output

Notes and rules
--------------
- Options that perform merges require `-srcmap` (unless `-copysrc`/`-eraseoutmap` semantics are used).
- Both `-srcmap` and `-outmap` must have a `.dat` extension when provided.
- Only one of `-utf8`, `-utf16*` or `-utf32*` may be used for output format.
- `-utf*` and `-bom` options require `-outmap` to be specified.
- Using `-copysrc` with merge options is not meaningful (the `-copysrc` implies overwriting/erasing the destination).

Examples
--------
- Merge tiles and heights from `src.dat` into `out.dat` with an offset:

  MMDatUtil.exe -srcmap "src.dat" -outmap "out.dat" -mergetile -mergeheight -offsetrow 2 -offsetcol -1 -overwrite

- Create a new `out.dat` copied from `src.dat`, then set the level name and creator:

  MMDatUtil.exe -srcmap "src.dat" -outmap "out.dat" -copysrc -mapname "My Level" -creator "Alice" -overwrite

- Replace the script of an existing map and optimize names:

  MMDatUtil.exe -outmap "out.dat" -script "newscript.scr" -soptnames -soptblank -overwrite

Build and run
-------------
This project is a Visual Studio C++ solution (.sln) and uses the MSVC toolchain.

To build in Visual Studio:
- Open `MMDatUtil.sln` in Visual Studio 2022+ and build the solution.

To run from Developer Command Prompt or PowerShell (after building):
- Run the executable from the `x64\Debug` or `x64\Release` folder, e.g.:

  x64\Debug\MMDatUtil.exe -help

Further documentation
---------------------
Further documentation
---------------------
This repository contains a small static documentation site under the `docs/` folder. The site contains a page for each command-line option and a navigation menu.

To view locally, open `docs/index.html` in your browser. For a better experience (and to avoid local file restrictions), serve the `docs/` folder using a simple static server. For example, with Python 3 installed:

```powershell
cd docs
python -m http.server 8000
# then open http://localhost:8000
```

If you need additional examples or clarifications, please open an issue in the repository and mark it documentation.

License & author
----------------
See the source headers. Copyright (c) 2022 by Tyab.

Issues
------
File a bug report or feature request using the repository issue tracker.
