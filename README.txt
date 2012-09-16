Purpose

Receives GPS data from a TTL UART GPS module and logs to
an SD card.  The info is logged as a text file using a FAT filesystem.

Information

For more information, please see the project blog at:
http://tinkerish.com/blog/?p=123

* main.c, Makefile - main project
* mmc_if.c, mmc_if.h - MMC/SD card driver
* diskio.c, diskio.h - lowlevel disk I/O shim between driver and filesystem
* ttf.c, ttf.h - tiny FAT filesystem

License

This project contains code shared under the GPL v2.  Some files are included
with this project and may be under a different license.  For each of those
files, the original copyright and source information is included, where
applicable.  If you have any questions, please contact me.

