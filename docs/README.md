# MMDatUtil — Official Documentation

MMDatUtil is a windows command line utility that provides many operations on Manic Miners .DAT files.

- Quick read-only analysis of the .DAT file checking for overall correct layout. It does include some objective, blocks, and script checking.
- Ability to merge parts of map into another map, optionally resizing and offset, changing height, borders and other items.
- Override map information such as creator, level name and other items.
- Have the briefing and successbriefing sections be replaced by a user defined file.
- Replace the script section with script from another file with useful enhancements such as macro substitution, include files, removing comments, and even variable and event chain name optimization.
- many more options.

Why have this utility?

The primary reason for this utility is to bring to a Manic Miner Map developer the same process any software engineer would expect.  That process is edit, build, deploy, test, iterate, use github for version control.

Thus one is able to have a source map. Then combine (compile) into that map separate scripts and briefings, change meta data and generate a destination map file. The script may use both predefined and custom macros and can event imbed github commit hash. That can then be deployed to Manic Miners for testing. If the actual map needs to be editor, the user can bring back the modified map making it their new source map. And the process repeats with the user using github to manage branches, tags, stories, and all of the editable files.

Development process (high-level)

```
			  +----------------+
			  |  Source Map    |
			  |  (srcmap.dat)  |
			  +-------+--------+
				  |
	 user options / overrides |  (e.g. -copysrc, -mapname, -creator)
				  v
    +---------------------------------------------------------------+
    | Inputs merged into the Source Map                             |
    |                                                               |
    |  - Script files (macros, includes, optimizations)             |
    |  - Briefing / SuccessBriefing files                           |
    |  - Metadata overrides (map name, creator, defaults, borders)  |
    |                                                               |
    +-------------------------+-------------------------------------+
			      |
			      v
		      +-------+--------+
		      | Destination    |
		      | Map (outmap)   |
		      +-------+--------+
			      |
			      v
		  +-------------------------+
		  | Deploy / Test in game   |
		  +-----------+-------------+
			      |
	      if changes needed |  (edit in the level editor or edit the user files)
			      v
			  +---+-----+
			  | Iterate |
			  +---------+

```
Quick start. Scan a map file for possible issues (map is processed read-only):

```
MMDatUtil -srcmap "source.dat"
```

Copy a source .DAT into a destination .DAT, replacing it if it exists, and change the creator and map name.

```
MMDatUtil -srcmap "source.dat" -outmap "dest.dat" -copysrc -levelname "Time to Panic" -creator "Tyab"
```
A map has incorrect number of tiles, heights or ore/crystal resource. Fix the map and output to a new map. Make a backup so the current "dest.dat" is never lost.
```
MMDatUtil -srcmap "source.dat" -outmap "dest.dat" -fix -copysrc -backup
```

Change the creator name of an existing map - two ways.
```
MMDATUtil -outmap "dest.dat" -overwrite -creator "Tyab"
MMDATUtil -srcmap "dest.dat" -outmap "dest.dat" -copysrc -creator "Tyab"
```

The general format is:
 - -srcmap  Full path to the source map. Can be optional if source and destination are the same.
 - -outmap  Full path to the destination map. Required if modifying or creating a map.
 - -options that affect the source map.
 - -options that affect the destination map.

Review the list of options for more details.

### Links
- [File Encoding](fileencoding.md)
- [Filenames](filenames.md)
- [Options](options.md)
