# menu_data.c: Menu structure for Fuse
# Copyright (c) 2004 Philip Kendall

# $Id$

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA

# Author contact information:

# Philip Kendall <pak21-fuse@srcf.ucam.org>
# Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

_File, Branch
File/_Open..., Item, F3
File/_Save Snapshot..., Item, F2

File/_Recording, Branch
File/Recording/_Record..., Item
File/Recording/Record _from snapshot..., Item
File/Recording/_Insert Snapshot, Item, Insert
File/Recording/Roll_back, Item, Delete
File/Recording/Rollback _to..., Item
File/Recording/_Play..., Item
File/Recording/_Stop, Item

File/A_Y Logging, Branch
File/AY Logging/_Record..., Item
File/AY Logging/_Stop, Item

File/separator, Separator
File/O_pen SCR Screenshot..., Item
File/S_ave Screen as SCR..., Item

#ifdef USE_LIBPNG
File/Save S_creen as PNG..., Item
#endif

File/_Movies, Branch
File/Movies/Record Movie as _SCR..., Item
#ifdef USE_LIBPNG
File/Movies/Record Movie as _PNG..., Item
#endif
File/Movies/S_top Movie Recording, Item

#ifndef USE_WIDGET
File/separator, Separator
File/Loa_d binary data..., Item
File/Save _binary data..., Item
#endif

File/separator, Separator
File/E_xit..., Item, F10

_Options, Branch
Options/_General..., Item, F4
Options/_Sound..., Item
Options/_Peripherals..., Item
Options/_RZX..., Item

Options/_Joysticks, Branch
Options/Joysticks/Joystick _1..., Item,, menu_options_joysticks_select, 1
Options/Joysticks/Joystick _2..., Item,, menu_options_joysticks_select, 2
Options/Joysticks/_Keyboard..., Item,, menu_options_joysticks_select, 3

Options/S_elect ROMs, Branch
Options/Select ROMs/Spectrum 1_6K..., Item,, menu_options_selectroms_select, 1
Options/Select ROMs/Spectrum _48K..., Item,, menu_options_selectroms_select, 2
Options/Select ROMs/Spectrum _128K..., Item,, menu_options_selectroms_select, 3
Options/Select ROMs/Spectrum +_2..., Item,, menu_options_selectroms_select, 4
Options/Select ROMs/Spectrum +2_A..., Item,, menu_options_selectroms_select, 5
Options/Select ROMs/Spectrum +_3..., Item,, menu_options_selectroms_select, 6
Options/Select ROMs/Spectrum +3_e..., Item,, menu_options_selectroms_select, 11
Options/Select ROMs/Timex _TC2048..., Item,, menu_options_selectroms_select, 7
Options/Select ROMs/Timex TC206_8..., Item,, menu_options_selectroms_select, 8
Options/Select ROMs/Timex TS2_068..., Item,, menu_options_selectroms_select, 14
Options/Select ROMs/_Pentagon 128K..., Item,, menu_options_selectroms_select, 9
Options/Select ROMs/S_corpion ZS 256..., Item,, menu_options_selectroms_select, 10
Options/Select ROMs/Spectrum _SE..., Item,, menu_options_selectroms_select, 12,
Options/Select ROMs/Interface _I..., Item,, menu_options_selectroms_select, 13

Options/_Filter..., Item

#ifdef HAVE_LIB_XML2
Options/separator, Separator
Options/S_ave, Item
#endif

_Machine, Branch

#ifndef USE_WIDGET
Machine/_Pause..., Item, Pause
#endif

Machine/_Reset..., Item, F5
Machine/_Select..., Item, F9
Machine/_Debugger..., Item
Machine/P_oke Finder..., Item

#ifndef USE_WIDGET
Machine/_Memory Browser..., Item
#endif

Machine/Pro_filer, Branch
Machine/Profiler/_Start, Item
Machine/Profiler/_Stop, Item

Machine/_NMI, Item

M_edia, Branch

Media/_Tape, Branch
Media/Tape/_Open..., Item, F7
Media/Tape/_Play, Item, F8
Media/Tape/_Browse..., Item
Media/Tape/_Rewind, Item
Media/Tape/_Clear, Item
Media/Tape/_Write..., Item, F6

Media/_Interface I, Branch

Media/Interface I/Microdrive _1, Branch
Media/Interface I/Microdrive 1/Insert _New, Item,, menu_media_mdr_new, 1
Media/Interface I/Microdrive 1/_Insert..., Item,, menu_media_mdr_insert, 1
Media/Interface I/Microdrive 1/_Sync, Item,, menu_media_mdr_sync, 1
Media/Interface I/Microdrive 1/_Eject, Item,, menu_media_mdr_eject, 1
Media/Interface I/Microdrive 1/_Write protect, Branch
Media/Interface I/Microdrive 1/Write protect/_Set, Item,, menu_media_mdr_writep, 0x11
Media/Interface I/Microdrive 1/Write protect/_Remove, Item,, menu_media_mdr_writep, 0x01

Media/Interface I/Microdrive _2, Branch
Media/Interface I/Microdrive 2/Insert _New, Item,, menu_media_mdr_new, 2
Media/Interface I/Microdrive 2/_Insert..., Item,, menu_media_mdr_insert, 2
Media/Interface I/Microdrive 2/_Sync, Item,, menu_media_mdr_sync, 2
Media/Interface I/Microdrive 2/_Eject, Item,, menu_media_mdr_eject, 2
Media/Interface I/Microdrive 2/_Write protect, Branch
Media/Interface I/Microdrive 2/Write protect/_Set, Item,, menu_media_mdr_writep, 0x12
Media/Interface I/Microdrive 2/Write protect/_Remove, Item,, menu_media_mdr_writep, 0x02

Media/Interface I/Microdrive _3, Branch
Media/Interface I/Microdrive 3/Insert _New, Item,, menu_media_mdr_new, 3
Media/Interface I/Microdrive 3/_Insert..., Item,, menu_media_mdr_insert, 3
Media/Interface I/Microdrive 3/_Sync, Item,, menu_media_mdr_sync, 3
Media/Interface I/Microdrive 3/_Eject, Item,, menu_media_mdr_eject, 3
Media/Interface I/Microdrive 3/_Write protect, Branch
Media/Interface I/Microdrive 3/Write protect/_Set, Item,, menu_media_mdr_writep, 0x13
Media/Interface I/Microdrive 3/Write protect/_Remove, Item,, menu_media_mdr_writep, 0x03

Media/Interface I/Microdrive _4, Branch
Media/Interface I/Microdrive 4/Insert _New, Item,, menu_media_mdr_new, 4
Media/Interface I/Microdrive 4/_Insert..., Item,, menu_media_mdr_insert, 4
Media/Interface I/Microdrive 4/_Sync, Item,, menu_media_mdr_sync, 4
Media/Interface I/Microdrive 4/_Eject, Item,, menu_media_mdr_eject, 4
Media/Interface I/Microdrive 4/_Write protect, Branch
Media/Interface I/Microdrive 4/Write protect/_Set, Item,, menu_media_mdr_writep, 0x14
Media/Interface I/Microdrive 4/Write protect/_Remove, Item,, menu_media_mdr_writep, 0x04

Media/Interface I/Microdrive _5, Branch
Media/Interface I/Microdrive 5/Insert _New, Item,, menu_media_mdr_new, 5
Media/Interface I/Microdrive 5/_Insert..., Item,, menu_media_mdr_insert, 5
Media/Interface I/Microdrive 5/_Sync, Item,, menu_media_mdr_sync, 5
Media/Interface I/Microdrive 5/_Eject, Item,, menu_media_mdr_eject, 5
Media/Interface I/Microdrive 5/_Write protect, Branch
Media/Interface I/Microdrive 5/Write protect/_Set, Item,, menu_media_mdr_writep, 0x15
Media/Interface I/Microdrive 5/Write protect/_Remove, Item,, menu_media_mdr_writep, 0x05

Media/Interface I/Microdrive _6, Branch
Media/Interface I/Microdrive 6/Insert _New, Item,, menu_media_mdr_new, 6
Media/Interface I/Microdrive 6/_Insert..., Item,, menu_media_mdr_insert, 6
Media/Interface I/Microdrive 6/_Sync, Item,, menu_media_mdr_sync, 6
Media/Interface I/Microdrive 6/_Eject, Item,, menu_media_mdr_eject, 6
Media/Interface I/Microdrive 6/_Write protect, Branch
Media/Interface I/Microdrive 6/Write protect/_Set, Item,, menu_media_mdr_writep, 0x16
Media/Interface I/Microdrive 6/Write protect/_Remove, Item,, menu_media_mdr_writep, 0x06

Media/Interface I/Microdrive _7, Branch
Media/Interface I/Microdrive 7/Insert _New, Item,, menu_media_mdr_new, 7
Media/Interface I/Microdrive 7/_Insert..., Item,, menu_media_mdr_insert, 7
Media/Interface I/Microdrive 7/_Sync, Item,, menu_media_mdr_sync, 7
Media/Interface I/Microdrive 7/_Eject, Item,, menu_media_mdr_eject, 7
Media/Interface I/Microdrive 7/_Write protect, Branch
Media/Interface I/Microdrive 7/Write protect/_Set, Item,, menu_media_mdr_writep, 0x17
Media/Interface I/Microdrive 7/Write protect/_Remove, Item,, menu_media_mdr_writep, 0x07

Media/Interface I/Microdrive _8, Branch
Media/Interface I/Microdrive 8/Insert _New, Item,, menu_media_mdr_new, 8
Media/Interface I/Microdrive 8/_Insert..., Item,, menu_media_mdr_insert, 8
Media/Interface I/Microdrive 8/_Sync, Item,, menu_media_mdr_sync, 8
Media/Interface I/Microdrive 8/_Eject, Item,, menu_media_mdr_eject, 8
Media/Interface I/Microdrive 8/_Write protect, Branch
Media/Interface I/Microdrive 8/Write protect/_Set, Item,, menu_media_mdr_writep, 0x18
Media/Interface I/Microdrive 8/Write protect/_Remove, Item,, menu_media_mdr_writep, 0x08

Media/Interface I/_RS232, Branch
Media/Interface I/RS232/Plug _RxD, Item,, menu_media_if1_rs232, 0x01
Media/Interface I/RS232/_Unplug RxD, Item,, menu_media_if1_rs232, 0x11
Media/Interface I/RS232/Plug _TxD, Item,, menu_media_if1_rs232, 0x02
Media/Interface I/RS232/Un_plug TxD, Item,, menu_media_if1_rs232, 0x12

Media/Interface I/_Sinclair NET, Branch
Media/Interface I/Sinclair NET/_Plug in, Item,, menu_media_if1_rs232, 0x03
Media/Interface I/Sinclair NET/_Unplug, Item,, menu_media_if1_rs232, 0x13

Media/_Disk, Branch

Media/Disk/+_3, Branch
Media/Disk/+3/Drive _A:, Branch
Media/Disk/+3/Drive A:/_Insert..., Item,, menu_media_disk_insert, 1
Media/Disk/+3/Drive A:/_Eject, Item,, menu_media_disk_eject, 1
Media/Disk/+3/Drive A:/Eject and _write..., Item,, menu_media_disk_eject, 3
Media/Disk/+3/Drive _B:, Branch
Media/Disk/+3/Drive B:/_Insert..., Item,, menu_media_disk_insert, 2
Media/Disk/+3/Drive B:/_Eject, Item,, menu_media_disk_eject, 2
Media/Disk/+3/Drive B:/Eject and _write..., Item,, menu_media_disk_eject, 4

Media/Disk/_TR-DOS, Branch
Media/Disk/TR-DOS/Drive _A:, Branch
Media/Disk/TR-DOS/Drive A:/_Insert..., Item,, menu_media_disk_insert, 3
Media/Disk/TR-DOS/Drive A:/_Eject, Item,, menu_media_disk_eject, 5
Media/Disk/TR-DOS/Drive A:/Eject and _write..., Item,, menu_media_disk_eject, 7
Media/Disk/TR-DOS/Drive _B:, Branch
Media/Disk/TR-DOS/Drive B:/_Insert..., Item,, menu_media_disk_insert, 4
Media/Disk/TR-DOS/Drive B:/_Eject, Item,, menu_media_disk_eject, 6
Media/Disk/TR-DOS/Drive B:/Eject and _write..., Item,, menu_media_disk_eject, 8

Media/_Cartridge, Branch
Media/Cartridge/_Timex Dock, Branch
Media/Cartridge/Timex Dock/_Insert..., Item
Media/Cartridge/Timex Dock/_Eject, Item
Media/Cartridge/_Interface II, Branch
Media/Cartridge/Interface II/_Insert..., Item
Media/Cartridge/Interface II/_Eject, Item

Media/ID_E, Branch

Media/IDE/Simple _8-bit, Branch
Media/IDE/Simple 8-bit/_Master, Branch
Media/IDE/Simple 8-bit/Master/_Insert..., Item,, menu_media_ide_insert, 1
Media/IDE/Simple 8-bit/Master/_Commit, Item,, menu_media_ide_commit, 1
Media/IDE/Simple 8-bit/Master/_Eject, Item,, menu_media_ide_eject, 1
Media/IDE/Simple 8-bit/_Slave, Branch
Media/IDE/Simple 8-bit/Slave/_Insert..., Item,, menu_media_ide_insert, 2
Media/IDE/Simple 8-bit/Slave/_Commit, Item,, menu_media_ide_commit, 2
Media/IDE/Simple 8-bit/Slave/_Eject, Item,, menu_media_ide_eject, 2

Media/IDE/ZX_ATASP, Branch
Media/IDE/ZXATASP/_Master, Branch
Media/IDE/ZXATASP/Master/_Insert..., Item,, menu_media_ide_insert, 3
Media/IDE/ZXATASP/Master/_Commit, Item,, menu_media_ide_commit, 3
Media/IDE/ZXATASP/Master/_Eject, Item,, menu_media_ide_eject, 3
Media/IDE/ZXATASP/_Slave, Branch
Media/IDE/ZXATASP/Slave/_Insert..., Item,, menu_media_ide_insert, 4
Media/IDE/ZXATASP/Slave/_Commit, Item,, menu_media_ide_commit, 4
Media/IDE/ZXATASP/Slave/_Eject, Item,, menu_media_ide_eject, 4

Media/IDE/ZX_CF CompactFlash, Branch
Media/IDE/ZXCF CompactFlash/_Insert..., Item,, menu_media_ide_insert, 5
Media/IDE/ZXCF CompactFlash/_Commit, Item,, menu_media_ide_commit, 5
Media/IDE/ZXCF CompactFlash/_Eject, Item,, menu_media_ide_eject, 5

_Help, Branch
Help/_Keyboard..., Item
