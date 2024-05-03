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

--[=====[

--]=====]

local AUP = package.loaded.AUP
local AUPCommand = AUP.module.Command
local PL = AUP.PL

-- loading the configuration file

if PL.path.exists("config-defaults.lua") then
  dofile("config-defaults.lua")
end
local arguments = AUP.arguments
assert(arguments)
local entry = arguments:get('dev_mode')
print(entry)
if entry then
  local p = "config-%s.lua"%{entry.value}
  if PL.path.exists(p) then
    dofile(p)
  else
    error("Bad dev_mode: %s"%{entry.value})
  end
end

for _,engine in ipairs({
  'euptex', 'pdftex', 'xetex', 'luatex', 'luahbtex', 'luajittex'
}) do
  print(engine)
  local p = AUPCommand.which(engine)
  local attr = PL.path.link_attrib(p)
end