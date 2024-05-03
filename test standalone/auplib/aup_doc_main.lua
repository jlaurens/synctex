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

print("Documentation of AUP library...")

-- The current directory
local lfs = package.loaded.lfs

local separator = package.config:sub(1,1)

print(lfs.currentdir())

local dir = match(arg[0], "^(.*)/[^/"..separator.."]*$")
local AUP
if dir then
  print(dir)
  local cwd = lfs.currentdir()
  assert(lfs.chdir(dir))
  assert(lfs.chdir('..'))
  AUP = require('auplib')
  assert(lfs.chdir(cwd))
else
  AUP = require('auplib')
end

local arguments = AUP.module.arguments.Arguments(arg)

local doc = AUP.module.doc.AUPDoc(arguments)
doc:make()

print("Documentation of AUP library... DONE")
