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
 
 Version 1.2
 Thu Jun 19 09:39:21 UTC 2008
 
 History:
 --------
 
 - the -d option for an input directory
 
 Important notice:
 -----------------
 This file is named "synctex_main.c".
 This is the command line interface to the synctex_parser.c.
 */

#   ifdef __linux__
#       define _ISOC99_SOURCE /* to get the fmax() prototype */
#   endif

#   ifdef __SYNCTEX_WORK__
#       include "synctex_parser_c-auto.h"
#   else
#       include <w2c/c-auto.h>
/*      for inline && HAVE_xxx */
#   endif

#   include <stdlib.h>
#   include <stdio.h>
#   include <string.h>
#   include <stdarg.h>
#   include <math.h>
#   include "synctex_version.h"
#   include "synctex_main_commands.h"
#   include "synctex_main_help.h"
#   include "synctex_parser_advanced.h"
#   include "synctex_parser_utils.h"

/*  The code below uses strlcat and strlcpy, which avoids security warnings with some compilers.
    However, if these are not available we simply use the old, unchecked versions;
    this is OK because all the uses in this code are working with a buffer that's been
    allocated based on measuring the strings involved.
 */
#ifndef HAVE_STRLCAT
#   define strlcat(dst, src, size) strcat((dst), (src))
#endif
#ifndef HAVE_STRLCPY
#   define strlcpy(dst, src, size) strcpy((dst), (src))
#endif
#ifndef HAVE_FMAX
#   define fmax my_fmax
inline static double my_fmax(double x, double y) { return (x < y) ? y : x; }
#endif

#ifdef WIN32
#   define snprintf _snprintf
#endif

#if SYNCTEX_DEBUG
#   ifdef WIN32
#       include <direct.h>
#       define getcwd _getcwd
#   else
#       include <unistd.h>
#   endif
#endif

int main(int argc, char *argv[]);


int main(int argc, char *argv[])
{
    int arg_index = 1;
    printf("This is SyncTeX command line utility, version " SYNCTEX_CLI_VERSION_STRING "\n");
    if(arg_index<argc) {
        if(0==strcmp("help",argv[arg_index])) {
            if(++arg_index<argc) {
                if(0==strcmp("view",argv[arg_index])) {
                    synctex_help_view(NULL);
                    return 0;
                } else if(0==strcmp("edit",argv[arg_index])) {
                    synctex_help_edit(NULL);
                    return 0;
                } else if(0==strcmp("update",argv[arg_index])) {
                    synctex_help_update(NULL);
                    return 0;
                }
            }
            synctex_help(NULL);
            return 0;
        } else if(0==strcmp("view",argv[arg_index])) {
            return synctex_view(argc-arg_index-1,argv+arg_index+1);
        } else if(0==strcmp("edit",argv[arg_index])) {
            return synctex_edit(argc-arg_index-1,argv+arg_index+1);
        } else if(0==strcmp("update",argv[arg_index])) {
            return synctex_update(argc-arg_index-1,argv+arg_index+1);
        } else if(0==strcmp("test",argv[arg_index])) {
            return synctex_test(argc-arg_index-1,argv+arg_index+1);
        }
    }
    synctex_help("Missing options");
    return 0;
}

