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
local dirname = PL.path.dirname
local PL_class = PL.class
local PL_utils = PL.utils
local quote_arg = PL_utils.quote_arg
local executeex = PL_utils.executeex

local List = PL.List

local lpeg = package.loaded.lpeg -- built into texlua

--- @class (exact) AUPArgumentEntry
--- @field _init fun(self: AUPArgumentEntry, key: string, value: string|true, arguments: AUPArguments)
--- @field key string the `⟨key⟩` in `--test-⟨key⟩=⟨value⟩` or `--test-⟨key⟩`
--- @field value string|true the `⟨value⟩` in `--test-⟨key⟩=⟨value⟩` or `true` for `--test-⟨key⟩`
--- @field consume fun(self: AUPArgumentEntry)
--- @field is_consumed fun(self: AUPArguments): boolean
--- @field value_is_string fun(self: AUPArgumentEntry): boolean
--- @field _arguments AUPArguments
local AUPArgumentEntry = PL_class.AUPArgumentEntry()

--- Initialize a new argument entry instance
--- @param key string
--- @param value string|true
---@param arguments AUPArguments
function AUPArgumentEntry:_init(key, value, arguments)
  self.key = key
  self._arguments = arguments
  if type(value) == 'string' then
    local m = match(value, '^"(.*)"$')
    if m then
      value = m
    end
  end
  self.value = value
end

--- Whether the value is a string
--- @return boolean
function AUPArgumentEntry:value_is_string()
  return type(self.value) == 'string'
end

--- Consume the receiver
function AUPArgumentEntry:consume()
  self._arguments:consume(self.key)
end

--- Whether the receiver is consumed
--- @return boolean
function AUPArgumentEntry:is_consumed()
  return self._arguments:is_consumed(self.key)
end

function AUPArgumentEntry:__tostring()
  return "AUPArgumentEntry: "..self.key..' -> '..(self.value == true and 'true' or self.value)
end

--- @enum (key) AUPArgumentGetMode
local AUPArgumentGetMode = {
  First   = 'First',
  Last = 'Last',
  All  = 'All',
}

--- @class (exact) AUPArguments
--- @field _init fun(self: AUPArguments, arg_list: string[])
--- @field iterator fun(self: AUPArguments): AUPArgumentsIterator
--- @field get fun(self: AUPArguments, i_or_key: integer|string, mode:AUPArgumentGetMode?): AUPArgumentEntry?|[AUPArgumentEntry]
--- @field consume fun(self: AUPArguments, i_or_key: integer|string): nil
--- @field is_consumed fun(self: AUPArguments, i_or_key: integer|string): boolean
--- @field _all AUPArgumentEntry[]
--- @field _consumed { [integer]: boolean }
--- @field build_dir string
--- @field uuid string
--- @field _uuid_txt string
--- @field _update_uuid boolean
local AUPArguments = PL_class.AUPArguments()

--- Initialize a new AUPArguments instance from a list of command arguments
--- Only `--debug=⟨level⟩` and `--debug` arguments are consumed.
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
    if k == "uuid_txt" then
      local p = lpeg.S([=['"]=])^0
      local open = lpeg.Cg(p, "init")
      local close = lpeg.C(p)
      local closeeq = lpeg.Cmt(close * lpeg.Cb("init"), function (_s, _i, a, b) return a == b end)
      local str = open * lpeg.C((lpeg.P(1) - closeeq)^0) * close / 1
      self._uuid_txt = str:match(v)
    elseif k == "update_uuid" then
      local p = lpeg.S([=['"]=])^0
      local open = lpeg.Cg(p, "init")
      local close = lpeg.C(p)
      local closeeq = lpeg.Cmt(close * lpeg.Cb("init"), function (_s, _i, a, b) return a == b end)
      local str = open * lpeg.C((lpeg.P(1) - closeeq)^0) * close / 1
      self._update_uuid = str:match(v)
    elseif k == "build_dir" then
      local p = lpeg.S([=['"]=])^0
      local open = lpeg.Cg(p, "init")
      local close = lpeg.C(p)
      local closeeq = lpeg.Cmt(close * lpeg.Cb("init"), function (_s, _i, a, b) return a == b end)
      local str = open * lpeg.C((lpeg.P(1) - closeeq)^0) * close / 1
      self.build_dir = str:match(v)
    elseif k == "debug" then
      -- do nothing
    elseif k then
      -- '--⟨key⟩=⟨value⟩' argument
      dbg:write(10, k..": "..v)
      table.insert(all, AUPArgumentEntry(k, v, self))
    else
      -- another '--⟨key⟩' argument?
      k = match(argument, "^%-%-?([^=]+)$")
      if k == "debug" then
        -- do nothingself.build_dir = v
      elseif k then
        dbg:write(10, "--"..k)
        table.insert(all, AUPArgumentEntry(k, true, self))
      end
    end
  end
  self._all = all
  self._consumed = {}
  if self._uuid_txt then
    self.build_dir = dirname(self._uuid_txt)
    local f = io.open(self._uuid_txt, "r")
    assert(f)
    self.uuid = f:read()
    f:close()
    dbg:printf(1, "AUP: UUID: %s\n", self.uuid)
  end
  if self._update_uuid then
    assert(self._uuid_txt)
    local cmd = quote_arg(List({
      'python3',
      self._update_uuid or false,
      self._uuid_txt or false
    }):filter(function(x)
      return type(x)=='string' and #x>0
    end))
    local c, r, o, e = executeex(cmd)
    dbg:printf(1, "%s/%i/%s/%s", c and "T" or "F", r or 0, o, e)
    os.exit(0)
  end
  assert(self.build_dir and PL.path.isdir(self.build_dir) and PL.path.exists(self.build_dir))
end

--- Consume the argument at the given index or for the given key
---
--- When a key is given, all the arguments with that key are marked as consumed
--- @param self AUPArguments
--- @param i_or_key integer|string
function AUPArguments:consume(i_or_key)
  if type(i_or_key)=='string' then
    for i,entry in ipairs(self._all) do
      if entry.key == i_or_key then
        self._consumed[i] = true
      end
    end
  else
    self._consumed[i_or_key] = true
  end
end

--- Whether the argument at the given index is consumed
--- @param self AUPArguments
--- @param i_or_key integer|string
--- @return boolean
function AUPArguments:is_consumed(i_or_key)
  if type(i_or_key) == 'string' then
    for i, entry in ipairs(self._all) do
      if entry.key == i_or_key and self._consumed[i] then
        return true
      end
    end
    return false
  else
    return self._consumed[i_or_key] and true
  end
end

--- Get the argument at the given index or for the given key
---
--- When a key is provided the last corresponding argument is returned
--- @param self AUPArguments
--- @param i_or_key integer|string
--- @param mode AUPArgumentGetMode?
--- @return AUPArgumentEntry?
function AUPArguments:get(i_or_key, mode)
  if type(i_or_key) == 'string' then
    local ans = nil
    if mode == AUPArgumentGetMode.All then
      ans = {}
      for _,entry in ipairs(self._all) do
        if entry.key == i_or_key then
          table.insert(ans, entry)
        end
      end
    elseif mode == AUPArgumentGetMode.Last then
      for _,entry in ipairs(self._all) do
        if entry.key == i_or_key then
          ans = entry
        end
      end
    else
      for _,entry in ipairs(self._all) do
        if entry.key == i_or_key then
          ans = entry
          break
        end
      end
    end
    return ans
  else
    return self._all[i_or_key]
  end
end

--- @class (exact) AUPArgumentsIterator
--- @field _arguments AUPArguments The arguments previously parsed
--- @field _i integer The next argument
--- @field next fun(self: AUPArgumentsIterator): AUPArgumentEntry? The next argument is any
--- @field consume fun(self: AUPArgumentsIterator) consume the argument at the given index
local AUPArgumentsIterator = PL_class.AUPArgumentsIterator()

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
--- Initialize an AUPArgumentsIterator instance
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

--- Return the next argument entry that is not consumed
--- @return AUPArgumentEntry?
function AUPArgumentsIterator:next()
  self._i = self._i+1
  while self._arguments:is_consumed(self._i) do
    self._i = self._i + 1
  end
  return self._arguments:get(self._i)
end

--- Mark the current argument as consumed
function AUPArgumentsIterator:consume()
  self._arguments:consume(self._i)
end

--- Return an argument entries iterator
--- @return AUPArgumentsIterator
function AUPArguments:iterator()
  return AUPArgumentsIterator(self)
end

return {
  Arguments = AUPArguments,
  Entry     = AUPArgumentEntry,
  Iterator  = AUPArgumentsIterator,
  GetMode  = AUPArgumentGetMode
}
