Tyab's Manic Miners .DAT utility (MMDatUtil)
===========================================

A command-line utility for viewing, merging and manipulating Manic Miner .DAT level files.

This repository contains the source for `MMDatUtil` (C++), a small tool for merging data from one .DAT map into another, editing map metadata, processing Script, and converting text encodings of .dat files.

This `README.md` is the official documentation for MMDatUtil and is the root for the on-line documentation. The previous PDF/DOCX user manual has been retired.

Quick usage
-----------
Run the utility from a command prompt. Options start with `-` (or `--` or `/`). Parameters containing spaces should be quoted.

  MMDatUtil.exe -help

## Usage
The general format is:
 - -srcmap  Full path to the source map. Can be optional if source and destination are the same.
 - -outmap  Full path to the destination map. Required if modifying or creating a map.
 - -options that affect the source map.
 - -options that affect the destination map.

Review the list of options for more details.

# Online documentation
[Online documentation](https://tyabnet.github.io/MMDatUtil/#/)

THIS IS DEVELOPMENT DOCUMENTATION. WHEN RELEASED CHANGE LINK IN MAIN README.MD AND SETTINGS ON GITHUB -> SETTINGS -> PAGES -> BRANCH


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
This project is a Microsoft Visual Studio 2026 C++ solution (.sln) and uses the v145 MSVC toolchain.

To build in Visual Studio:
- Open `MMDatUtil.sln` in Visual Studio 2026+ and build the solution.

To run from Developer Command Prompt or PowerShell (after building):
- Run the executable from the `x64\Debug` or `x64\Release` folder, e.g.:

  x64\Debug\MMDatUtil.exe -help

Further documentation
---------------------
Github is hosting the online documentation. While not required, one can look at the documentation locally. One may also run a local http server to view it.

To view locally, open `docs/index.html` in your browser. For a better experience (and to avoid local file restrictions), serve the `docs/` folder using a simple static server. For example, with Python 3 installed:

```
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
