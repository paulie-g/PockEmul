Update
======

The original author (matsumo/PockEmul) has chosen to discontinue development in favour of a paid, proprietary version with a monthly subscription model.
I make no moral judgments with respect to this decision and thank him for all his work on the GPL2-licensed version up until that point.
Unfortunately, his last GPL2-licensed master no longer builds on modern Linux systems (latest QT5, gcc13 etc).
I have pulled the changes from the most active fork (cbonello/PockEmul - evidently mostly Android fixes) and fixed the build.
This repo's master now succesfully builds the Linux desktop version on latest QT5 (~5.15) with gcc13.

To build, clone this repo, then do ```qmake pockemul.pro; make```

Building other targets (Windows, Android) has not been tested. Open an issue if you try it, pull requests welcome.

PockEmul
========
PockEmul is a Multi-Pocket Computer emulator.

![](http://pockemul.free.fr/forum/dev/device1.png)

It is develop in C++ using Qt4 or Qt5.

It is available on Win32, Linux, Mac OS X and Android

http://pockemul.free.fr

Main computer emulated:
 * Sharp pc-1245
 * Seiko MC-2200
 * Sharp pc-1250
 * Tandy PC-3
 * Sharp pc-1251
 * Tandy PC-3 (4Ko)
 * Sharp pc-1251H
 * Sharp pc-1255 
 * Sharp pc-1260 
 * Sharp pc-1261 
 * Sharp pc-1262 
 * Sharp pc-1280 
 * Sharp pc-1350 
 * Sharp pc-1360 
 * Sharp pc-1401 
 * Sharp pc-1402 
 * Sharp pc-1403
 * Sharp pc-1403H
 * Sharp pc-1421 
 * Sharp pc-1450 
 * Sharp pc-1475 
 * Sharp pc-1500 
 * Tandy PC-2 
 * Sharp pc-1500A
 * Sharp pc-1600 
 * Sharp pc-2500
 * Casio FP-200
 * Canon X-07
 * Sharp pc-E500 
 * Sharp pc-E550 
 * Sharp PC-G850V
 * Casio PB-1000
 * Casio PB-2000C
 * Casio Z-1GR
 * NEC PC-2001
 * Panasonic HHC
 * TI-57
 * HP-41

Emulated Extensions:
 * Sharp ce-152
 * Sharp ce-125
 * Seiko MP-220
 * Tandy 26-3591
 * Sharp ce-126p
 * Sharp ce-123p
 * Sharp ce-129p
 * Sharp ce-120p
 * Sharp ce-140p
 * Sharp ce-150"
 * Sharp ce-140f
 * Sharp ce-1600p
 * Canon X-710
 * Sharp EA-129C
 * Casio MD-100
 * Casio FP-100
 * Casio FP-40
 * NEC PC-2021
