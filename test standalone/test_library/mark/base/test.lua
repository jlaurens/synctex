--[[
Copyright (c) 2024 jerome DOT laurens AT u-bourgogne DOT fr
This file is a bridge to the __SyncTeX__ package testing framework.

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
 
--]]

local AUP = package.loaded.AUP
local x = [==[
  \the\synctex
  a%
  \expandafter\ifx\csname synctexmark\endcsname\relax
  \def\synctexmark#1#2{\message{^^JFAILED}}%
  \fi
  \message{^^JSupported characters in SyncTeX mark label}%
  \message{^^J***** !"#$&'()*+,-./}%
  \synctexmark {!"#$&'()*+,-./}%
  \message{^^J***** 0123456789:;<=>?@}%
  \synctexmark {0123456789:;<=>?@}%
  \message{^^J***** ABCDEFGHIJKLMNOPQRSTUVWXYZ}%
  \synctexmark {ABCDEFGHIJKLMNOPQRSTUVWXYZ}%
  \message{^^J***** []_`}%
  \synctexmark {[]_`}%
  \message{^^J***** abcdefghijklmnopqrstuvwxyz}%
  \synctexmark {abcdefghijklmnopqrstuvwxyz}%
  \def\abc{d e f}%
  \synctexmark { b{l}a=/. *_:+-;\abc, a⚫︎►z}%
  \synctexmark {  }%
  \synctexmark {  ©  }%
  \synctexmark 0 { b{l}a=/. *_:+-;\abc, a⚫︎►z}%
  \synctexmark 1 { b{l}a=/. *_:+-;\abc, }%
  \synctexmark 2 { b{l}a=/. *_:+-;\abc, }%
  \synctexmark ? { blablabla }%
  {\synctex = -5
  \the\synctex}%
  \the\synctex
  {\global\synctex = -3
  \the\synctex}%
  \the\synctex
  \message{^^J********** AFTER SYNCTEX **********^^J}%
  b%
  \hbox to 10em{a\synctexmark{SecondMark}\hbox to 5em{a\synctexmark{TernaryMark}b }b }%
  \message{^^J********** AFTER HBOX **********^^J}%
  \vbox to 10em {
    \synctexmark{FourthMark}
    \leavevmode
    WXC%
  }
  \message{^^J********** AFTER VBOX **********^^J}%
  \bye
  ]==]
local y = [==[
  !"A\synctexmark{LABEL_1}B!"%
  \message{^^J********** BEFORE hbox **********^^J}%
  \hbox to 10em{A\synctexmark{LABEL_2}\hbox to 5em{B\synctexmark{LABEL_3}C }D }%
  \message{^^J********** AFTER hbox **********^^J}%
  \message{^^J********** BEFORE vbox **********^^J}%
  \vbox to 10em {
    \synctexmark{LABEL_4}%
    \leavevmode
    !"%
  }%
  \message{^^J********** AFTER vbox **********^^J}%
  \bye
]==]
local z = [==[
A%
\synctexmark{LABEL_1}%
B%
\message{**** before \hbox:}%
\hbox to 10em{%
\message{**** within \hbox:}%
C%
\synctexmark{LABEL_2}%
D%
}
\bye
]==]
AUP.test_library_mark('pdftex', "base", [==[
\tracingonline1%
\tracingcommands1%
\noindent
A\synctexmark{LABEL_1}B%
\hbox{C\synctexmark{LABEL_2}D}%
E%
\hbox{F%
  \hbox{G\synctexmark{LABEL_3}H}%
I}%
\synctexmark1{LABEL_4}%
\hbox{J}%
\synctexmark2{LABEL_5}%
$%
K%
\synctexmark2{LABEL_6}%
\hbox{L}%
M%
$%
\bye
]==])
