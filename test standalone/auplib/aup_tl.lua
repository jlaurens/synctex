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
Here are defined functions to setup the directory layout for installed texlive distributions.
Installed TeXLive distributions are used for engine and library testing up to year 2222 excluded.

The current TeXlive distribution `AUP.TL.current` is used for library testing.
It is set up during the testing setup phase. It determines where the typesetting engines are located
depending on the `AUP.Command.Path`.
--]=====]

local insert = table.insert

---@type LuaFileSystem
local lfs = lfs

local pl_class      = require"pl.class"
local pl_utils      = require"pl.utils"
local pl_path       = require"pl.path"
local assert_string = pl_utils.assert_string

--- @class AUP
local AUP = package.loaded.AUP

local K = AUP.K

--- @class AUP.TL: AUP.State.Compliant
--- @field super fun(self: AUP.State.Compliant)
local TL = pl_class(AUP.State.Compliant)

AUP.TL = TL

---Initialize a texlive helper
function TL:_init()
  self:super()
end

---Set the location of the distribution
---@param dir string -- the directory can be relative
function TL:dir_set(dir)
  assert_string(2, dir)
  self:state_set(K.dir, dir)
end

--- Get the location of the distribution
---
--- raises when not previously set.
--- @param no_check boolean? -- check for directory existence iff falsy
--- @return string
function TL:dir_get(no_check)
  local dir assert(self:state_get_string(K.dir),
  'AUP.TL('..self._id.."):dir_set(...) must be called in a `test_setup...lua` file.")
  if not no_check then
    assert(pl_path.isdir(dir), 'No directory at '..dir)
  end
  return dir
end

--- @class AUP.K
--- @field relative_bin_dir string

K.relative_bin_dir = "relative_bin_dir"

--- Get the relative location where tex binaries are stored.
--- @return string
function TL:relative_bin_dir_get()
  return assert(self:state_get_string(K.relative_bin_dir),
  "...:relative_bin_dir_set(...) must be called in a `test_setup_local_⟨cfg⟩.lua` file.")
end

--- Set the relative location where tex binaries are stored.
--- @param path string -- the path relative to the `Work` directoty.
function TL:relative_bin_dir_set(path)
  self:state_set(K.relative_bin_dir, path)
end

--- The directory of tex binaries.
--- @param no_check boolean? -- check for directpry existence iff falsy
--- @return string
function TL:bin_dir_get(no_check)
  local dir = pl_path.join(self:dir_get(), self:relative_bin_dir_get())
  if not no_check then
    assert(pl_path.isdir(dir), 'No directory at '..dir)
  end
  return dir
end

---Set the `by_year` function
---@param f fun(year: string|integer): string
function TL.set_by_year(f)
  TL.by_year = f
end

--- Set the ditribution to the given year.
--- @param year string|integer
function TL:set_year(year)
  self:dir_set(TL.by_year(year))
end

--- @class AUP.TL
local current = TL()

TL.current = current

return {
  TL = TL,
}
