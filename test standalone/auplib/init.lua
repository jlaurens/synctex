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

local match = string.match
local ipairs = ipairs

local separator = package.config:sub(1,1)
local lfs = package.loaded.lfs
local path_dir_auplib = lfs.currentdir() ..'/auplib/'
local kpathsea = package.loaded.kpse.new("kpsewhich")
local pl

--[[
  Sometimes debugging is complicated because the error is reported by penlight.
  One possibility is to use a local penlight distribution installed with luarocks
  at `.../test standalone/auplib/lua_modules/`.
]]
do
  local already = false
  for _,v in ipairs(arg) do
    if v == "--local_penlight" then
    -- try the local distribution, if any
      already = true
      print('Local penlight distribution')
      package.path = path_dir_auplib.."lua_modules/share/lua/5.3/?.lua;"..package.path
      pcall(function ()
        pl = require('pl.import_into')()
      end)
      break
    end
  end
  if not pl then
    local p = kpathsea:find_file('penlight.lua')
    print('Penlight distribution at '..(p or "?"))
    assert(p)
    local dir = match(p, "^(.*/)[^/"..separator.."]*/[^/"..separator.."]*$")
    package.path = dir.."?/?.lua;"..package.path
    local status, _ = pcall(function ()
      pl = require('penlight')
    end)
    -- final possibility: local distribution if not already tested before
    if not status and not already then
      package.path = path_dir_auplib.."lua_modules/share/lua/5.3/?.lua;"..package.path
      pl = require('pl.import_into')()
    end
  end
end
print("loading auplib...3")
assert(pl, "No penlight available")
package.path = path_dir_auplib.."?.lua;"..package.path
local raise = pl.utils.raise

--- @class AUPK

--- @class AUP
--- @field _VERSION string
--- @field _DESCRIPTION string
--- @field K AUPK
--- @field test_standalone_dir string
--- @field loaded table
--- @field PL table
--- @field short_path fun(p: string?)
--- @field import_l3build fun(self: AUP, env: table)
--- @field l3build_proxy AUPL3BuildProxy
--- @field pushd fun(dir: string, label: string):boolean, string?
--- @field pushd_or_raise fun(dir: string, label: string)
--- @field popd fun(label: string):boolean, string?
--- @field popd_or_raise fun(label: string)
--- @field arguments AUPArguments?
--- @field br fun(label: string, char: string?)

local AUP = {
  _VERSION = '0.1',
  _DESCRIPTION = 'SyncTeX testing framework',
  loaded = {},
  PL = pl,
  K = {},
}
setmetatable(AUP, {
  -- lazy table
  __index = function(t, key)
    if key == "l3build_proxy" then
      local ans = t:import_l3build()
      t.l3build_proxy = ans
      return ans
    end
    local lower_key = string.lower(key)
    if lower_key ~= key then
      local module = t.loaded[lower_key]
      if module == nil then
        if AUP.dbg then
          AUP.dbg:write(1, lower_key)
        end
        local path = pl.path.join(path_dir_auplib, 'aup_'..lower_key..'.lua')
        if AUP.dbg then
          AUP.dbg:write(1, "Module path: "..AUP.short_path(path))
        end
        local f, err = loadfile(path)
        if err then
          raise(err, 2)
        end
        if not f then
          print(debug.traceback())
          error('Unknown AUP module '.. key)
        end
        module = assert(f())
        t.loaded[lower_key] = module
      end
      t[key] = module[key]
      return module[key]
    end
  end
})

-- from https://gist.github.com/jrus/3197011

function AUP:import_l3build(env)
  local importer = self.L3Build
  self.l3build_proxy = importer(env)
  self.import_l3build = function(s,e) end
end

local PL = AUP.PL

--- @class PL_List
--- @field iter fun(): fun()
--- @field append fun(self: PL_List, x: any)
--- @field contains fun(self: PL_List, x: any): boolean
--- @field sort fun(self: PL_List, cmp:(string|fun(x: any, y:any): boolean)?)
--- @field join fun(self: PL_List, sep: string?): string
--- @field foreach fun (self: PL_List, fun: fun(x: any, ...), ...)
--- @field map fun (self: PL_List, fun: (fun(x: any, ...): any), ...)
local PL_List = PL.List

--- @class PLMap
--- @field keys PL_List
--- @field values PL_List
--- @field get fun(self: PLMap, key: any): any
--- @field getvalues fun(self: PLMap, keys: PL_List): PL_List
--- @field set fun(self: PLMap, key: any, value: any)
--- @field iter fun(): any
--- @field items fun(self: PLMap): PL_List
--- @field setdefault fun(self: PLMap, key: any, default: any)
--- @field len fun(self: PLMap): number
--- @field update fun(self: PLMap, t: table)

--- @class PLOrderedMap: PLMap
--- @field insert fun(self: PLOrderedMap, pos: number, key: any, val: any)
--- @field keys fun(self: PLOrderedMap): PL_List
--- @field values fun(self: PLOrderedMap): PL_List
--- @field sort fun(self: PLMap, cmp:(string|(fun(x: any, y: any): boolean))?)

local currentdir = PL.path.currentdir
local chdir = PL.path.chdir
local relpath = PL.path.relpath

local pushd_stack = PL_List()

PL.text.format_operator()

--- Sort of command line pushd emulation.
--- @param dir string
--- @param label string
--- @return boolean
--- @return string?
function AUP.pushd(dir, label)
  PL.utils.assert_string(1, dir)
  PL.utils.assert_string(2, label)
  local cwd = currentdir()
  local ans, err = chdir(dir)
  if ans then
    pushd_stack:append({ cwd = cwd, label = label})
    if AUP.dbg then
      AUP.dbg:write(999, "pushd (%s):\n  from: %s\n  to:   %s"%{label, cwd, PL.path.currentdir()})
      -- if AUP.dbg:level_get() > 99 then
      --   print(debug.traceback(3))
      -- end
    end
  end
  return ans, err
end

--- Sort of command line pushd emulation.
--- @param dir string
--- @param label string
function AUP.pushd_or_raise(dir, label)
  local ans, err = AUP.pushd(dir, label)
  if not ans then
    -- print(lfs.currentdir())
    -- print(debug.traceback(2))
    error("Improbable error: "..err, 2)
  end
end

--- Sort of command line popd emulation.
--- @param label string
--- @return boolean
--- @return string?
function AUP.popd(label)
  PL.utils.assert_string(1, label)
  if #pushd_stack > 0 then
    local pop = pushd_stack:pop()
    if pop == nil then
      return false, 'nothing to pop to'
    end
    if label and label ~= pop.label then
      return false, "Unbalanced pop label: %s ~= %s"%{pop.label, label}
    end
    if AUP.dbg then
      local cwd = lfs.currentdir()
      local ans, err = chdir(pop.cwd)
      AUP.dbg:write(999, "popd(%s):\n  from: %s\n  to:   %s"%{pop.label, cwd, pop.cwd})
      -- if AUP.dbg:level_get() > 99 then
      --   print(debug.traceback(3))
      -- end
      return ans, err
    else
      return chdir(pop.cwd)
    end
  end
  return false, "popd without a pushd"
end

--- Sort of command line popd emulation.
--- @param label string
function AUP.popd_or_raise(label)
  local ans, err = AUP.popd(label)
  if not ans then
    print('cwd: %s'%{lfs.currentdir()})
    print(debug.traceback(3))
    error("Improbable error: "..err, 2)
  end
end

AUP.test_standalone_dir = currentdir()

print(AUP.test_standalone_dir)

--- Return the argument relative to the initial current directory.
--- This is used to keep some information private.
--- This is not full proofed.
--- @param p string?
--- @return string
function AUP.short_path(p)
  p = p or currentdir()
  return PL_List({'...', relpath(p, AUP.test_standalone_dir)})
    :filter(function(x) return #x >0 end)
    :join('/')
end

package.loaded.auplib = AUP
package.loaded.AUP = AUP

--- @class AUP
--- @field open_file fun(path: string)
--- @field dbg AUPDBG
--- @field state AUPState

--- Open a file
---
--- Does nothing if `--only_term` has been specified.
--- Forwards to `AUP.open_file` otherwise.
--- @param path string
function AUP.open_file(path)
  if AUP.arguments and AUP.arguments.only_term then
    return
  end
  AUP.do_open_file(path)
end

--- Open a file
---
--- Default implementation does nothing
--- @param path string
function AUP.do_open_file(path)
  error("Must be overriden in a `test_setup_⟨os type⟩_⟨os name⟩.lua` file")
  --PL.utils.executeex('open %s.pdf'%{jobname})
end

-- next definition is helpless in practice
-- the lls does not recognize the AUP_br_arg type

---@alias AUP_br_key
---| '"label"' # The label to display
---| '"ch"' # the character used in the separator

--- @alias AUP_br_arg table<AUP_br_key, string>

local separations = PL.Map()

--- Print a break line
---
--- One can choose the character to draw the line
--- One can also choose the label to display at the end of the line
--- @param kvarg AUP_br_arg
function AUP.br(kvarg)
  local ch = kvarg.ch or '-'
  local prefix = separations[ch]
  if prefix == nil then
    prefix = PL.stringx.rstrip(PL.stringx.indent(ch, 19, ch))
    separations[ch] = prefix
  end
  print('%s %s'%{prefix, kvarg.label or ''})
end

AUP.dbg = AUP.DBG()
AUP.state = AUP.State()
AUP.Utils.patch_penlight()

print("AUP loaded")
