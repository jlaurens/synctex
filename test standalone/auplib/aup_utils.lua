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

local pl_class  = require"pl.class"
local pl_utils  = require"pl.utils"
local pl_compat = require"pl.compat"

--- @class AUP.Utils: AUP.Class
local Utils = pl_class()

AUP.Utils = Utils

--- execute a shell command, in a compatible and platform independent way.
-- This is a compatibility function that returns the same for Lua 5.1 and
-- Lua 5.2+.
--
-- NOTE: Windows systems can use signed 32bit integer exitcodes. Posix systems
-- only use exitcodes 0-255, anything else is undefined.
--
-- NOTE2: This is penlight implementation limited to texlua.
-- @param cmd a shell command
-- @return true if successful
-- @return actual return code
function Utils.execute(cmd)
  --- @type integer
  --- @diagnostic disable-next-line: assign-type-mismatch
  local res1  = os.execute(cmd)
  if not pl_utils.is_windows then
    res1 = res1 > 255 and res1 / 256 or res1
  end
  return res1==0, res1
end

--- Path penlight
function Utils.patch_penlight()
  pl_compat.execute = Utils.execute
  pl_utils.execute = Utils.execute
  AUP.dbg:write(1, "Penlight patched")
end

AUP.dbg:write(1, "aup_utils loaded")

return {
  Utils = Utils,
}
