# Building and testing the SyncTeX library and the SyncTeX command line tool

The main building and testing tool is `meson`, see downloading instructions from the [official meson site](https://mesonbuild.com/).

## Clone
First retrieve the source code from GitHub. You can use the green code button from the [main SyncTeX page](https://github.com/jlaurens/synctex). If you plan to contribute to the code, it is recommanded to clone the GitHub synctex repository and use GitHub desktop application if available for your operating system.

## Building
Instructions below are given to build the `synctex` command line tool on possibly various operating systems.
Building is made through the command line.

### Building with meson (and ninja)

To build the project, from the `.../synctexdir/meson` folder run:
```sh
meson setup build
meson compile -C build
```

To install the project, run
```sh
meson install -C build
```
You might need to `sudo` if you don't own enough privilege.

To uninstall the project, run
```sh
meson --internal uninstall -C build
```
You might need to `sudo` if you don't own enough privilege.

### Setting up Xcode with meson

Install meson with [homebrew](https://brew.sh/):
```sh
brew install meson ninja
```

Build the Xcode project files:
```sh
meson setup --backend xcode build
meson compile -C build
```

You will find the Xcode project in `.../synctexdir/meson/build/synctex.xcodeproj`.

### Setting up Visual Studio with meson

Next instructions are not tested.

```sh
meson setup --backend vs build
meson compile -C build
```

## Setting up Xcode manually

If `meson` is not suitable, next instructions were used for Xcode 7 but they might apply as well for other versions. The `build` folder and project are created out of the `.../synctexdir/` folder.

### create a new project

- create a new project
- choose a command line tool project
- set the product name to synctex
- set the language to C
- save it next to the `.../synctexdir/` folder just cloned
- in the navigation pane, remove the default syntex folder and its `main.c` default contents

you then have both folders `synctex` and `synctexdir` in the same directory.

### Add sources

- select the synctex project in xcode navigation left pane
- select menu itme File->Add files to "synctex"
- select the `.../synctexdir/` folder in the file browser
- uncheck "Copy items if needed"
- choose "Create groups"
- validate your choice
- select the synctex project
- go to the build phase pane
- remove `Readme.md` from the compile sources list

You end with 3 sources : `synctex_main.c`, `synctex_parser.c`, `synctex_parser_utils.c`

### Setup project

- add `-DSYNCTEX_STANDALONE` as other C flag (or `SYNCTEX_STANDALONE` preprocessor macro)
- set `Always Search User Path` build setting to YES
- in the build phase pane, link to libz1.2.5.tbd (or similar)

You are ready to build.

## Test

In order to run all the tests, from `.../synctexdir/meson/` run
```sh
meson test -C build
```

The `.../synctexdir/test standalone/` folder contains material for testing. There is also a `.../synctexdir/tests/` used by `TeXLive` for testing, this is not mirrored nor discussed here.

More details in the `.../synctexdir/test standalone/README.md`.

## How to

### Develop for TeXLive
Use case: a default TeX distribution for everyday use, for example TeXLive 2024, and a different TeX distribution for SyncTeX development, for example TeXLive checked out with svn. If you want to use TeX engine binaries from the development TeX distribution, you can specify

```sh
meson test -C build --test-args='--bin_dir=<location>'
```
For example
```sh
meson test -C build --test-args='--bin_dir="<somewhere>/TeXLive/Build/source/inst/bin/aarch64-apple-darwin23.2.0"'
```
Use `./Build CPPFLAGS="-DSYNCTEX_DEBUG=1000"` to build TeXLive binaries while activating debugging logs in SyncTeX support code.

These are the flags to disable things
```
  --disable-autosp \
  --disable-axodraw2 \
  --disable-devnag \
  --disable-lacheck \
  --disable-m-tx \
  --disable-pmx \
  --disable-ps2eps \
  --disable-t1utils \
  --disable-texdoctk \
  --disable-tpic2pdftex \
  --disable-vlna \
  --disable-xml2pmx \
  --disable-xpdfopen \
  --disable-web2c \
  --enable-auto-core \
  --disable-ipc \
  --disable-tex \
  --enable-tex-synctex \
  --enable-etex \
  --disable-etex-synctex \
  --enable-ptex \
  --disable-ptex-synctex \
  --enable-eptex \
  --disable-eptex-synctex \
  --enable-uptex \
  --disable-uptex-synctex \
  --disable-euptex \
  --disable-euptex-synctex \
  --disable-aleph \
  --disable-hitex \
  --disable-pdftex \
  --disable-pdftex-synctex \
  --disable-luatex \
  --disable-luajittex \
  --disable-luahbtex \
  --disable-luajithbtex \
  --disable-mp \
  --disable-pmp \
  --disable-upmp \
  --disable-xetex \
  --disable-xetex-synctex \
  --disable-mf \
  --disable-mf-nowin \
  --disable-mflua \
  --enable-mflua-nowin \
  --disable-mfluajit \
  --enable-mfluajit-nowin \
  --enable-epsfwin \
  --enable-hp2627win \
  --enable-mftalkwin \
  --disable-web-progs \
  --disable-synctex \
  --disable-afm2pl \
  --disable-bibtex-x \
  --disable-bibtex8 \
  --disable-bibtexu \
  --disable-chktex \
  --disable-cjkutils \
  --disable-detex \
  --disable-dtl \
  --disable-dvi2tty \
  --disable-dvidvi \
  --disable-dviljk \
  --disable-dviout-util \
  --disable-dvipdfm-x \
  --disable-dvipng \
  --disable-dvipos \
  --disable-dvipsk \
  --disable-dvisvgm \
  --disable-gregorio \
  --disable-gsftopk \
  --disable-lcdf-typetools \
  --disable-cfftot1 \
  --disable-mmafm \
  --disable-mmpfb \
  --disable-otfinfo \
  --disable-otftotfm \
  --disable-t1dotlessj \
  --disable-t1lint \
  --disable-t1rawafm \
  --disable-t1reencode \
  --disable-t1testpage \
  --disable-ttftotype42 \
  --disable-auto-cfftot1 \
  --disable-auto-t1dotlessj \
  --disable-auto-ttftotype42 \
  --disable-auto-updmap \
  --disable-makeindexk \
  --disable-makejvf \
  --disable-mendexk \
  --disable-musixtnt \
  --disable-ps2pk \
  --disable-psutils \
  --disable-seetexk \
  --disable-tex4htk \
  --disable-ttf2pk2 \
  --disable-ttfdump \
  --disable-upmendex \
  --disable-xdvik
```

```
  --disable-autosp        do not build the autosp package
  --disable-axodraw2      do not build the axodraw2 package
  --disable-devnag        do not build the devnag package
  --disable-lacheck       do not build the lacheck package
  --disable-m-tx          do not build the m-tx package
  --disable-pmx           do not build the pmx package
  --disable-ps2eps        do not build the ps2eps package
  --disable-t1utils       do not build the t1utils package
  --disable-texdoctk      do not build the texdoctk package
  --disable-tpic2pdftex   do not build the tpic2pdftex package
  --disable-vlna          do not build the vlna package
  --enable-xindy          build the xindy package
  --enable-xindy-rules      build and install make-rules package
  --enable-xindy-docs       build and install documentation
  --disable-xml2pmx       do not build the xml2pmx package
  --disable-xpdfopen      do not build the xpdfopen package
  --disable-web2c         do not build the web2c (TeX & Co.) package
  --enable-auto-core        cause TeX&MF to dump core, given a certain
                            filename
  --disable-dump-share      make fmt/base files architecture-dependent
  --disable-ipc             disable TeX's --ipc option, i.e., pipe to a
                            program
  --disable-tex             do not compile and install TeX
  --enable-tex-synctex        build TeX with SyncTeX support
  --enable-etex             compile and install e-TeX
  --disable-etex-synctex      build e-TeX without SyncTeX support
  --enable-ptex             compile and install pTeX
  --disable-ptex-synctex      build pTeX without SyncTeX support
  --enable-eptex            compile and install e-pTeX
  --disable-eptex-synctex     build e-pTeX without SyncTeX support
  --enable-uptex            compile and install upTeX
  --disable-uptex-synctex     build upTeX without SyncTeX support
  --disable-euptex          do not compile and install e-upTeX
  --disable-euptex-synctex    build e-upTeX without SyncTeX support
  --disable-aleph           do not compile and install Aleph
  --disable-hitex           do not compile and install HiTeX
  --disable-pdftex          do not compile and install pdfTeX
  --disable-pdftex-synctex    build pdfTeX without SyncTeX support
  --disable-luatex          do not compile and install LuaTeX
  --disable-luajittex       do not compile and install LuaJITTeX
  --disable-luahbtex        do not compile and install LuaHBTeX
  --disable-luajithbtex     do not compile and install LuaHBJITTeX
  --disable-mp              do not compile and install MetaPost
  --disable-pmp             do not compile and install pMetaPost
  --disable-upmp            do not compile and install upMetaPost
  --disable-xetex           do not compile and install XeTeX
  --disable-xetex-synctex     build XeTeX without SyncTeX support
  --disable-mf              do not compile and install METAFONT
  --disable-mf-nowin          do not build a separate non-window-capable
                              METAFONT
  --disable-mflua           do not compile and install MFLua
  --enable-mflua-nowin        build a separate non-window-capable MFLua
  --disable-mfluajit        do not compile and install MFLuaJIT
  --enable-mfluajit-nowin     build a separate non-window-capable MFLuaJIT
  --enable-epsfwin            include EPSF pseudo window support
  --enable-hp2627win          include HP 2627 window support
  --enable-mftalkwin          include mftalk (generic server) window support
  --enable-nextwin            include NeXT window support
  --enable-regiswin           include Regis window support
  --enable-suntoolswin        include old Suntools (not X) window support
  --enable-tektronixwin       include Tektronix window support
  --enable-unitermwin         include Uniterm window support
  --disable-web-progs       do not build WEB programs bibtex ... weave
  --disable-synctex         do not build the SyncTeX library and tool
  --disable-afm2pl        do not build the afm2pl package
  --disable-bibtex-x      do not build the bibtex-x package
  --disable-bibtex8         do not build the bibtex8 program
  --disable-bibtexu         do not build the bibtexu program
  --disable-chktex        do not build the chktex package
  --disable-cjkutils      do not build the cjkutils package
  --disable-detex         do not build the detex package
  --disable-dtl           do not build the dtl package
  --disable-dvi2tty       do not build the dvi2tty package
  --disable-dvidvi        do not build the dvidvi package
  --disable-dviljk        do not build the dviljk package
  --disable-dviout-util   do not build the dviout-util package
  --disable-dvipdfm-x     do not build the dvipdfm-x package
  --disable-dvipng        do not build the dvipng package
  --disable-debug           Compile without debug (-d) option
  --enable-timing           Output execution time of dvipng
  --disable-dvipos        do not build the dvipos package
  --disable-dvipsk        do not build the dvipsk package
  --disable-dvisvgm       do not build the dvisvgm package
  --disable-gregorio      do not build the gregorio package
  --disable-gsftopk       do not build the gsftopk package
  --disable-lcdf-typetools
                          do not build the lcdf-typetools package
  --disable-cfftot1         do not build the cfftot1 program
  --disable-mmafm           do not build the mmafm program
  --disable-mmpfb           do not build the mmpfb program
  --disable-otfinfo         do not build the otfinfo program
  --disable-otftotfm        do not build the otftotfm program
  --disable-t1dotlessj      do not build the t1dotlessj program
  --disable-t1lint          do not build the t1lint program
  --disable-t1rawafm        do not build the t1rawafm program
  --disable-t1reencode      do not build the t1reencode program
  --disable-t1testpage      do not build the t1testpage program
  --disable-ttftotype42     do not build the ttftotype42 program
  --disable-auto-cfftot1    disable running cfftot1 from otftotfm
  --disable-auto-t1dotlessj disable running t1dotlessj from otftotfm
  --disable-auto-ttftotype42
                            disable running ttftotype42 from otftotfm
  --disable-auto-updmap     disable running updmap from otftotfm
  --disable-makeindexk    do not build the makeindexk package
  --disable-makejvf       do not build the makejvf package
  --disable-mendexk       do not build the mendexk package
  --disable-musixtnt      do not build the musixtnt package
  --disable-ps2pk         do not build the ps2pk package
  --disable-psutils       do not build the psutils package
  --disable-seetexk       do not build the seetexk package
  --disable-tex4htk       do not build the tex4htk package
  --disable-ttf2pk2       do not build the ttf2pk2 package
  --disable-ttfdump       do not build the ttfdump package
  --disable-upmendex      do not build the upmendex package
  --disable-xdvik         do not build the xdvik package
```

Next should work but seems not.
```sh
../configure --disable-all-pkgs --enable-pdftex-synctex \
    -C CFLAGS=-g CXXFLAGS=-g >&outc || echo fail
```
Do the make:
```
make >&outm || echo fail
```
Run the tool
```
TEXMFROOT=/usr/local/texlive/YYYY \
TEXMFCNF=$TEXMFROOT/texmf-dist/web2c \
     ./pdftex ...
```

## Real setup on MacOS
This setup was used to develop the `\synctexmark` command.
We start with a fresh check out of TeXLive that we then build.
Then we build a `.texdist` structure from scratch to point to this TeXLive check out. One can duplicate an
existing `.texdist` and make the appropriate changes inside. Once done, we select this distribution with the tl manager. Now we can work safely without interfering with everyday work, provided we switch back to the proper distribution.
We then check out a fresh pdftex source from
```
svn co svn://tug.org/pdftex/branches/stable/
```
and build.
Then we link
```
ln -s .../source/build-pdftex/texk/web2c/pdftex /Library/TeX/Root/pdftex
```
Once you modify some source of pdftex, including synctex, run from `.../source/build-pdftex/texk/web2c`:
```
make pdftex && fmtutil --sys --byfmt pdftex
```

## Print debug information
During test, some supplemental information may help: use
```sh
meson test -C build --test-args='--debug'
```
or
```sh
meson test -C build --test-args='--debug=<non negative number>'
```
In the former, each `--debug` is cumulative meaning that is allows the display of more information.

## Working with TeXLive from GitHub

We start from a clone of the `texlive-source` repository on github. Then we build everything with
```
./Build CPPFLAGS="-DSYNCTEX_DEBUG=1000"
```

We setup the environment:
```
MY=/Library/TeX/Distributions/.FactoryDefaults/TeXLive-GitHub/Contents
export TEXMF=$MY/Root/texmf-dist
export TEXMFCNF=$TEXMF/web2c
export TEXMFVAR=$MY/TexmfVar
export TEXMFSYSVAR=$MY/TexmfSysVar
export TEXFORMATS=$MY/TexmfSysVar
export PATH=$MY/Programs/texbin:$PATH
```

We duplicate some
`/Library/TeX/Distributions/.FactoryDefaults/TeXLive-...`
into
`/Library/TeX/Distributions/.FactoryDefaults/TeXLive-GitHub`.
Then we replace the destinations of the symbolic links in
`/Library/TeX/Distributions/.FactoryDefaults/TeXLive-GitHub/Programs/` by `.../texlive-source/Work/texk/web2c/`.
We also change the target of the `TexmfSysVar` symbolic link to `.../texlive-source/Work/TexmfSysVar` and we also change the target of the `TexmfVar` symbolic link to `.../texlive-source/Work/TexmfVar`.

## Working with TeXLive from GitHub, july 2024 workflow

We start from a clone of the `texlive-source` repository on github. Then we build everything with
```
./Build CPPFLAGS="-DSYNCTEX_DEBUG=1000"
```

We setup the environment from the `auplib` testing framework before launching the commands.
