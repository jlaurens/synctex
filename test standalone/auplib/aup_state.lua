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

--[==[
The state is a proxy that maintains some variables like the various locations where binaries
are found.

There is an implicit stack of states implemented with metatables.
--]==]

--- @class AUP
local AUP = package.loaded.AUP

local List        = require"pl.List"
local Map         = require"pl.Map"
local OrderedMap  = require"pl.OrderedMap"
local pl_class    = require"pl.class"
local pl_utils    = require"pl.utils"
local pl_path     = require"pl.path"
local assert_string = pl_utils.assert_string

-- AUP is a bridge to the `_state` variable
--- @class AUP.State: AUP.Class
local State = pl_class()

AUP.State = State

--- Create a new scoping level
---
--- Use it before each test setup
function State:_init()
  self._level = 0
  self._state = {}
  self._before_stack = List()
  self._after_stack = List()
  self._count = 0
end

--- Create a new scoping level
---
--- Use it before each test setup
function State:setup()
  local function f()
    local mt = self._state
    local t = {}
    setmetatable(t, {
      _state = self._state,
      _before_stack = self._before_stack,
      _after_stack = self._after_stack,
      __index = function(_t, k)
        if AUP.dbg then
          AUP.dbg:write(99999, "Accessing state key: "..k)
        end
        return mt[k]
      end
    })
    self._state = t
  end
  f()
  self._before_stack = List()
  self._after_stack = List()
end

--- Register a function to be executed before the next teardown
--- @param f fun(): any
function State:on_before_teardown(f)
  self._before_stack:append(f)
end

--- Register a function to be executed after the next teardown
--- @param f fun(): any
function State:on_after_teardown(f)
  self._after_stack:append(f)
end

--- Remove one scoping level
---
--- Use it before each test teardown
function State:teardown()
  local mt = getmetatable(self._state)
  assert(mt, "Unbalanced state:teardown")
  assert(mt._state, "Corrupted metatable")
  assert(mt._before_stack, "Corrupted metatable")
  assert(mt._after_stack, "Corrupted metatable")
  for ff in self._before_stack:iter() do ff() end
  self._before_stack = mt._before_stack
  self._state = mt._state
  for ff in self._after_stack:iter() do ff() end
  self._after_stack = mt._after_stack
end

--- Get a state value
--- @param key string
--- @param raw boolean?
--- @return any
function State:get(key, raw)
  assert_string(2, key)
  if raw ~= nil and raw then
    return rawget(self._state, key)
  else
    return self._state[key]
  end
end

--- Get a state string value
--- @param key string
--- @param raw boolean?
--- @return string?
--- @return string? -- error message when there is a value that is not a string
function State:get_string(key, raw)
  assert_string(2, key)
  local ans = self:get(key, raw)
  if type(ans) == "string" then
    return ans
  end
  return nil, ans ~= nil and 'Not a string for key '..key or nil
end

--- Get a state boolean value
--- @param key string
--- @param raw boolean?
--- @return boolean?
--- @return string? -- error message when there is a value that is not a boolean
function State:get_boolean(key, raw)
  assert_string(2, key)
  local ans = self:get(key, raw)
  if type(ans) == "boolean" then
    return ans
  else
    return nil, ans ~= nil and 'Not a string for key '..key or nil
  end
end

--- Set a new state value
--- @param key string
--- @param value any
function State:set(key, value)
  assert_string(2, key)
  rawset(self._state, key, value)
end

--- @class pl.List
--- @field class_of fun(class: pl.List, instance: any): boolean

--- Get the value for the given key as List
---
--- `pl.List` values are not inherited as is. A copy is inherited on the first
--- request to allow further modifications on the list.
--- @param key string
--- @return pl.List?
--- @return string? -- error message when there is a value that is not a `pl.List`
function State:get_List(key)
  assert_string(2, key)
  local ans = rawget(self._state, key)
  if List:class_of(ans) then
    return List(ans)
  elseif ans ~= nil then
    return nil, 'Not a pl.List for key '..key
  end
  ans = self._state[key]
  if type(ans) == 'table' or ans == nil then
    ans = List(ans)
    rawset(self._state, key, ans)
    return ans
  end
  return nil, 'Not a suitable value for key '..key
end

--- @class pl.Map
--- @field class_of fun(class: pl.Map, instance: any): boolean

--- Get the value for the given key as pl.Map
---
--- `pl.Map` values are not inherited as is. A copy is inherited on the first
--- request to allow further modifications on the map.
--- @param key string
--- @return pl.Map?
--- @return string? -- error message when there is a value that is not a `pl.List`
function State:get_Map(key)
  assert_string(2, key)
  local ans = rawget(self._state, key)
  if Map:class_of(ans) then
    return ans
  elseif ans ~= nil then
    return nil
  end
  local map = self._state[key]
  if type(map) == 'table' or map == nil then
    ans = Map()
    rawset(self._state, key, ans)
    if map then
      ans:update(map)
    end
    return ans
  end
  return nil, 'Not a suitable value for key '..key
end

--[=====[
--- @class pl.OrderedMap
--- @field class_of fun(class: pl.OrderedMap, instance: any): boolean

--- Get the value for the given key as pl.Map
---
--- `pl.Map` values are not inherited as is. A copy is inherited on the first
--- request to allow further modifications on the map.
--- @param key string
--- @return pl.OrderedMap?
function State:get_OrderedMap(key)
  assert_string(2, key)
  local ans = rawget(self._state, key)
  if OrderedMap:class_of(ans) then
    return ans
  elseif ans ~= nil then
    return nil
  end
  local map = self._state[key]
  if type(map) == 'table' or map == nil then
    ans = OrderedMap()
    rawset(self._state, key, ans)
    if map then
      ans:update(ans)
    end
    return ans
  else
    return nil
  end
end
--]=====]

--- Unique prefix for objects namespace
--- @param id string
--- @return string
function State:unique_prefix(id)
  assert_string(2, id)
  self._count = 1 + self._count
  return id..'.'..self._count..'/'
end

State.default = AUP.State()

--- @class AUP.State.Compliant: AUP.Class
local Compliant = pl_class(AUP.Class)

State.Compliant = Compliant

--- Initialize an instance
--- @param state AUP.State? -- When not provided, it defaults to `AUP.state` at run time
function Compliant:_init(id, state)
  id = id or ''
  assert_string(2, id)
  self._id = id
  self._state = state
  self._prefix = self:state():unique_prefix(id)
end

--- Unique private key from a given base key
--- @return AUP.State
function Compliant:state()
  return self._state or AUP.State.default
end

--- Unique private key from a given base key
--- @param id string
--- @return string -- key
function Compliant:state_unique_prefix(id)
  return self._prefix..id..'/'
end

--- Setup the receiver's state
--- @return any
function Compliant:state_setup()
  return self:state():setup()
end

--- Teardown the receiver's state
--- @return any
function Compliant:state_teardown()
  return self:state():teardown()
end

--- Unique private key from a given base key
--- @param base string
--- @return string -- key
function Compliant:state_key(base)
  assert_string(2, base)
  return self._prefix .. base
end

--- Return the state value
--- @param base string
--- @param raw boolean?
--- @return any
function Compliant:state_get(base, raw)
  return self:state():get(self:state_key(base), raw)
end

--- Return the state value
--- @param base string
--- @param value any
--- @return any
function Compliant:state_set(base, value)
  assert_string(2, base)
  return self:state():set(self:state_key(base), value)
end

--- Return the state value
--- @param base string
--- @param raw boolean?
--- @return string?
function Compliant:state_get_string(base, raw)
  return self:state():get_string(self:state_key(base), raw)
end

--- Return the state value
--- @param base string
--- @param raw boolean?
--- @return boolean?
function Compliant:state_get_boolean(base, raw)
  return self:state():get_boolean(self:state_key(base), raw)
end

--- Return the state value
--- @param base string
--- @return pl.List?
function Compliant:state_get_List(base)
  return self:state():get_List(self:state_key(base))
end

--- Return the state value
--- @param base string
--- @return pl.Map?
function Compliant:state_get_Map(base)
  return self:state():get_Map(self:state_key(base))
end

AUP.dbg:write(1, "aup_state loaded")

return {
  _DESCRIPTION = 'AUPLib State',
  _VERSION = '0.1',
  State = State,
}
