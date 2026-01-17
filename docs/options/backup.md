# -backup

No parameter.

Destination file (if it exists) will be backed up so its contents will never be lost.

MMDatUtil first writes any output to a temp file. Then if this option is used, the current (if it exists) destination file is copied to a backup file with a unique name, following the pattern described below. Once the current destination has been backed up, then the temp file is copied to the destination file.

This option will continue to generate new unique backup names. It will never delete or override any existing file so you are able to have a history of backups.

Backup files are in the same directory as the destination file.

The file name format is:
> FILENAME_Backup_YYYYMMDD_nnn.DAT

 - FILENAME is the destination backup without the .DAT extension.
 - YYYY is the current year as a 4 digit number.
 - MM is the current month 01-12 as a 2 digit number.
 - DD is the day of the month 01-31 as a 2 digit number.
 - nnn is a sequence number starting at 1 and increments. There is no upper limit to this number.

 The sequence number starts at 1 for the current date and increments by one for every backup. Thus 1 is the oldest backup, and the highest number in the directory is the latest backup.
 Because no backup file is ever overwritten with this option, you will get more and more .DAT backups over time and may eventually want to clean up the backups manually.

 Example:

```
MMDatUtil -srcmap "source.dat" -outmap "destination.dat" -copysrc -backup
```
source.dat is rewritten as destination.dat with destination.dat being backed-up prior to replacement if it already exists.


### Links
- [Filenames](../filenames.md)
- [File Encoding](../fileencoding.md)
- [Back to Overview](../README.md)
