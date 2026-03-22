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

local AUP = package.loaded.AUP

local dbg = AUP.dbg

local AUP_units = AUP.units
assert(AUP_units)

local PL = AUP.PL
local PL_file = PL.file

local read = PL_file.read
local write = PL_file.write
local printf = PL.utils.printf
local AUPEngine = AUP.Engine
local AUPSyncTeX = AUP.SyncTeX

local AUPKPSEWhich = AUP.KPSEWhich
if dbg:level_get()>0 then
  local result = AUPKPSEWhich():filename('texmf.cnf'):all():run()
  result:print_stdout()
  result = AUPKPSEWhich():var_value('TEXMFROOT'):run()
  print('TEXMFROOT: '..result.stdout)
end
local result = AUPKPSEWhich():filename('hyperref.sty'):run()
assert(string.find(result.stdout, 'hyperref.sty'), "'hyperref.sty' not found")

function AUP.test_library_dump(engines, jobname, content)
  if type(engines) ~= 'table' then
    engines = { engines }
  end
  local p = AUP_units:tmp_dir_current()
  AUP.pushd_or_raise(p, 'dump')
  write (jobname..".tex", content)
  for _,engine in ipairs(engines) do
    PL.dir.makepath(engine)
    AUP.pushd_or_raise(engine, 'engine')
    write (jobname..".tex", content)
    result = AUPEngine(engine):synctex(-1):interaction(AUP.Engine.InteractionMode.nonstopmode):file(jobname):run()
    if dbg:level_get()>0 then
      result:print()
    end
    local ss = read('%s.synctex'%{jobname})
    if not ss then
      print('No %s.synctex'%{jobname})
      for f in PL.dir.getallfiles(PL.path.currentdir()):sort():iter() do
        print(f)
      end
      ss = read(PL.path.join('..', '%s.synctex'%{jobname}))
      if not ss then
        print(PL.path.join('No ..', '%s.synctex'%{jobname}))
        for f in PL.dir.getallfiles(PL.path.join(PL.path.currentdir(), '..')):sort():iter() do
          print(f)
        end
        print(read(jobname..'.log'))
        error('Missing .synctex file')
      end
    end
    print(ss)
    result = AUPSyncTeX.Dump():o(jobname..'.pdf'):run()
    if dbg:level_get()>0 then
      result:print()
    end
    AUP.popd_or_raise('engine')
    print('PWD='..PL.path.currentdir())
    print(PL.path.exists(PL.path.currentdir()) and 'YES' or 'NO')
  end
  AUP.popd_or_raise('dump')
end
