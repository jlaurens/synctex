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
 This file is named "synctex_commands.c".
 This was part of synctex_main.c but has been separated fort testing purposes.
 */

#ifndef __SYNCTEX_VIEW__
#define __SYNCTEX_VIEW__

#ifdef __cplusplus
extern "C" {
#endif
    
int synctex_view(int argc, char *argv[]);
int synctex_edit(int argc, char *argv[]);
int synctex_update(int argc, char *argv[]);
int synctex_test(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif /* __SYNCTEX_VIEW__ */
