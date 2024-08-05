--[==[
Copyright (c) 2024 jerome DOT laurens AT u-bourgogne DOT fr
This file is part of the __SyncTeX__ package testing framework.

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
 
--]==]

local pl_class = require"pl.class"

--- @class AUP
local AUP = package.loaded.AUP

--- @class AUP.Build: AUP.Class
local Build = pl_class()

AUP.Build = Build

--[=====[
  # Create build directory:
  mkdir Work && cd Work

  # Do the configure:
  ../configure --disable-all-pkgs --enable-dvipdfm-x \
    -C CFLAGS=-g CXXFLAGS=-g >&outc || echo fail

  # Do the make:
  make >&outm || echo fail

  # Run the tests:
  cd texk/dvipdfm-x
  make check

  # Run the new binary in the build tree, finding support files
  # in a separate tree for a TeX Live release YYYY
  # (Bourne shell syntax):
  TEXMFROOT=/usr/local/texlive/YYYY \
  TEXMFCNF=$TEXMFROOT/texmf-dist/web2c \
  ./xdvipdfmx ...

  Then you can modify source files in 'mydir/texk/dvipdfm-x' and rerun
'make' in 'mydir/Work/texk/dvipdfm-x' to rebuild; that build directory
is where the binary ends up and where you can run a debugger, etc.
--]=====]

function Build.touch_src()

end

function Build.configure_make_gh()

end

return {
  Build = Build,
}
