--[===[
Copyright (c) 2008-2024 jerome DOT laurens AT u-bourgogne DOT fr
This file is part of the __SyncTeX__ package testing facilities.

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
 
]===]

local AUP = package.loaded.AUP

local dbg = AUP.dbg

dbg:write(1, "Testing mathsurround (2024)")

local PL = AUP.PL
local PL_file = PL.file
local PL_utils = PL.utils
local PL_stringx = PL.stringx

local AUPEngine = AUP.Engine
local InteractionMode = AUPEngine.InteractionMode

local match = string.match
local join = PL.path.join
local read = PL_file.read
local splitlines = PL_stringx.splitlines

local units = AUP.units

local cwd = PL.path.currentdir();

local unit = 'gh30+84'
local unit_tmp = unit..'_tmp'
AUP.pushd_or_raise(units:tmp_dir_current(), unit_tmp)
for name in AUPEngine.tex_all() do
  local base = unit..'_'..name
  local source = join(cwd, base..".tex")
  local engine = AUPEngine(name):synctex(-1):interaction(InteractionMode.nonstopmode):file(source)
  local result = engine:run()
  assert(result.status)
  for _,l in ipairs(splitlines(result.stdout)) do
    if match(l, "(Fatal format file error; I'm stymied)") then
      print("Bad format")
      local AUPFmtUtil = AUP.FmtUtil
      result = AUPFmtUtil():sys():byengine(name):remake_when_older()
      assert(result.status)
      result:print()
      result = engine:run()
      assert(result.status)
    end
  end
  result:print_stdout()
  AUP.br{}
  for _, l in ipairs(splitlines(result.stdout)) do
    if string.match(l, '/SyncTeX:') then
      print("**** WARNING for next line\n"..l)
    end
  end
  local base_synctex = base..'.synctex'
  local s = read(base_synctex)
  if not s then
    units:fail("No .synctex available (cmd: %s)"%{engine:cmd()})
  elseif string.find(name, "lua") then
    if not string.find(s, "$1,7:1000,") then
      units:fail("Wrong "..base_synctex.." (no '$1,7:1000,')")
      print(s)
    elseif not string.find(s, "$1,18:1000,") then
      units:fail("Wrong "..base_synctex.." (no '$1,18:1000,')")
      print(s)
    end
  elseif not string.find(s, "$1,7:1000,") then
    units:fail("Wrong "..base_synctex.." (no '$1,7:1000,')")
    print(s)
  end
end
AUP.popd(unit_tmp)
