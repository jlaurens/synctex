--[==[
Copyright (c) 2024 jerome DOT laurens AT u-bourgogne DOT fr
This file is part of the __SyncTeX__ package testing framework.

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
 
--]==]

local match = string.match

print("Launching the SyncTeX testing framework...")

-- The current directory
local lfs = package.loaded.lfs
local exit = os.exit

local separator = package.config:sub(1,1)

local dir = match(arg[0], "^(.*)/[^/"..separator.."]*$")
--- @type AUP
local AUP
if dir then
  local cwd = lfs.currentdir()
  assert(lfs.chdir(dir),"Cannot chdir to "..(dir or "nil"))
  AUP = require('auplib')
  assert(lfs.chdir(cwd),"Cannot chdir back to "..(cwd or "nil"))
else
  AUP = require('auplib')
end

-- AUP:import_l3build()
-- local proxy = AUP.l3build_proxy
-- assert(proxy)
-- AUP.PL.pretty.write(proxy)

AUP.arguments = AUP.module.arguments.Arguments(arg)
AUP.units = AUP.module.units.Units()
AUP.PL.pretty.write(AUP.units)
AUP.units:check()

local number_of_failures = AUP.units:print_failed()

print("SyncTeX testing DONE")

if number_of_failures>0 then
  exit(1)
end