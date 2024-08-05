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

---@type LuaFileSystem
local lfs = lfs

local pl_dir  = require"pl.dir"
local pl_path = require"pl.path"
local pl_file = require"pl.file"
local read = pl_file.read
local write = pl_file.write

--- @class AUP
local AUP = package.loaded.AUP

local dbg = AUP.dbg

local units = AUP.units
assert(units)

local Engine    = AUP.Engine
local SyncTeX   = AUP.SyncTeX
local KPSEWhich = AUP.KPSEWhich

if dbg:level_get()>0 then
  local result = KPSEWhich():filename('texmf.cnf'):all():run()
  result:print_stdout()
  result = KPSEWhich():var_value('TEXMFROOT'):run()
  print('TEXMFROOT: '..result.stdout)
end
local result = KPSEWhich():filename('hyperref.sty'):run()
assert(string.find(result.stdout, 'hyperref.sty'), "'hyperref.sty' not found")

function AUP.test_library_dump(engines, jobname, content)
  if type(engines) ~= 'table' then
    engines = { engines }
  end
  local p = units:tmp_dir_current()
  AUP.pushd_or_raise(p, 'dump')
  write (jobname..".tex", content)
  for _,engine in ipairs(engines) do
    pl_dir.makepath(engine)
    AUP.pushd_or_raise(engine, 'engine')
    write (jobname..".tex", content)
    result = Engine(engine):synctex(-1):interaction(Engine.InteractionMode.nonstopmode):file(jobname):run()
    if dbg:level_get()>0 then
      result:print()
    end
    local ss = read('%s.synctex'%{jobname})
    if not ss then
      print('No %s.synctex'%{jobname})
      for f in pl_dir.getallfiles(pl_path.currentdir()):sort():iter() do
        print(f)
      end
      ss = read(pl_path.join('..', '%s.synctex'%{jobname}))
      if not ss then
        print(pl_path.join('No ..', '%s.synctex'%{jobname}))
        for f in pl_dir.getallfiles(pl_path.join(assert(lfs.currentdir()), '..')):sort():iter() do
          print(f)
        end
        print(read(jobname..'.log'))
        error('Missing .synctex file')
      end
    end
    print(ss)
    --- @type AUP.Command.Result
    result = SyncTeX.Dump():o(jobname..'.pdf'):run()
    if dbg:level_get()>0 then
      result:print()
    end
    AUP.popd_or_raise('engine')
    print('PWD='..assert(lfs.currentdir()))
    print(pl_path.exists(assert(lfs.currentdir())) and 'YES' or 'NO')
  end
  AUP.popd_or_raise('dump')
end
