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
local mkdir = PL.path.mkdir
local AUPEngine = AUP.module.engine

local kpse = package.loaded.kpse
kpse.set_program_name('kpsewhich')
local lookup = kpse.lookup

local where = lookup('hyperref.sty')
print('hyperref.sty: '..where)
assert(where)

function AUP.test_library_mark(engines, jobname, content)
  if type(engines) ~= 'table' then
    engines = { engines }
  end
  local p = AUP_units:get_current_tmp_dir()
  AUP.pushd_or_raise(p)
  write (jobname..".tex", content)
  for _,engine in ipairs(engines) do
    mkdir(engine)
    AUP.pushd_or_raise(engine)
    local s, c, o, e = AUPEngine(engine):synctex(-1):interaction(AUP.Engine.InteractionMode.nonstopmode):file("../"..jobname):run()
    dbg:printf(1, "%s/%i/%s/%s\n", s and "T" or "F", c or 0, o, e)
    local ss = read(jobname..'.synctex')
    if not ss then
      -- we need hyperref.sty but it might not be available when using the TL development setup
      ss = read(jobname..'.log')
      print(ss)
      print("This test needs `hyperref.sty`")
      error('Maybe missing `TEXMFROOT` or `TEXMFCNF` environment variables.')
    end
    PL.utils.executeex("open "..jobname..'.pdf')
    -- print(ss)
    -- s, c, o, e = AUPSyncTeX.Dump():o(jobname..'.pdf'):run()
    -- dbg:printf(1, "%s/%i\n------ stdout ------\n%s\n------ stderr ------\n%s\n--------------------\n", s and "T" or "F", c or 0, o, e)
    AUP.popd()
  end
  AUP.popd()
end
