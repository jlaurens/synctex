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

local insert = table.insert
local remove = table.remove

local AUP = package.loaded.AUP
local PL = AUP.PL
local kpse = package.loaded.kpse
kpse.set_program_name('kpsewhich')
local lookup = kpse.lookup


--- @class AUPL3BuildProxy
--- @field env table
--- @field activate fun()
--- @field deactivate fun()

--- Imports l3build in the given environment or the current environment.
--- Locate the `l3buld.lua` in the TDS using `kpse`,
--- configure `package.path` accordingly,
--- finally `require('l3build')`.
--- Everything is imported in the current environment per `l3build` design.
--- nevertheless, one can play with environments to disable `l3build`
--- temporarily when there can be name conflicts.
--- change the environment before importing
--- @param env table? where `l3build` material is imported,
--- it defaults to a void table. Any ways its `__index` event points to the
--- current environment.
--- @return AUPL3BuildProxy
local function importer(env)
  local l3build_path = lookup("l3build.lua")
  local l3build_dir = string.match(l3build_path,"(.*[/])")
  package.path = l3build_dir.."?.lua;"..package.path
  env = env or {}
  setmetatable(env, {
    __index = _ENV
  })
  --- @type (table|true)[]
  local stack = {}
  local l3build_proxy = {
    env = env,
    activate = function()
      if _ENV ~= env then
        insert(stack, _ENV)
        _ENV = setmetatable(env, { __index = _ENV } )--?
      else
        insert(stack, true)
      end
    end,
    deactivate = function()
      --- @type table|true
      local e = assert(remove(stack))
      if e ~= true then
        _ENV = e
      end
    end,
  }
  local _ENV = setmetatable(env, { __index = _ENV })
  local l3build_lua = PL.file.read(l3build_path)
  local ra = {}
  for f in string.gmatch(l3build_lua, '_require%(.([^)]*).%)') do
    table.insert(ra, f)
  end
  for _,f in ipairs(ra) do
    AUP.dbg:printf(0, "Loading l3build-%s.lua\n", f)
    require('l3build-'..f)
  end
  return l3build_proxy
end

return importer
