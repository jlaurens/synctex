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

local pl_seq  = require"pl.seq"
local pl_dir  = require"pl.dir"
local pl_file = require"pl.file"

local read = pl_file.read
local write = pl_file.write

--- @type AUP
local AUP = package.loaded.AUP

local dbg = AUP.dbg

local units = AUP.units
assert(units)

local Engine = AUP.Engine

local p = units:tmp_dir_current()
local jobname = "many"
AUP.pushd_or_raise(p, jobname)
write (jobname..".tex", [[
  a
  \input ../%s1
  b
  \input ../%s2
  c
  \input ../%s3
  d
  \input ../%s4
  e
  \bye
]]%{jobname, jobname, jobname, jobname})
for n in pl_seq.list{'1', '2', '3', '4'} do
  write ("%s%s.tex"%{jobname, n}, [[
    %s
  ]]%{n})
end
local engines = {"pdftex"}
for engine in pl_seq.list(engines) do
  pl_dir.makepath(engine)
  AUP.pushd_or_raise(engine, 'engine')
  for f in pl_dir.getallfiles():sort():iter() do
    print(f)
  end
  local result = Engine(engine):synctex(-1):interaction(AUP.Engine.InteractionMode.nonstopmode):file("../%s"%{jobname}):run()
  if dbg:level_get()>0 then
    result:print()
  end
  local ss = read('%s.synctex'%{jobname})
  if not ss then
    print('***** No file at %s.synctex'%{jobname})
    ss = read('../%s.synctex'%{jobname})
    if not ss then
      print('***** No file at ../%s.synctex'%{jobname})
      -- we need hyperref.sty but it might not be available when using the TL development setup
      AUP.br{label=jobname}
      ss = read('%s.log'%{jobname})
      print(ss)
      AUP.br{}
      print('cwd: %s'%{pl_dir.currentdir()})
      for f in pl_dir.getallfiles():sort():iter() do
        print(f)
      end
      error('Maybe missing `TEXMFROOT` or `TEXMFCNF` environment variables.')
    end
  end
  AUP.open_file('%s.pdf'%{jobname})
  AUP.popd_or_raise('engine')
end
AUP.popd_or_raise(jobname)
