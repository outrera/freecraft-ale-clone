@ECHO OFF
GOTO START
REM
REM  ___________                     _________                _____  __
REM  \_   _____/______   ____   ____ \_   ___ \____________ _/ ____\/  |_
REM   |    __) \_  __ \_/ __ \_/ __ \/    \  \/\_  __ \__  \\  __ \\   __|
REM   |     \   |  | \/\  ___/\  ___/\     \____|  | \// __ \|  |   |  |
REM   \___  /   |__|    \___  >\___  >\______  /|__|  (____  /__|   |__|
REM       \/                \/     \/        \/            \/
REM ______________________                           ______________________
REM                       T H E   W A R   B E G I N S
REM	   FreeCraft - A free fantasy real time strategy game engine
REM
REM	build.bat	-	The graphics and sound extractor.
REM
REM	(c) Copyright 1999-2003 by Lutz Sammer and Jimmy Salmon.
REM
REM	FreeCraft is free software; you can redistribute it and/or modify
REM	it under the terms of the GNU General Public License as published
REM	by the Free Software Foundation; only version 2 of the License.
REM
REM	FreeCraft is distributed in the hope that it will be useful,
REM	but WITHOUT ANY WARRANTY; without even the implied warranty of
REM	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM	GNU General Public License for more details.
REM	
REM	$Id: build.bat,v 1.35 2003/02/12 21:01:16 jsalmon3 Exp $
REM
:START


REM
REM     Enter the letter of your CDROM drive.
REM

SET CDROM=D:


REM
REM     Alternatively: Enter the path to WC2 on your hard drive.
REM	If you have the expansion installed use the -e option.
REM

REM SET ARCHIVE="C:\Program Files\War2\data"
REM SET EXPANSION=-e
SET ARCHIVE=%CDROM%\data


REM
REM	This is the name of the directory where the files will be extracted.
REM	If you installed with fcmp then you should use data.wc2 instead.
REM

SET DIR=data
REM SET DIR=data.wc2








REM ###########################################################################
REM ##      DO NOT EDIT BELOW THIS LINE
REM ###########################################################################



SET BINDIR=tools
SET CONTRIB=contrib

IF NOT [%1] == [] SET CDROM=%1
IF NOT [%2] == [] SET DIR=%2


REM ###########################################################################
REM ##      Extract
REM ###########################################################################

ECHO Extracting files...

REM ADD -e      To force the archive are expansion compatible
REM ADD -n      To force the archive is not expansion compatible   
%BINDIR%\wartool %EXPANSION% %ARCHIVE% %DIR%

REM copy own supplied files

copy /b %CONTRIB%\cross.png %DIR%\graphics\ui\cursors
copy /b %CONTRIB%\red_cross.png %DIR%\graphics\missiles
copy /b %CONTRIB%\mana.png %DIR%\graphics\ui
copy /b %CONTRIB%\mana2.png %DIR%\graphics\ui
copy /b %CONTRIB%\health.png %DIR%\graphics\ui
copy /b %CONTRIB%\health2.png %DIR%\graphics\ui
copy /b %CONTRIB%\food.png %DIR%\graphics\ui
copy /b %CONTRIB%\score.png %DIR%\graphics\ui
copy /b "%CONTRIB%\ore,stone,coal.png" %DIR%\graphics\ui
copy /b %CONTRIB%\freecraft.png %DIR%\graphics\ui
mkdir %DIR%\music
copy /b %CONTRIB%\music\toccata.mod.gz %DIR%\music\default.mod.gz

REM ###########################################################################
REM ##      MISC
REM ###########################################################################

REM Compress HOW-TODO ?

REM
REM	Copy original puds into data directory
REM
mkdir %DIR%\puds\single
mkdir %DIR%\puds\multi
mkdir %DIR%\puds\multiple
mkdir %DIR%\puds\strange
copy /b %ARCHIVE%\..\puds\multi\* %DIR%\puds\multi
copy /b %ARCHIVE%\..\puds\single\* %DIR%\puds\single
copy /b %ARCHIVE%\..\puds\strange\* %DIR%\puds\strange
copy /b %ARCHIVE%\..\*.pud %DIR%\puds

REM
REM	Copy contrib puds into data directory
REM
copy /b %CONTRIB%\puds\single\* %DIR%\puds\single
copy /b %CONTRIB%\puds\multi\* %DIR%\puds\multiple

REM
REM	Setup the default pud
REM
copy /b %DIR%\puds\multi\(2)mysterious-dragon-isle.pud.gz %DIR%\puds\default.pud.gz
ECHO You only need to run this script once

:EOF
