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

---@class AUP
local AUP = package.loaded.AUP
local dbg = AUP.dbg

local List = require"pl.List"
local pl_class = require"pl.class"

--- @class lpeg
local lpeg = package.loaded.lpeg -- built into texlua

--- @class AUP.Arguments: AUP.Class
--- @field _all AUP.Arguments.Entry[]
--- @field _consumed { [integer]: boolean }
--- @field setup boolean
--- @field name string
--- @field session_id string
--- @field dev boolean
--- @field build_dir string
--- @field local_value string
--- @field dry boolean
--- @field only_term boolean
local Arguments = pl_class()

AUP.Arguments = Arguments

--- @class AUP.Arguments.Entry: AUP.Class
--- @field key string the `⟨key⟩` in `--test-⟨key⟩=⟨value⟩` or `--test-⟨key⟩`
--- @field value string|true the `⟨value⟩` in `--test-⟨key⟩=⟨value⟩` or `true` for `--test-⟨key⟩`
--- @field _arguments AUP.Arguments
local Entry = pl_class()

Arguments.Entry = Entry

---Initialize a new argument entry instance
--- @param key string
--- @param value string|true
--- @param arguments AUP.Arguments
function Entry:_init(key, value, arguments)
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
function Entry:value_is_string()
  return type(self.value) == 'string'
end

--- The value typed as string or a void string
---
--- Use it after `value_is_string` has returned `true`.
--- @return string
function Entry:string_value()
  return tostring(self.value) or ''
end

--- The value interpreted as boolean
---
--- An explicit case insensitive 'false' is `false`.
--- An explicit case insensitive 'no' is `false`.
--- Everything else is `true`.
--- @return boolean
function Entry:boolean_value()
  local s = string.lower(self:string_value())
  return s ~= 'false' and s ~= 'no'
end

--- Consume the receiver
function Entry:consume()
  self._arguments:consume(self.key)
end

--- Whether the receiver is consumed
--- @return boolean
function Entry:is_consumed()
  return self._arguments:is_consumed(self.key)
end

function Entry:__tostring()
  return "Entry: "..self.key..' -> '..(self.value == true and 'true' or self.value)
end

--- @enum (key) AUP.Arguments.GetMode
local GetMode = {
  First   = 'First',
  Last = 'Last',
  All  = 'All',
}

Arguments.GetMode = GetMode

-- - @class AUP.K
-- - @field only_term string
-- - @field setup string
-- - @field name string
-- - @field session_id string
-- - @field dev string
-- - @field no_dev string
-- - @field build_dir string
-- - @field debug string
-- - @field local_key string
-- - @field dry string

---@class AUP.K
local K = AUP.K

K.only_term = 'only_term'
K.setup = 'setup'
K.session_id = 'session_id'
K.name = 'name'
K.dev = 'dev'
K.no_dev = 'no_dev'
K.build_dir = 'build_dir'
K.debug = 'debug'
K.local_key = 'local'
K.dry = 'dry'
K.build_gh = 'build_gh'
K.build_svn = 'build_svn'
K.euptex_gh = 'euptex_gh'
K.euptex_svn = 'euptex_svn'
K.pdftex_gh = 'pdftex_gh'
K.pdftex_svn = 'pdftex_svn'
K.xetex_gh = 'xetex_gh'
K.xetex_svn = 'xetex_svn'

local unquote, as_boolean
do
  local p = lpeg.S([=['"]=])^0
  local open = lpeg.Cg(p, "init")
  local close = lpeg.C(p)
  local closeeq = lpeg.Cmt(close * lpeg.Cb("init"), function (_s, _i, a, b) return a == b end)
  local str = open * lpeg.C((lpeg.P(1) - closeeq)^0) * close / 1
  unquote = function (v)
    local ans = str:match(v)
    return #ans > 0 and ans or v
  end
  local no_list = List{'false', 'no', 'n', 'f'}
  as_boolean = function(v)
    return not no_list:contains(string.lower(v))
  end
end

--- Initialize a new AUP.Arguments. instance from a list of command arguments
--- Only `--debug=⟨level⟩` and `--debug` arguments are consumed.
--- --⟨key⟩=⟨value⟩` argument and `--⟨key⟩` simply give a new entry.
--- @param arg_list string[]
function Arguments:_init(arg_list)
  local function do_something() end
  -- pl.pretty(arg_list)
  for k,v in ipairs(arg_list) do
    local k2,v2 = match(v, "^%-%-?([^=]+)=(.*)$")
    if k2 == AUP.K.debug then
      dbg:level_set(v2)
      do_something = function ()
        print("debug level: "..dbg:level_get())
      end
    else
      k = match(v, "^%-%-?([^=]+)$")
      if k == AUP.K.debug then
        dbg:level_increment(1)
        do_something = function ()
          print("debug level: "..dbg:level_get())
        end
      end
    end
  end
  do_something()
  local all = {}
  self.setup = false
  self.name = ''
  self.session_id = ''
  self.build_dir = ''
  self.dev = false
  self.dry = false
  self.local_value = ''
  self.only_term = false
  self.build_gh = false
  self.build_svn = false
  self.euptex_gh = false
  self.euptex_svn = false
  self.pdftex_gh = false
  self.pdftex_svn = false
  self.xetex_gh = false
  self.xetex_svn = false
  for i,argument in ipairs(arg_list) do
    dbg:printf(1, "argument: %i -> %s\n", i, argument)
    local k,v = match(argument, "^%-%-?([^=]+)=(.*)$")
    if k == K.setup then
      self.setup = as_boolean(v)
    elseif k == K.build_dir then
      self.build_dir = unquote(v)
    elseif k == K.debug then
      -- do nothing
    elseif k == K.only_term then
      self.only_term = as_boolean(v)
    elseif k == K.dry then
      self.dry = as_boolean(v)
    elseif k == K.dev then
      self.dev = as_boolean(v)
    elseif k == K.no_dev then
      self.dev = as_boolean(v)
    elseif k == K.name then
      self.name = unquote(v)
    elseif k == K.local_key then
      self.local_value = unquote(v)
    elseif k == K.session_id then
      self.session_id = unquote(v)
    elseif k == K.build_gh then
      self.build_gh = as_boolean(v)
    elseif k == K.build_svn then
      self.build_svn = as_boolean(v)
    elseif k == K.euptex_gh then
      self.euptex_gh = as_boolean(v)
    elseif k == K.euptex_svn then
      self.euptex_svn = as_boolean(v)
    elseif k == K.pdftex_gh then
      self.pdftex_gh = as_boolean(v)
    elseif k == K.pdftex_svn then
      self.pdftex_svn = as_boolean(v)
    elseif k == K.xetex_gh then
      self.xetex_gh = as_boolean(v)
    elseif k == K.xetex_svn then
      self.xetex_svn = as_boolean(v)
    elseif k then
      -- '--⟨key⟩=⟨value⟩' argument
      dbg:write(10, k..": "..v)
      table.insert(all, AUP.Arguments.Entry(k, v, self))
    else
      -- another '--⟨key⟩' argument?
      k = match(argument, "^%-%-?([^=]+)$")
      if k == AUP.K.setup then
        self.setup = true
      elseif k == AUP.K.debug then
        -- do nothing
      elseif k == AUP.K.only_term then
        self.only_term = true
      elseif k == AUP.K.dry then
        self.dry = true
      elseif k == AUP.K.dev then
        self.dev = true
      elseif k == AUP.K.no_dev then
        self.dev = false
      elseif k == K.build_gh then
        self.build_gh = true
      elseif k == K.build_svn then
        self.build_svn = true
      elseif k == K.euptex_gh then
        self.euptex_gh = true
      elseif k == K.euptex_svn then
        self.euptex_svn = true
      elseif k == K.pdftex_gh then
        self.pdftex_gh = true
      elseif k == K.pdftex_svn then
        self.pdftex_svn = true
      elseif k == K.xetex_gh then
        self.xetex_gh = true
      elseif k == K.xetex_svn then
        self.xetex_svn = true
      elseif k then
        dbg:write(10, "--"..k)
        table.insert(all, AUP.Arguments.Entry(k, true, self))
      end
    end
  end
  self._all = all
  self._consumed = {}
  assert(#self.build_dir>0, 'Unknown build directory')
  if self.dev then
    if #self.local_value == 0 then
      self.local_value = kpse.var_value('SYNCTEX_DEV_LOCAL') or ''
      dbg:assert(#self.local_value>0, "Missing `--local=⟨...⟩` argument or `SYNCTEX_DEV_LOCAL=⟨...⟩` environment variable")
    end
  end
end

--- Consume the argument at the given index or for the given key
---
--- When a key is given, all the arguments with that key are marked as consumed
--- @param self AUP.Arguments
--- @param i_or_key integer|string
function Arguments:consume(i_or_key)
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
--- @param self AUP.Arguments
--- @param i_or_key integer|string
--- @return boolean
function Arguments:is_consumed(i_or_key)
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
--- @param self AUP.Arguments
--- @param i_or_key integer|string
--- @param mode AUP.Arguments.GetMode?
--- @return AUP.Arguments.Entry?
function Arguments:get(i_or_key, mode)
  if type(i_or_key) == 'string' then
    local ans = nil
    if mode == AUP.Arguments.GetMode.All then
      ans = {}
      for _,entry in ipairs(self._all) do
        if entry.key == i_or_key then
          table.insert(ans, entry)
        end
      end
    elseif mode == AUP.Arguments.GetMode.Last then
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

--- @class (exact) AUP.Arguments.Iterator: AUP.Class
--- @field _arguments AUP.Arguments The arguments previously parsed
--- @field _i integer The next argument
--- @field next fun(self: AUP.Arguments.Iterator): AUP.Arguments.Entry? The next argument is any
--- @field consume fun(self: AUP.Arguments.Iterator) consume the argument at the given index
local Iterator = pl_class()

Arguments.Iterator = Iterator

-- Patch the `__call` function
do
  local mt = getmetatable(Iterator)
  if mt then
    local call = mt.__call
    mt.__call = function(fun, ...)
      return call(fun, ...)
    end
  end
end

--- Initialize an AUP.Arguments.Iterator instance
--- @param arguments AUP.Arguments
function Iterator:_init(arguments)
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
--- @return AUP.Arguments.Entry?
function Iterator:next()
  self._i = self._i + 1
  while self._arguments:is_consumed(self._i) do
    self._i = self._i + 1
  end
  return self._arguments:get(self._i)
end

--- Mark the current argument as consumed
function Iterator:consume()
  self._arguments:consume(self._i)
end

--- Return an argument entries iterator
--- @return AUP.Arguments.Iterator
function Arguments:iterator()
  return AUP.Arguments.Iterator(self)
end

return {
  Arguments = Arguments,
}
