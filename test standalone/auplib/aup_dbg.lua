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

local io_write = io.write

local AUP = package.loaded.AUP
local PL = AUP.PL
local printf = PL.utils.printf

--- @class (exact) AUPDBG debugging message
--- @field write fun(self: AUPDBG, level: integer, ...: unknown)
--- @field format fun(self: AUPDBG, level: integer, format: string, env: table?)
--- @field printf fun(self: AUPDBG, level: integer, format: string, ...: unknown)
--- @field level_get fun(self: AUPDBG): integer
--- @field level_set fun(self: AUPDBG, value: integer): integer
--- @field level_increment fun(self: AUPDBG, delta: integer): integer

local AUPDBG = PL.class.AUPDBG()

function AUPDBG:_init()
  self._level = 0
end

--- Print debuggging information.
--- Then level is positive, extra debugging information is
--- displayed. The higher the level, the richer the information.
--- @param level integer
--- @param ... unknown
function AUPDBG:write(level, ...)
  if level <= self._level then
    local sep = ''
    local endl = ''
    local t = {...}
    for _,v in ipairs(t) do
      io_write(sep)
      sep = ','
      endl = '\n'
      local ans = pcall(function() io_write(''..v) end)
      if not ans then
        pcall(function() io_write(v) end)
      end
    end
    io_write(endl)
  end
end

--- Print debuggging information.
--- then level is positive, extra debugging information is
--- displayed. The higher the level, the richer the information.
--- @param level integer
--- @param template string
--- @param env table?
function AUPDBG:format(level, template, env)
  if level <= self._level then
    local ans, error, code = PL.template.substitute(template, env)
    if ans then print(ans) else print(error) end
    if code then print(code) end
  end
end

--- Print debuggging formatted information.
--- then level is positive, extra debugging information is
--- displayed. The higher the level, the richer the information.
--- @param level integer
--- @param format string
--- @param ... unknown
function AUPDBG:printf(level, format, ...)
  --local arg = table.pack(...)
  if level <= self._level then
    printf(format, ...)
  end
end

--- Returns the debug level
--- @return integer
function AUPDBG:level_get()
  return self._level
end

--- Set the debug level
--- @param level integer|string
function AUPDBG:level_set(level)
  if type(level) ~= 'number' then
    level = tonumber(level) or 0
  end
  self._level = level >= 0 and level or 0
  return self._level
end

--- Increment the debug level
--- @param delta integer|string
function AUPDBG:level_increment(delta)
  if type(delta) ~= 'number' then
    delta = tonumber(delta) or 1
  end
  if delta > 0 then
    self._level = self._level + delta
  end
  return self._level
end

return AUPDBG
