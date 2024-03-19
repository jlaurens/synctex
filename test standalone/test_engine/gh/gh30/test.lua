--[===[
Copyright (c) 2008-2024 jerome DOT laurens AT u-bourgogne DOT fr
This file is part of the __SyncTeX__ package testing facilities.

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
 
]===]

print("This test always passes")
print("=======================")

local AUP = package.loaded.AUP
local PL = AUP.PL
local dir = PL.dir
local path = PL.path
local file = PL.file
local seq = PL.seq
local utils = PL.utils
local stringx = PL.stringx

local AUPEngine = AUP.module.engine
local InteractionMode = AUPEngine.InteractionMode

local match = string.match
local join = path.join
local makepath = dir.makepath
local write = file.write
local read = file.read
local splitlines = stringx.splitlines
local printf = utils.printf
local my_path = join(AUP.tmp_dir, '¡¢£¤¥¦§', '¨©ª«¬­®')
makepath(my_path)
local p_1 = join(my_path, "gh30.tex")
write (p_1, [==[
%\documentclass{article}
%\begin{document}
\mathsurroundskip=1cm
$y$%
\bye
%\end{document}
]==])
for name in seq.list {'luatex'} do
  local p_2 = join(my_path, name)
  makepath(p_2)
  if AUP.pushd(p_2) then
    local _status, _ans, stdout, _errout = AUPEngine('luatex'):synctex(-1):interaction(InteractionMode.nonstopmode):run('../gh30.tex')
    for i,l in ipairs(splitlines(stdout)) do
      if match(l, "Synchronize ERROR") then
        printf("Unexpected at line %i: <%s>\n", i, l)
      end
    end
    local s = read("gh30.synctex")
    print(s)
    AUP.popd()
  end
end
