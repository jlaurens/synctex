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

--]=====]

local pl_path = require"pl.path"

--- @type AUP
local AUP = package.loaded.AUP

AUP.TL.current_set_from_year(2222)

local Dir = AUP.Dir
local Command = AUP.Command
local Engine = AUP.Engine

local dbg = AUP.dbg

Dir.synctex_bin_setup()
Command.PATHList_promote(assert(Dir.synctex_bin_get()))

-- synchronize the texlive work directory with this contents

local gh = AUP.TL.gh

local tex_bin_gh = assert(gh:bin_dir_get())
print("tex_bin_gh", tex_bin_gh)
print("Dir.synctex", Dir.synctex)
print(AUP.synctexdir)

local status, err = gh:configure_make()
print(status, err)
os.exit(123)

local synctexdir_mtime = gh:mtime_get()

-- Dir.touch_src()

local d = AUP.Dir.synctex

gh:synchronize_synctexdir_with(d)

local mt = pl_path.getmtime("abc")
print(mt)
-- list filed 
os.exit(123)

AUP.units:test_setup_on_after(function()
  if dbg:level_get() > 0 then
    for name in Engine.tex_all() do
      local source = AUP.Command.which(name)
      print('engine: '..name..'=>'..source)
    end
    print('')
  end
end)
