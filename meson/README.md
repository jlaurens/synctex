# Building and testing the SyncTeX command line tool

The main tool is `meson`, see downloading instructions from the [official meson site](https://mesonbuild.com/).

## Clone
First retrieve the source code. You can use the green code button from the [main SyncTeX page](https://github.com/jlaurens/synctex). If you plan to contribute to the code, it is recommanded to clone the GitHub synctex repository and use GitHub desktop application if available on your operating system.

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