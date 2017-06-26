# SyncTeX

Synchronization for TeX

This is a partial clone of synctexdir in the TeXLive svn repository.
The focus is on the client code for synchronization between text editor and pdf viewer.
Instructions below are given to build the synctex command line tool on various operating systems.

## Clone
Clone the git repository 

## Instruction for OS X
These instructions are for Xcode 7 but they might apply as well for other versions.

### create a new project

- create a new project
- choose a command line tool project
- set the product name to synctex
- set the language to C
- save it next to the synctexdir folder just cloned
- in the navigation pane, remove the default syntex folder and its `main.c` default contents

you then have both folders synctex and synctexdir in the same directory.

### Add sources

- select the synctex project in xcode navigation left pane
- select menu itme File->Add files to "synctex"
- select the synctexdir folder in the file browser
- uncheck "Copy items if needed"
- choose "Create groups"
- validate your choice
- select the synctex project
- go to the build phase pane
- remove `Readme.md` from the compile sources list

You end with 3 sources : `synctex_main.c`, `synctex_parser.c`, `synctex_parser_utils.c`

### Setup project

- add `-D__SYNCTEX_WORK__` as other C flag
- also add `-D__SYNCTEX_ZLIB__` as other C flag on systems prior to Sierra
- set Always Search User Path build setting to YES
- in the build phase pane, link to libz1.2.5.tbd (or similar)

You are ready to build
