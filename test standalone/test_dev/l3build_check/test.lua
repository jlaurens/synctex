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
local PL = AUP.PL
local List = PL.List

-- exclude directories in next list
local exclude = List({"pdftex"})

AUP_units:test_currentdir(exclude)

local errors = PL.List()
local Arguments = AUP.module.arguments
local arguments = AUP.arguments

local L3Build = AUP.module.l3build

local one_check = function (dirname)
  if not string.match("^%.", dirname) then
    if AUP.pushd(dirname) then
      if PL.path.isfile('build.lua') then
        print("DEBUG CHECKING "..dirname)
        local l3build = L3Build(AUP.L3Build.Target.Check)
        local status, ans, stdout, errout = l3build:run({
          SYNCTEX_TEMP_DIR = AUP_units:get_current_tmp_dir()
        })
        print("status")
        print(status)
        print("ans")
        print(ans)
        print("stdout")
        print(stdout)
        print("errout")
        print(errout)
      end
      AUP.popd()
    end
  end
end

-- l3build check wrapper
if #arguments:get("no_check", Arguments.GetMode.All)==0 then
  local checks = arguments:get("check", Arguments.GetMode.All)
  arguments:consume("check")
  for _,check in ipairs(checks) do
    if check.value == true then
      -- get all the subfolders that have a `build.lua` file and run
      -- the `l3build` command there
      local _, dir_obj = PL.path.dir(".")
      while true do
        local dirname = dir_obj:next()
        if dirname then
          one_check(dirname)
        else
          break
        end
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
