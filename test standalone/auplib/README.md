# AUP utility library

Work in progress.

The testing logic is collected in different files of this directory.
All files are prefixed with `aup_` for practical reasons.

## Testing new engines.

The purpose is to test modifications on engines. This was used while implementing the `\synctexmark` command.

The source code comes from the git repository at
```
https://github.com/TeX-Live/texlive-source
```
You first make a fork of this repository on your own account and then clone with GitHub cli of GitHub desktop. When built, the modified engines are collected in
```
.../texlive-source/Work/texk/web2c/
```
In order to make these binaries available without giving the full path,we can amend the `PATH` environment variable to include this directory.

We use a distribution for testing. This can be a clone of a recent `texlive/YYYY`. On `macOS` we duplicate `/usr/local/texlive/2024/` into `/usr/local/texlive/2222/`. We change `/usr/local/texlive/2222/texmf.cnf`.
```
% (Public domain.)
% This texmf.cnf file should contain only your personal changes from the
% original texmf.cnf (for example, as chosen in the installer).
%
% That is, if you need to make changes to texmf.cnf, put your custom
% settings in this file, which is .../texlive/YYYY/texmf.cnf, rather than
% the distributed file (which is .../texlive/YYYY/texmf-dist/web2c/texmf.cnf).
% And include *only* your changed values, not a copy of the whole thing!
%
TEXMFHOME = ~/Library/texmf-2222
TEXMFVAR = ~/Library/texlive/2222/texmf-var
TEXMFCONFIG = ~/Library/texlive/2222/texmf-config
```

Using a local `texmf.cnf` will help with this configuration.
The environment variable `TEXMFCNF` is set to ensure that this local `texmf.cnf` is read such that the distribution used is the correct one. More precisely, the environment variable will mimic what a normal `kpsewich -all texmf.cnf` gives.

## Test driven development of TeX engines

### Introduction

This only concerns `pdftex`, `euptex` and `xetex`.
This does not concern `LuaTeX` which has a very specific developing workflow.

We have the directory where `TeX` engine sources are collected, edited and compiled.
Actually, `TeXLive` sources from `git` are used for both engines,
but `pdfTeX` also has its own source tree which is not considered at first.

In order to run the modified engines, we need a full working distribution.
This is based on official `TeXLive` distribution, for different years.

Once the development is done, we have to sytnchronize to an `svn` repository.

### `git` source tree

In order to build the engine from the `TeXLive` source tree, from the directory
that contains the topmost `configure` script, run
```
mkdir Work-synctex
cd Work-synctex
../configure --without-x --disable-shared --disable-all-pkgs \
--enable-pdftex --enable-pdftex-synctex \
--enable-euptex --enable-euptex-synctex \
--enable-xetex --enable-xetex-synctex \
--enable-synctex \
--enable-missing -C CFLAGS=-g CXXFLAGS=-g CPPFLAGS="-DSYNCTEX_DEBUG=1000"
```
then
```
make
```
In order to run the engine,
```
TEXMFROOT=/usr/local/texlive/2024 \
TEXMFCNF=$TEXMFROOT/texmf-dist/web2c \
⟨...⟩/Work-synctex/texk/web2c/pdftex ⟨whatever⟩.tex
```
