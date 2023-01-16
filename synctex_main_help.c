/*
 Copyright (c) 2008-2023 jerome DOT laurens AT u-bourgogne DOT fr
 
 This file is part of the __SyncTeX__ package.
 
 [//]: # (Latest Revision: Mon Jan  2 11:25:09 UTC 2023)
 [//]: # (Version: 1.23 / 3.0.0)
 
 See `synctex_parser_readme.md` for more details
 
 ## License
 
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE
 
 Except as contained in this notice, the name of the copyright holder
 shall not be used in advertising or otherwise to promote the sale,
 use or other dealings in this Software without prior written
 authorization from the copyright holder.
 
 Acknowledgments:
 ----------------
 The author received useful remarks from the pdfTeX developers, especially Hahn The Thanh,
 and significant help from XeTeX developer Jonathan Kew
 
 Nota Bene:
 ----------
 If you include or use a significant part of the synctex package into a software,
 I would appreciate to be listed as contributor and see "SyncTeX" highlighted.
 
 Version 1.23
 Thu Jun 19 09:39:21 UTC 2008
 
 Important notice:
 -----------------
 This file is named "synctex_main_commands.c".
 This was part of synctex_main.c but has been separated fort testing purposes.
 */

#   include <stdlib.h>
#   include <stdio.h>
#   include <string.h>
#   include <stdarg.h>
#   include <math.h>
#   include "synctex_config.h"
#   include "synctex_main_commands.h"
#   include "synctex_parser_advanced.h"
#   include "synctex_parser_utils.h"

static void synctex_usage(const char * error,va_list ap) {
    if(error) {
        fprintf(stderr,"SyncTeX ERROR: ");
        vfprintf(stderr,error,ap);
        fprintf(stderr,"\n");
    }
    fprintf((error?stderr:stdout),
            "usage: synctex <subcommand> [options] [args]\n"
            "Synchronize TeXnology command-line client, version " SYNCTEX_CLI_VERSION_STRING "\n\n"
            "The Synchronization TeXnology by Jérôme Laurens is a rather new feature of recent TeX engines.\n"
            "It allows to synchronize between input and output, which means to\n"
            "navigate from the source document to the typeset material and vice versa.\n"
            "SyncTeX version is " SYNCTEX_VERSION_STRING ".\n\n"
            );
    return;
}

void synctex_help(const char * error,...) {
    va_list v;
    va_start(v, error);
    synctex_usage(error, v);
    va_end(v);
    fprintf((error?stderr:stdout),
            "Available subcommands:\n"
            "   view     to perform forwards synchronization\n"
            "   edit     to perform backwards synchronization\n"
            "   update   to update a synctex file after a dvi/xdv to pdf filter\n"
            "   help     this help\n\n"
            "Type 'synctex help <subcommand>' for help on a specific subcommand.\n"
            "There is also an undocumented test subcommand.\n"
            );
    return;
}

void synctex_help_view(const char * error,...) {
    va_list v;
    va_start(v, error);
    synctex_usage(error, v);
    va_end(v);
    fputs("synctex view: forwards or direct synchronization,\n"
          "command sent by the editor to view the output corresponding to the position under the mouse\n"
          "\n"
          "usage: synctex view -i line:column:[page_hint:]input -o output [-d directory] [-x viewer-command] [-h before/offset:middle/after]\n"
          "\n"
          "-i line:column:[page_hint:]input\n"
          "       specify the line, column, optional page hint and input file.\n"
          "       The line and column are 1 based integers,\n"
          "       they allow to identify every character in a file.\n"
          "       column is the offset of a character relative to the containing line.\n"
          "       Pass 0 if this information is not relevant.\n"
          "       page_hint is the currently displayed page number.\n"
          "       If there is an answer on that page, it will be returned.\n"
          "       Pass 0 if this information is not available to you.\n"
          "       input is either the name of the main source file or an included document.\n"
          "       It must be the very name as understood by TeX, id est the name exactly as it appears in the log file.\n"
          "       It does not matter if the file actually exists or not, except that the command is not really useful.\n"
          "       \n"
          "-o output\n"
          "       is the full or relative path of the output file (with any relevant path extension).\n"
          "       This file must exist.\n"
          "       \n"
          "-d directory\n"
          "       is the directory containing the synctex file, in case it is different from the directory of the output.\n"
          "       This directory must exist.\n"
          "       An example will explain how things work: for synctex -o ...:bar.tex -d foo,\n"
          "       the chosen synctex file is the most recent among bar.synctex, bar.synctex.gz, foo/bar.synctex and foo/bar.synctex.gz.\n"
          "        The other ones are simply removed, if the authorization is granted\n"
          "       \n"
          "-x viewer-command\n"
          "       Normally the synctex tool outputs its result to the stdout.\n"
          "       It is possible to launch an external tool with the result.\n"
          "       The viewer-command is a printf like format string with following specifiers.\n"
          "       %{output} is the name specifier of the main document, without path extension.\n"
          "       %{page} is the 0 based page number specifier, %{page+1} is the 1 based page number specifier.\n"
          "       To synchronize by point, %{x} is the x coordinate specifier, %{y} is the y coordinate specifier,\n"
          "       both in dots and relative to the top left corner of the page.\n"
          "       To synchronize by box,\n"
          "       %{h} is the horizontal coordinate specifier of the origin of the enclosing box,\n"
          "       %{v} is the vertical coordinate specifier of the origin of the enclosing box,\n"
          "       both in dots and relative to the upper left corner of the page.\n"
          "       They may be different from the preceding pair of coordinates.\n"
          "       %{width} is the width specifier, %{height} is the height specifier of the enclosing box.\n"
          "       The latter dimension is naturally counted from bottom to top.\n"
          "       There is no notion of depth for such a box.\n"
          "       To synchronize by content, %{before} is the word before,\n"
          "       %{offset} is the offset specifier, %{middle} is the middle word, and %{after} is the word after.\n"
          "\n"
          "       If no viewer command is provided, the content of the SYNCTEX_VIEWER environment variable is used instead.\n"
          "\n"
          "-h before/offset:middle/after\n"
          "       This hint allows a forwards synchronization by contents.\n"
          "       Instead of giving a character offset in a line, you can give full words.\n"
          "       A full word is a sequence of characters (excepting '/').\n"
          "       You will choose full words in the source document that will certainly appear unaltered in the output.\n"
          "       The \"middle\" word contains the character under the mouse at position offset.\n"
          "       \"before\" is a full word preceding middle and \"after\" is following it.\n"
          "       The before or after word can be missing, they are then considered as void strings.\n"
          "       \n"
          "The result is a list of records. In general the first one is the most accurate but\n"
          "it is the responsibility of the client to decide which one best fits the user needs.\n",
          (error?stderr:stdout)
          );
    return;
}

void synctex_help_edit(const char * error,...) {
    va_list v;
    va_start(v, error);
    synctex_usage(error, v);
    va_end(v);
    fputs(
          "synctex edit: backwards or reverse synchronization,\n"
          "command sent by the viewer to edit the source corresponding to the position under the mouse\n\n"
          "\n"
          "usage: synctex edit -o page:x:y:output [-d directory] [-x editor-command] [-h offset:context]\n"
          "\n"
          "-o page:x:y:output\n"
          "       specify the page and coordinates of the point under the mouse.\n"
          "       page is 1 based.\n"
          "       Coordinates x and y are counted from the top left corner of the page.\n"
          "       Their unit is the big point (72 dpi).\n"
          "       \n"
          "       output is in general the path of a pdf or dvi file.\n"
          "       It can be either absolute or relative to the current directory.\n"
          "       This named file must always exist.\n"
          "       \n"
          "-d directory\n"
          "       is the directory containing the synctex file, in case it is different from the directory of the output file.\n"
          "       This directory must exist.\n"
          "       An example will explain how things work: for synctex -o ...:bar.tex -d foo,\n"
          "       the chosen synctex file is the most recent among bar.synctex, bar.synctex.gz, foo/bar.synctex and foo/bar.synctex.gz.\n"
          "        The other ones are simply removed, if the authorization is granted.\n"
          "       \n"
          "-x editor-command\n"
          "       Normally the synctex tool outputs its result to the stdout.\n"
          "       It is possible to execute an external tool with the result of the query.\n"
          "       The editor-command is a printf like format string with following specifiers.\n"
          "       They will be replaced by their value before the command is executed.\n"
          "       %{output} is the full path specifier of the output document, with no extension.\n"
          "       %{input} is the name specifier of the input document.\n"
          "       %{line} is the 0 based line number specifier. %{line+1} is the 1 based line number specifier.\n"
          "       %{column} is the 0 based column number specifier or -1. %{column+1} is the 1 based column number or -1.\n"
          "       %{offset} is the 0 based offset specifier and %{context} is the context specifier of the hint.\n"
          "       \n"
          "       If no editor-command is provided, the content of the SYNCTEX_EDITOR environment variable is used instead.\n"
          "       \n"
          "-h offset:context\n"
          "       This hint allows a backwards or reverse synchronization by contents.\n"
          "       You give a context including the character at the mouse location, and\n"
          "       the offset of this character relative to the beginning of this bunch of text.\n"
          "       \n",
          (error?stderr:stdout)
          );
    return;
}

void synctex_help_update(const char * error,...) {
    va_list v;
    va_start(v, error);
    synctex_usage(error, v);
    va_end(v);
    fputs(
          "synctex update: up to date synctex file,\n"
          "Use this command to update the synctex file once a dvi/xdv to pdf filter is applied.\n"
          "\n"
          "usage: synctex update -o output [-d directory] [-m number] [-x dimension] [-y dimension]\n"
          "\n"
          "-o output     is the full or relative path of an existing file,\n"
          "              either the real synctex file you wish to update\n"
          "              or a related file: foo.tex, foo.pdf, foo.dvi...\n"
          "-d directory  is the directory containing the synctex file, in case it is different from the directory of the output.\n"
          "              This directory must exist.\n"
          "              An example will explain how things work: for synctex -o ...:bar.tex -d foo,\n"
          "              the chosen synctex file is the most recent among bar.synctex, bar.synctex.gz, foo/bar.synctex and foo/bar.synctex.gz.\n"
          "              The other ones are simply removed, if the authorization is granted\n"
          "              \n"
          "-m number     Set additional magnification\n"
          "-x dimension  Set horizontal offset\n"
          "-y dimension  Set vertical offset\n"
          "In general, these are exactly the same options provided to the dvi/xdv to pdf filter.\n",
          (error?stderr:stdout)
          );
    return;
}

