# Automated tests of SyncTeX library and SyncTeX command line tool

All the material related to automatic testing is gathered in this folder named `test standalone`.
We have here two different kinds of tests: those that rely on TeX engines and those that do not. We have the `test_engine` directory for tests related to engines and the `test_library` directory for tests that are not related to engines. The former are generally based on the engine output whereas the latter are based on the `synctex-test` extented variant of `synctex`. These directories are not meant to have a counterpart in TeXLive sources.

The `test_dev` folder is used during development of new features. It belongs the development process and should not be distributed. Comments below partly concern this folder.

## Structure

Test units are logically gathered in first level subfolders of `test_engine` or `test_library` directories. The name of these subfolders are designated by `⟨suite⟩`. Each subfolder corresponds to one test unit and is designated by `⟨unit⟩`.

## Declaration in `meson.build`

For example, in order to test the whole library `⟨suite⟩`, we declare
```
test(
  'library/⟨suite⟩',
  texlua,
  args: shared_library_args + ['--suite=⟨suite⟩'],
  workdir: synctex_test_dir,
  env: synctex_no_malloc,
)
```
Then running next command from `.../synctexdir/meson`
```
meson test -C build library/⟨suite⟩
```
will change the current directory to
```
.../synctexdir/test standalone/test_library/⟨suite⟩
```
and run the `test.lua` file found there. This file can in turn load other test files. This is used for the `gh` library and engine suites.

The variable `texlua` points to the `texlua` (or `texlua.exe`) command currently available. The variable `shared_library_args` is a table containing shared arguments for library related tests. The variable `synctex_test_dir` contains the full path to `.../synctexdir/test standalone/`. Finally, the variable `synctex_no_malloc` allows some convenient setup.

For example, in order to test the unit `⟨unit⟩` of engine suite `⟨suite⟩`, we declare
```
test(
  'engine/⟨suite⟩/...',
  texlua,
  args: shared_engine_args + ['--suite=⟨suite⟩', '--unit=⟨unit⟩'],
  workdir: synctex_test_dir,
  env: synctex_no_malloc,
)
```
Then running next command from `.../synctexdir/meson`
```
meson test -C build library/⟨suite⟩/...
```
will change the current directory to
```
.../synctexdir/test standalone/test_engine/⟨suite⟩/⟨unit⟩
```
and run the `test.lua` file found there.

## Setup and teardown

When executing a `meson test` request, various `.lua` script files are loaded and executed. Before `test.lua`,

- `.../test standalone/test_setup.lua`, always
- `.../test standalone/test_⟨library|engine⟩/test_setup.lua`, for library or engine tests
- `.../test standalone/test_⟨library|engine⟩/⟨suite⟩/test_setup.lua`, when `⟨suite⟩` is also involved

After the `test.lua`,
- `.../test standalone/test_⟨library|engine⟩/⟨suite⟩/test_teardown.lua`, when `⟨suite⟩` is also involved
- `.../test standalone/test_⟨library|engine⟩/test_teardown.lua`, for library or engine tests
- `.../test standalone/test_teardown.lua`, always

In addition, `.../test_setup_⟨local⟩.lua` and `.../test_setup_⟨local⟩.lua` are executed when `--local=⟨local⟩` was an argument provided on the command line otherwaise when the environment variable `SYNCTEX_DEV_LOCAL=⟨local⟩` is set.
These files may contain processing that cannot be shared between developers, for example system dependent processing or configuration dependent processing.
Particularly useful while during develoment.

## Testing framework

The `auplib` folder contains goodies and material to perform the tests. These are essentailly `texlua` scripts called by the top level `test_main.lua` script of the `test standalone` folder. We use the `penlight` library as well as `os.type` and `os.name` extensiopns to the `os` table provided by `texlua`.

The documentation of the frameworks is available in `.../test standalone/doc/` and various `README.md` spread in subdirectories.

## Subfolders

### `test_engine`

This folder is gathering various tests targeting the TeX engine features.

### `test_library`

This folder is gathering various tests targeting synctex library features.

### `test_dev`

This folder is gathering various tests for development.

