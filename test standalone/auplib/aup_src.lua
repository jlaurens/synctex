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
`Src` objects are source directory controllers. They allow to build engines.
There are 2 kinds of sources:

- `TeXLive` for `pdftex`, `euptex` and `xetex`.
- A standalone directory for `pdftex` only.

We do not take care of `LuaTeX` because it is different.

`Src` objects are state compliant.
--]=====]

---@type LuaFileSystem
local lfs = lfs

local pl_class      = require"pl.class"
local pl_utils      = require"pl.utils"
local pl_path       = require"pl.path"
local assert_string = pl_utils.assert_string

--- @class AUP
local AUP = package.loaded.AUP

local K = AUP.K

--- @class AUP.Src: AUP.State.Compliant
--- @field super fun(self: AUP.State.Compliant)
local Src = pl_class(AUP.State.Compliant)

AUP.Src = Src

---Initialize a source controller helper
function Src:_init()
  self:super()
end

--- @class AUP.K
--- @field dir string

AUP.K.dir = 'dir'

---Initialize a source controller helper
---@param dir string -- the path to the source directory, must be a string. Can be relative. State compliant.
function Src:dir_set(dir)
  assert_string(2, dir)
  self:state_set(K.dir, dir)
end

--- Get the path to the source directory of the receiver
---
--- Raises if this path is not defined or does not point to a directory.
--- @param no_check boolean? -- check for existence iff falsy
---@return string
function Src:dir_get(no_check)
  local dir = assert(self:state_get_string(K.dir), '`dir_set` never called')
  if not no_check then
    assert(pl_path.isdir(dir), 'No directory at '..dir)
  end
  return dir
end

--- @class AUP.K
--- @field relative_Work_dir string

K.relative_Work_dir = 'relative_Work_dir'

--- Get the relative location where tex binaries are stored.
--- @return string
function Src:relative_Work_dir_get()
  return assert(self:state_get_string(K.relative_bin_dir),
    "AUP.Src("..self._id.."):relative_Work_dir_set(...) must be called in a `test_setup_local_⟨cfg⟩.lua` file.")
end

--- Set the relative location of the `Work` directory.
--- @param path string
function Src:relative_Work_dir_set(path)
  self:state_set(K.relative_bin_dir, path)
end

--- Get the location where tex binaries are stored.
--- @param no_check boolean? -- do not check for existence of the result
--- @return string
function Src:Work_dir_get(no_check)
  local dir = pl_path.join(self:dir_get(), self:relative_Work_dir_get())
  if not no_check then
    assert(pl_path.isdir(dir), 'No directory at '..dir)
  end
  return dir
end

--- Set the relative location where tex binaries are stored in the Work directory.
--- @param path string
function Src:relative_bin_dir_set(path)
  assert_string(2, path)
  self:state_set(K.relative_bin_dir, path)
end

--- Get the relative location where tex binaries are stored in the Work directory.
--- @return string
function Src:relative_bin_dir_get()
  return assert(self:state_get_string(K.relative_bin_dir),
  "AUP.Src()"..self._id.."):relative_bin_dir_set(...) must be called in a `test_setup_local_⟨cfg⟩.lua` file.")
end

--- The directory of tex binaries.
--- @param no_check boolean? -- do not check for existence of the result
--- @return string
function Src:bin_dir_get(no_check)
  local dir = pl_path.join(self:Work_dir_get(), self:relative_bin_dir_get())
  if not no_check then
    assert(pl_path.isdir(dir), 'No directory at '..dir)
  end
  return dir
end

--- @class AUP.Src.Pdf: AUP.Src
local Pdf = pl_class(Src)

local pdf = Pdf()
Src.pdf = pdf

--- The directory of tex binaries.
function Pdf:build()
  -- How whould you build
end

return {
  Src = Src,
}
