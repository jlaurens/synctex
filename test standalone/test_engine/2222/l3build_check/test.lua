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
local dbg = AUP.dbg
local units = AUP.units

local List    = require"pl.List"
local pl_path = require"pl.path"

-- exclude directories in next list
local exclude = List({"pdftex"})

units:test_currentdir(exclude)

local arguments = assert(AUP.arguments)

local L3Build = AUP.L3Build

local one_check = function (dirname)
  if not string.match("^%.", dirname) then
    if AUP.pushd(dirname, 'one_check') then
      if pl_path.isfile('build.lua') then
        print("DEBUG CHECKING "..dirname)
        local l3build = L3Build(AUP.L3Build.Target.Check)
        local result = l3build:run({
          SYNCTEX_TEMP_DIR = units:tmp_dir_current()
        })
        result:assert_success('l3build run error')
        result:print()
      end
      AUP.popd('one_check')
    end
  end
end

-- l3build check wrapper
if #arguments:get("no_check", AUP.Arguments.GetMode.All)==0 then
  local checks = arguments:get("check", AUP.Arguments.GetMode.All)
  arguments:consume("check")
  for _,check in ipairs(assert(checks)) do
    if check.value == true then
      -- get all the subfolders that have a `build.lua` file and run
      -- the `l3build` command there
      for dirname in pl_path.dir(".") do
        one_check(dirname)
      end
    else
      one_check(check.value)
    end
    check:consume()
  end
else
  dbg:write(10,"No l3build check")
end
arguments:consume("no_check")
