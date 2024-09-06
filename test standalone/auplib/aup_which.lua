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
Similar to `which` unix command.

This module defines the `PathMgr` class that manages the `PATH` variable. 
--]=====]

--- @class AUP
local AUP = package.loaded.AUP

local State = AUP.State

local K = AUP.K

local pl_path   = require"pl.path"
local pl_class  = require"pl.class"
local List      = require"pl.List"
local pl_utils  = require"pl.utils"
local assert_string = pl_utils.assert_string

local dbg = AUP.dbg

local module = {}
AUP.Which = module

--- @class AUP.Which.PathMgr: AUP.State.Compliant
--- @field super fun(self: AUP.State.Compliant, id: string?, state: AUP.State?)
local PathMgr = pl_class(State.Compliant)

module.PathMgr = PathMgr

local os_concat
local os_setenv

--- @class oslib
--- @field type string

if os.type == "windows" then
  os_concat  = "&"
  os_setenv  = "set"
else
  os_concat  = ";"
  os_setenv  = "export"
end

local arguments = AUP.arguments
assert(arguments, "Internal error")

PathMgr.dev = arguments.dev

-- Intentionnaly declare a field
--- @class AUP.K
--- @field list 'list'

K.list = 'list'

local storage = AUP.State.Compliant()

--- Initialize instance
--- @param id string?
--- @param state AUP.State?
function PathMgr:_init(id, state)
  self:super(id, state)
end

--- Set content to feed the `PATH` environment variable.
--- @param l pl.List -- list of strings
function PathMgr:list_set(l)
  assert(List:class_of(l))
  self:state_set(K.list, l)
end

--- Get content to feed the `PATH` environment variable.
--- @return pl.List
function PathMgr:list_get()
  local ans = self:state_get(K.list, true)
  if ans == nil then
    ans = List()
    self:list_set(ans)
    PathMgr.list_get = function ()
      return assert(self:state_get_List(K.list), 'A pl.List was expected')
    end
  end
  return ans
end

--- Prepend the given directory to the actual `PATH` list.
--- @param dir string -- must point to an existing directory
function PathMgr:promote(dir)
  assert_string(1, dir)
  assert(pl_path.isdir(dir))
  local l = self:list_get()
  l:remove_value(dir)
  l:insert(1, dir)
end

--- Get content to feed the `PATH` environment variable.
--- @return string
function PathMgr:get()
  local l = self.list_get()
  return l:join(pl_path.dirsep)
end

--- Similar to terminal's `which`.
---
--- If a file exists at path `name` as si, `name` is returned normalized.
--- @param name string -- Completed by `.exe` on windows (if missing)
--- @return string? -- the full normalized path to the engine, if any
--- @return string? -- if name is a relative path, the directory containing name 
function PathMgr:which(name)
  dbg:printf(9, [[
PathMgr.which name:     %s
]], name)
  assert_string(1, name)
  if pl_path.is_windows and pl_path.extension(name)=="" then
    name = name .. '.exe'
  end
  if pl_path.isabs(name) then
    return pl_path.exists(name) and pl_path.normpath(name) or nil
  end
  local l = self:list_get()
  if dbg:level_get()>9 then
    print("PATH:"..#l)
    l:foreach(function (x) print("  "..x) end)
  end
  local res = l:map(function(x) return {n=pl_path.join(x,name), d=x} end)
  res = res:filter(function(x) return pl_path.exists(x.n) ~= false end)
  if #res > 0 then
    return pl_path.normpath(res[1].n), res[1].d
  end
end

---@type AUP.Which.PathMgr
module.library = PathMgr()

---@type AUP.Which.PathMgr
module.engine = PathMgr()

---@type AUP.Which.PathMgr
module.dev = PathMgr()

do
  --- @type AUP.Which.PathMgr
  ---
  --- State compliant.
  local Which_current = AUP.State.Compliant()

  --- @class AUP.K
  --- @field Which_current 'Which_current'

  K.Which_current = 'Which_current'

  ---Get the current PATH manager.
  ---
  --- State compliant.
  ---@return AUP.Which.PathMgr
  function module.current_get()
    return Which_current:state_get(K.Which_current)
  end

  ---Get the current PATH manager.
  ---@param type 'library'|'engine'|'dev'
  function module.current_set(type)
    return Which_current:state_set(K.Which_current, module[type])
  end
end

module._DESCRIPTION = 'AUPLib PATH manager'
module._VERSION = '0.1'

dbg:write(1, "aup_which loaded")

return module
