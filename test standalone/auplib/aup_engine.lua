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
--- @field which fun(self: AUPEngine, file: string): string?
--- @field run fun(file: string): boolean, integer, string, string

local AUPEngine = PL.class.AUPEngine()

local arguments = AUP.arguments

local PATH = nil

--- Initialize an AUPEngine instance
--- @param name string
--- @param synctex_mode integer?
--- @param ... string
function AUPEngine:_init(name, synctex_mode, ...)
  assert_string(2, name)
  self._engine = name
  self._PATH = List()
  local bin_dirs = List({synctex_mode, ...}):filter(
    function(x) return type(x)=='string' and #x>0 end
  )
  if #bin_dirs then
    self._PATH:extend(bin_dirs)
  end
  if type(synctex_mode) == 'number' then
    self:synctex(synctex_mode)
  end
end

--- A simple implementation of the which command.
--- This looks for
--- the given file on the path. On windows, it will assume an extension
--- of `.exe` if no extension is given.
--- rom https://stevedonovan.github.io/Penlight/api/examples/which.lua.html
--- @param file string
--- @return string?
function AUPEngine:which(file)
  assert_string(2, file)
  if PL_path.is_windows and PL_path.extension(file) then
    file = file..'.exe'
  end
  local res = self._PATH:map(PL_path.join, file)
  res = res:filter(PL_path.exists)
  if #res > 0 then return res[1] end
  if not PATH then
    PATH = List()
    if arguments then
      local iterator = arguments:iterator()
      local entry = iterator:next()
      while(entry) do
        if entry.key == 'bin_dir' then
          if  type(entry.value) == 'string' then
            PATH:append(entry.value)
          end
          iterator:consume()
        end
        entry = iterator:next()
      end
    end
    PATH:extend(List.split(os.getenv('PATH'), PL_path.dirsep))
    print(PATH)
    PL.pretty.dump(PATH)
  end
  res = PATH:map(PL_path.join, file)
  res = res:filter(PL_path.exists)
  if #res then return res[1] end
end

--- Set the `--synctex` option.
--- @param value string|number
--- @return AUPEngine
function AUPEngine:synctex(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  --assert_string (2, value)
  self._synctex_mode = "--synctex="..value
  return self
end

--- Set the `--synctex` option.
--- @param value AUPInteractionMode
--- @return AUPEngine
function AUPEngine:interaction(value)
  assert_string(2, value)
  self._interaction = "--interaction="..value
  return self
end

local quote_arg = PL_utils.quote_arg
--- Set the `--synctex` option.
--- @param file string
--- @return boolean status
--- @return integer code
--- @return string stdout
--- @return string errout
function AUPEngine:run(file)
  local cmd = self:which(self._engine)
  if not cmd then
    return false, -1, "", 'Unknown engine '..self._engine
  end
  cmd = quote_arg(List({cmd, self._synctex_mode, self._interaction, file}):filter(function(x)
    return type(x)=='string' and #x>0
  end))
  print(cmd)
  return executeex(cmd)
end

AUPEngine.InteractionMode = AUPInteractionMode

local l = AUPEngine('luatex')

return AUPEngine
