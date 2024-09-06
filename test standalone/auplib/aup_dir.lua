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
Here are defined functions to setup the directory layout.
We have different kind of directories:

- standalone test directory, hard coded
- source or distribution directories
  - texlive: `texlive_dir`, state compliant
  - texlive svn: `texlive_svn_dir`, state compliant
  - texlive GitHub: `texlive_gh_dir`, state compliant
  - synctex Github repository: `synctexdir`, hard coded
  - synctex svn: `synctexdir_svn_dir`, from `texlive_svn_dir`
  - synctex GitHub: `synctexdir_gh_dir`, from `texlive_gh_dir`
- binary directories
  - distributions
    - texlive: from `texlive_dir
  - development
    - texlive svn: from `texlive_svn_dir`
    - texlive GitHub: from `texlive_gh_dir`
  - synctex binaries: state compliant
--]=====]

---@type LuaFileSystem
local lfs = lfs

local pl_class      = require"pl.class"
local pl_utils      = require"pl.utils"
local pl_path       = require"pl.path"
local pl_dir        = require"pl.dir"
local assert_string = pl_utils.assert_string

--- @class AUP
local AUP = package.loaded.AUP

local K = AUP.K

local Command = AUP.Command
local dbg = AUP.dbg

--- @class AUP.Dir: AUP.Class
local Dir = pl_class()

AUP.Dir = Dir

Dir.test_standalone = assert(AUP.test_standalone_dir)
Dir.synctex = assert(AUP.synctexdir)

print("Dir.test_standalone", Dir.test_standalone)
print("Dir.synctex", Dir.synctex)

local arguments = AUP.arguments
assert(arguments, "Internal error")

--- @class AUP.K
--- @field synctex_bin_dir 'synctex_bin_dir'

K.synctex_bin_dir = 'synctex_bin_dir'

local storage = AUP.State.Compliant()

--- Sets the location where synctex binaries are stored.
---
--- @param dir string -- can be a relative path.
function Dir.synctex_bin_set(dir)
  assert_string(1, dir)
  storage:state_set(K.synctex_bin_dir, dir)
end

--- Gets the location where synctex binaries are stored.
--- @return string? -- the directory if any
--- @return string? -- an error message if something else than a string is there.
function Dir.synctex_bin_get()
  local ans, err = storage:state_get_string(K.synctex_bin_dir)
  if err then return ans, err end
  storage:state_setup()
  local p = Command.which("synctex", ans, true)
  storage:state_teardown()
  if p==nil then
    error("Unexpected directory %s"%{ans}, 2)
  end
  return ans
end

--- Sets the location where development synctex binaries are stored from arguments
function Dir.synctex_bin_setup()
  --- @type string
  local p
  local entry = arguments:get(K.synctex_bin_dir)
  if entry ~= nil then
    assert(entry:value_is_string(), "`--%s` needs a value"%{K.synctex_bin_dir})
    p = entry:string_value()
    entry:consume()
  else
    -- default
    p = pl_path.join(Dir.synctex, 'meson/build')
  end
  Dir.synctex_bin_set(pl_path.normpath(p))
  dbg:write(1,"Dir.synctex_bin_setup: "..Dir.synctex_bin_get())
end

--- Touch the src files.
function Dir.touch_src()
  for _,f in ipairs(pl_dir.getfiles(Dir.synctex, "synctex*")) do
    lfs.touch(f)
  end
end

Dir.synctex_bin_setup()

return {
  Dir = Dir,
}
