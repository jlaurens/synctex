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

--- @type AUP
local AUP = package.loaded.AUP
local PL = AUP.PL
local PL_class = PL.class
local PL_List = PL.List
local PLMap = PL.Map

-- Where the data are really stored
local _state = {}

-- AUP is a bridge to the `_state` variable
--- @class AUPState
--- @field setup fun(self: AUPState)
--- @field teardown fun(self: AUPState)
--- @field on_before_teardown fun(self: AUPState, f: fun(): any)
--- @field on_after_teardown fun(self: AUPState, f: fun(): any)

local AUPState = PL_class()

--- Create a new scoping level
---
--- Use it before each test setup
function AUPState:_init()
  self._level = 0
  self._state = {}
  self._before_stack = PL_List()
  self._after_stack = PL_List()
end

--- Create a new scoping level
---
--- Use it before each test setup
function AUPState:setup()
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
  self._before_stack = PL_List()
  self._after_stack = PL_List()
end

--- Register a function to be executed before the next teardown
---@param f fun(): any
function AUPState:on_before_teardown(f)
  self._before_stack:append(f)
end

--- Register a function to be executed after the next teardown
---@param f fun(): any
function AUPState:on_after_teardown(f)
  self._after_stack:append(f)
end

--- Remove one scoping level
---
--- Use it before each test teardown
function AUPState:teardown()
  local mt = getmetatable(self._state)
  assert(mt, "Unbalanced state:teardown")
  assert(mt._state, "Corrupted metatable")
  assert(mt._before_stack, "Corrupted metatable")
  assert(mt._after_stack, "Corrupted metatable")
  for ff in self._before_stack:iter() do ff() end
  self._before_stack = mt._before_stack
  _state = mt._state
  for ff in self._after_stack:iter() do ff() end
  self._after_stack = mt._after_stack
end

-- AUP is a bridge to the `_state` variable
--- @class AUPState
--- @field set fun(self: AUPState, key: string, value: any)
--- @field get fun(self: AUPState, key: string, raw: boolean?): any

--- Get a state value
---
--- Accessing state values through `AUPState` should be sufficient
--- but this is allows a less verbose coding.
---@param key string
---@param raw boolean?
---@return any
function AUPState:get(key, raw)
  PL.utils.assert_string(2, key)
  if raw ~= nil and raw then
    return rawget(self._state, key)
  else
    return self._state[key]
  end
end

--- Set a new state value
---@param key string
---@param value any
function AUPState:set(key, value)
  PL.utils.assert_string(2, key)
  self._state[key] = value
end

--- @class AUPState
--- @field listGet fun(self: AUPState, key: string): PL_List
--- @field mapGet fun(self: AUPState, key: string): PLMap

--- Get the value for the given key as PL_List
---
--- PL_List values are not inherited as is. A copy is inherited on the first
--- request to allow further modifications on the list.
---@param key string
---@return PL_List?
function AUPState:listGet(key)
  PL.utils.assert_string(2, key)
  local ans = rawget(self._state, key)
  if PL_List:class_of(ans) then
    return ans
  elseif type(ans) == 'table' or ans == nil then
    ans = PL_List(ans)
    self._state[key] = ans
    return ans
  else
    return nil
  end
end

--- Get the value for the given key as PLMap
---
--- PLLMap values are not inherited as is. A copy is inherited on the first
--- request to allow further modifications on the map.
---@param key string
---@return PLMap?
function AUPState:mapGet(key)
  PL.utils.assert_string(2, key)
  local ans = rawget(self._state, key)
  if PLMap:class_of(ans) then
    return ans
  elseif type(ans) == 'table' or ans == nil then
    self._state[key] = PLMap()
    if ans then
      self._state[key]:update(ans)
    end
    return self._state[key]
  else
    return nil
  end
end


--- @class AUP
--- @field State AUPState

AUP.State = AUPState

AUP.dbg:write(1, "aup_command loaded")

return {
  State = AUPState,
}
