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

---@class AUP
local AUP = package.loaded.AUP

local pl_template = require"pl.template"
local pl_class = require"pl.class"
local pl_utils = require"pl.utils"
local printf = pl_utils.printf

--- @class AUP.DBG: AUP.Class debugging message
local DBG = pl_class()

AUP.DBG = DBG

function DBG:_init()
  self._level = 0
end

--- Print debuggging information.
--- Then level is positive, extra debugging information is
--- displayed. The higher the level, the richer the information.
--- @param level integer
--- @param ... unknown
function DBG:write(level, ...)
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
function DBG:format(level, template, env)
  if level <= self._level then
    local ans, error, code = pl_template.substitute(template, env)
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
function DBG:printf(level, format, ...)
  --local arg = table.pack(...)
  if level <= self._level then
    printf(format, ...)
  end
end

--- Returns the debug level
--- @return integer
function DBG:level_get()
  return self._level
end

--- Set the debug level
---
--- Usefull to display more debugging messages.
--- @param level integer|string
function DBG:level_set(level)
  if type(level) ~= 'number' then
    level = tonumber(level) or 0
  end
  self._level = level >= 0 and level or 0
  return self._level
end

--- Increment the debug level
--- @param delta integer|string
function DBG:level_increment(delta)
  if type(delta) ~= 'number' then
    delta = tonumber(delta) or 1
  end
  if delta > 0 then
    self._level = self._level + delta
  end
  return self._level
end

--- Wrapper over `assert`
--- 
--- Prints the traceback in the message.
--- @generic T
--- @param v? T
--- @param message? string|any
--- @param level? integer|any
--- @param ... any
--- @return T
--- @return string?
--- @return any ...
function DBG:assert(v, message, level, ...)
  return assert(v, debug.traceback(message, level or 2), v)
end


return {
  DBG = DBG
}
