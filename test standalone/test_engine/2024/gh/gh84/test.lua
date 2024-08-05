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

local pl_path     = require"pl.path"
local pl_file     = require"pl.file"
local pl_stringx  = require"pl.stringx"

local join = pl_path.join
local read = pl_file.read
local splitlines = pl_stringx.splitlines


---@class AUP
local AUP = package.loaded.AUP

local dbg = AUP.dbg

dbg:write(1, "Testing mathsurround (2024)")

local FmtUtil = AUP.FmtUtil
local Engine = AUP.Engine
local InteractionMode = Engine.InteractionMode

local units = AUP.units

local cwd = pl_path.currentdir();

local unit = 'gh84'
local unit_tmp = unit..'_tmp'
AUP.pushd_or_raise(units:tmp_dir_current(), unit_tmp)
for name in Engine.tex_all() do
  local base = unit..'_'..name
  local source = join(cwd, base..".tex")
  local engine = Engine(name):synctex(-1):interaction(InteractionMode.nonstopmode):file(source)
  local result = engine:run()
  assert(result.status)
  for _,l in ipairs(splitlines(result.stdout)) do
    if l:match("(Fatal format file error; I'm stymied)") then
      print("Bad format")
      result = FmtUtil():sys():byengine(name):remake_when_older()
      assert(result.status)
      result:print()
      result = engine:run()
      assert(result.status)
    end
  end
  result:print_stdout()
  AUP.br{}
  for _, l in ipairs(splitlines(result.stdout)) do
    if l:match('/SyncTeX:') then
      print("**** WARNING for next line\n"..l)
    end
  end
  local base_synctex = base..'.synctex'
  local s = read(base_synctex)
  local function f(...)
    for _,request in ipairs({...}) do
      if not s:find(request) then
        units:fail("Wrong "..base_synctex.." (no '"..request.."')")
        print(s)
        return
      end
    end
  end
  local request
  if not s then
    units:fail("No .synctex available (cmd: %s)"%{engine:cmd()})
  elseif name:find("lua") then
    f("$1,7:1111,", "$1,18:1111,")
  else
    f("$1,7:1111,")
  end
end
AUP.popd(unit_tmp)
