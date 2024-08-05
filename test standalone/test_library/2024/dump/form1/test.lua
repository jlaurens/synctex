--[[
Copyright (c) 2024 jerome DOT laurens AT u-bourgogne DOT fr
This file is a bridge to the __SyncTeX__ package testing framework.

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
 
--]]

--- @type AUP
local AUP = package.loaded.AUP

AUP.test_library_dump('pdflatex', 'form1', [==[
\documentclass{article}
\usepackage{hyperref}
\begin{document}
\begin{Form}[action={http://your-web-server.com/path/receiveform.cgi}]
  \CheckBox[width=1em]{x}
\end{Form}
\end{document}
]==])

--[[
---@type LuaFileSystem
local lfs = lfs

local kpse = package.loaded.kpse
kpse.set_program_name('kpsewhich')
local lookup = kpse.lookup

local dbg = AUP.dbg

print(lfs.currentdir())
print(AUP._VERSION)
print(AUP._DESCRIPTION)

local units = AUP.units
local exclude = {}
units:test_currentdir(exclude)

local pl_file = require "pl.file"
local pl_utils = require "pl.utils"

local write = pl_file.write
local read = pl_file.read
local printf = pl_utils.printf
local Engine = AUP.Engine
local SyncTeX = AUP.SyncTeX

local units = AUP.units
assert(units)
local my_path = units:tmp_dir_current()
AUP.pushd_or_raise(my_path, '')
write ("dump.tex", [==[
\documentclass{article}
\usepackage{hyperref}
\begin{document}
\begin{Form}[action={http://your-web-server.com/path/receiveform.cgi}]
  \CheckBox[width=1em]{x}
\end{Form}
\end{document}
]==])
local runner = Engine('pdflatex'):synctex(-1):interaction(AUP.Engine.InteractionMode.nonstopmode):file('dump.tex')
local result = runner:run()
if dbg:level_get()>0 then
  result:print()
end
local ss = read('dump.synctex')
if not ss then
  -- we need hyperref.sty but it might not be available when using the TL development setup
  ss = read('dump.log')
  print(ss)
  print("This test needs `hyperref.sty`")
  error('Maybe a missing TEXMFROOT, TEXMFCNT')
  for _,x in ipairs({
    'hyperref.sty',
    'iftex.sty',
    'keyval.sty',
    'kvsetkeys.sty',
    'kvdefinekeys.sty',
    'pdfescape.sty',
    'ltxcmds.sty',
    'pdftexcmds.sty',
    'infwarerr.sty',
    'hycolor.sty',
    'auxhook.sty',
    'nameref.sty',
    'refcount.sty',
    'gettitlestring.sty',
    'kvoptions.sty',
    'etoolbox.sty',
    'pd1enc.def',
    'intcalc.sty',
    'puenc.def',
    'url.sty',
    'bitset.sty',
    'bigintcalc.sty',
    'atbegshi-ltx.sty',
    'hpdftex.def',
    'atveryend-ltx.sty',
    'rerunfilecheck.sty',
    'uniquecounter.sty',
    'l3backend-pdftex.def',
  }) do
    local p = lookup(x)
    assert(p)
    lfs.link (p, x, true)
  end
  result = runner:run()
  if dbg:level_get()>0 then
    result:print()
    ss = read('dump.log')
    print(ss)
  end
  ss = read('dump.synctex')
  if not ss then
    error("Error: NO dump.synctex")
  end
  print(ss)
end
print(ss)
sresult = SyncTeX.Dump():o('dump.pdf'):run()
if dbg:level_get()>0 then
  result:print()
end
AUP.popd('')
]]
