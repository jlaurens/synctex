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

--- @type AUP
local AUP = package.loaded.AUP
local PL = AUP.PL

local List = PL.List
local PL_path = PL.path
local PL_utils = PL.utils
local assert_string = PL_utils.assert_string
local executeex = PL_utils.executeex

---  simple implementation of the which command.
--- his looks for
--- he given file on the path. On windows, it will assume an extension
--- f `.exe` if no extension is given.
--- rom https://stevedonovan.github.io/Penlight/api/examples/which.lua.html
--- @param file string
--- @return string?
local function which(file)
  assert_string(1, file)
  if PL_path.extension(file) == '' and PL_path.is_windows then
    file = file..'.exe'
  end
  local path = List.split(os.getenv('PATH'), PL_path.dirsep)
  local res = path:map(PL_path.join, file)
  res = res:filter(PL_path.exists)
  if res then return res[1] end
end

--- @enum (key) AUPInteractionMode
local AUPInteractionMode = {
  batchmode   = 'batchmode',
  nonstopmode = 'nonstopmode',
  scrollmode  = 'scrollmode',
  errorstopmode = 'errorstopmode'
}

--- @class AUPEngine
--- @field _init fun(self: AUPEngine, name: string)
--- @field synctex fun(self: AUPEngine, value: integer): AUPEngine
--- @field interaction fun(self: AUPEngine, value: AUPInteractionMode): AUPEngine
--- @field run fun(file: string): boolean, integer, string, string

local AUPEngine = PL.class.AUPEngine()

--- nitialize an AUPEngine instance
--- @param name string
function AUPEngine:_init(name)
  assert_string(1, name)
  self._engine = name
end

--- et the `--synctex` option.
--- @param value string|number
--- @return AUPEngine
function AUPEngine:synctex(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  --assert_string (2, value)
  self._synctex = "--synctex="..value
  return self
end

--- et the `--synctex` option.
--- @param value AUPInteractionMode
--- @return AUPEngine
function AUPEngine:interaction(value)
  assert_string(2, value)
  self._interaction = "--interaction="..value
  return self
end

local quote_arg = PL_utils.quote_arg
--- et the `--synctex` option.
--- @param file string
--- @return boolean status
--- @return integer code
--- @return string stdout
--- @return string errout
function AUPEngine:run(file)
  local cmd = which(self._engine)
  if not cmd then
    return false, -1, "", 'Unknown engine '..self._engine
  end
  cmd = quote_arg(List({cmd, self._synctex, self._interaction, file}):filter(function(x) return #x>0 end))
  print(cmd)
  return executeex(cmd)
end

AUPEngine.InteractionMode = AUPInteractionMode

local l = AUPEngine('luatex')

return AUPEngine
