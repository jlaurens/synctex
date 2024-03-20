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

--- @source

local match = string.match
local print = print

local AUP = package.loaded.AUP
local dbg = AUP.dbg

local PL = AUP.PL
local path = PL.path
local class = PL.class

local lpeg = package.loaded.lpeg -- built into texlua

--- @class (exact) AUPArgumentEntry
--- @field _init fun(self: AUPArgumentEntry, key: string, value: string|true)
--- @field key string the `⟨key⟩` in `--test-⟨key⟩=⟨value⟩` or `--test-⟨key⟩`
--- @field value string|true the `⟨value⟩` in `--test-⟨key⟩=⟨value⟩` or `true` for `--test-⟨key⟩`
local AUPArgumentEntry = class.AUPArgumentEntry()

--- nitialize a new argument entry instance
--- @param key string
--- @param value string|true
function AUPArgumentEntry:_init(key, value)
  self.key = key
  if type(value) == 'string' then
    local m = match(value, '^"(.*)"$')
    if m then
      value = m
    end
  end
  self.value = value
end

function AUPArgumentEntry:__tostring()
  return "AUPArgumentEntry: "..self.key..' -> '..(self.value == true and 'true' or self.value)
end

--- @class (exact) AUPArguments
--- @field _init fun(self: AUPArguments, arg_list: string[])
--- @field iterator fun(self: AUPArguments): AUPArgumentsIterator
--- @field get fun(self: AUPArguments, i: integer): AUPArgumentEntry
--- @field consume fun(self: AUPArguments, i: integer): nil
--- @field is_consumed fun(self: AUPArguments, i: integer): boolean
--- @field _all AUPArgumentEntry[]
--- @field _consumed { [string]: boolean }
--- @field build_dir string
local AUPArguments = class.AUPArguments()

--- nitialize a new AUPArguments instance from a list of command arguments
--- nly `--debug=⟨level⟩` and `--debug` arguments are consumed.
--- --⟨key⟩=⟨value⟩` argument and `--⟨key⟩` simply give a new entry.
--- @param arg_list string[]
function AUPArguments:_init(arg_list)
  local function do_something() end
  for k,v in ipairs(arg_list) do
    local k2,v2 = match(v, "^%-%-?([^=]+)=(.*)$")
    if k2 == 'debug' then
      dbg:level_set(v2)
      do_something = function ()
        print("debug level: "..dbg:level_get())
      end
    else
      k = match(v, "^%-%-?([^=]+)$")
      if k == 'debug' then
        dbg:level_increment(1)
        do_something = function ()
          print("debug level: "..dbg:level_get())
        end
      end
    end
  end
  do_something()
  local all = {}
  for i,argument in ipairs(arg_list) do
    dbg:printf(1, "argument: %i -> %s\n", i, argument)
    local k,v = match(argument, "^%-%-?([^=]+)=(.*)$")
    if k == "build_dir" then
      local p = lpeg.S([=['"]=])^0
      local open = lpeg.Cg(p, "init")
      local close = lpeg.C(p)
      local closeeq = lpeg.Cmt(close * lpeg.Cb("init"), function (_s, _i, a, b) return a == b end)
      local string = open * lpeg.C((lpeg.P(1) - closeeq)^0) * close / 1
      self.build_dir = string:match(v)
    elseif k == "debug" then
      -- do nothing
    elseif k then
      -- '--⟨key⟩=⟨value⟩' argument
      dbg:write(10, k..": "..v)
      table.insert(all, AUPArgumentEntry(k, v))
    else
      -- another '--⟨key⟩' argument?
      k = match(argument, "^%-%-?([^=]+)$")
      if k == "debug" then
        -- do nothingself.build_dir = v
      elseif k then
        dbg:write(10, "--"..k)
        table.insert(all, AUPArgumentEntry(k, true))
      end
    end
  end
  self._all = all
  self._consumed = {}
  assert(self.build_dir and path.isdir(self.build_dir) and path.exists(self.build_dir))
end

--- onsume the argument at the given index
--- @param self AUPArguments
--- @param i integer
function AUPArguments:consume(i)
  self._consumed[i] = true
end

--- hether the argument at the given index is consumed
--- @param self AUPArguments
--- @param i integer
--- @return boolean
function AUPArguments:is_consumed(i)
  return self._consumed[i] and true
end

--- hether the argument at the given index is consumed
--- @param self AUPArguments
--- @param i integer
--- @return AUPArgumentEntry
function AUPArguments:get(i)
  return self._all[i]
end

--- @class (exact) AUPArgumentsIterator
--- @field _arguments AUPArguments The arguments previously parsed
--- @field _i integer The next argument
--- @field next fun(self: AUPArgumentsIterator): AUPArgumentEntry|nil The next argument is any
--- @field consume fun(self: AUPArgumentsIterator) consume the argument at the given index
local AUPArgumentsIterator = class.AUPArgumentsIterator()

-- Patch the `__call` function
do
  local mt = getmetatable(AUPArgumentsIterator)
  if mt then
    local call = mt.__call
    mt.__call = function(fun, ...)
      return call(fun, ...)
    end
  end
end
--- nitialize an AUPArgumentsIterator instance
--- @param arguments AUPArguments
function AUPArgumentsIterator:_init(arguments)
  self._arguments = arguments
  self._i = 0
  local mt = getmetatable(self)
  if mt then
    local call = mt.__call
    mt.__call = function(fun, ...)
      if fun == self then
        return self:next()
      end
      if call then
        return call(fun, ...)
      end
    end
  end
end

--- eturn the next argument entry that is not consumed
--- @return AUPArgumentEntry?
function AUPArgumentsIterator:next()
  self._i = self._i+1
  while self._arguments:is_consumed(self._i) do
    self._i = self._i + 1
  end
  return self._arguments:get(self._i)
end

--- ark the current argument as consumed
function AUPArgumentsIterator:consume()
  self._arguments:consume(self._i)
end

--- eturn an argument entries iterator
--- @return AUPArgumentsIterator
function AUPArguments:iterator()
  return AUPArgumentsIterator(self)
end

return {
  AUPArguments        = AUPArguments,
  AUPArgumentEntry    = AUPArgumentEntry,
  AUPArgumentsIterator = AUPArgumentsIterator
}
