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

---@type LuaFileSystem
local lfs = lfs

local pl_path     = require"pl.path"
local pl_file     = require"pl.file"
local pl_utils    = require"pl.utils"
local pl_stringx  = require"pl.stringx"

local match = string.match
local join = pl_path.join
local read = pl_file.read
local splitlines = pl_stringx.splitlines
local printf = pl_utils.printf

--- @class AUP
local AUP = package.loaded.AUP

local dbg = AUP.dbg

dbg:write(1, "Testing mathsurround")

local Command = AUP.Command
local Engine = AUP.Engine
local InteractionMode = Engine.InteractionMode

local units = AUP.units

local cwd = assert(lfs.currentdir());

local unit = 'gh87'
AUP.pushd_or_raise(units:tmp_dir_current(), 'tmp_'..unit)
for name in Engine.tex_all() do
  AUP.br{label='ENGINE: '..name}
  local base = unit..'_'..name
  local source = join(cwd, base..".tex")
  dbg:write(1, Command.which(name, Command.tex_bin_get(), true))
  local engine = Engine(name):synctex(-1):interaction(InteractionMode.nonstopmode):file(source)
  local result = engine:run()
  assert(result.status, 'cmd: %s'%{engine:cmd()})
  for i,l in ipairs(splitlines(result.stdout)) do
    if l:match("Synchronize ERROR") then
      printf("Unexpected at line %i: <%s>\n", i, l)
    end
  end
  result:print()
  local base_synctex = base..'.synctex'
  local s = read(base_synctex)
  if s ~= nil then
    AUP.br{label=base_synctex}
    print(s)
    local request = '$1,8:0,'
    local i = s:find(request)
    if i==nil then
      units:fail('Wrong '..base_synctex..' (no "'..request..'")')
      print(s)
    else
      request = '$1,8:1111,'
      i = s:find(request)
      if i==nil then
        units:fail('Wrong '..base_synctex..' (no "'..request..'")')
        print(s)
      end
    end
  else
    units:fail('dev: MISSING '..base_synctex)
  end
end
AUP.popd_or_raise('tmp_'..unit)
