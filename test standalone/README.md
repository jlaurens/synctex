# Automated tests

The `synctexdir` folder of TeXLive has a `tests` directory that we do not mirror here. Nevertheless, some tests may be similar in TeXLive and GitHub. 

All the material related to testing is gathered in this folder named `test standalone`.
We have here two different kinds of tests: those that rely on TeX engines and those that do not. Instead we have the `test_engine` directory for tests related to engines and the `test_library` directory for tests that are not related to engines. The former are based generally based on the engine output whereas the latter are based on the `synctex-test` extented variant of `synctex`. These directories are not meant to have a counterpart in TeXLive sources.

The `auplib` folder contains goodies and material to perform the tests. These are texlua scripts called by the top level `test_main.lua` script of the `test standalone` folder.

## Structure

Test units are logically gathered in first level subfolders of `test_engine` or `test_library` directories. The name of these subfolders are designated by `⟨units-dir⟩`.

## Declaration

Each folder `⟨units-dir⟩` corresponds to entries in `meson.build`.
First some shared variables:
```
synctex_no_malloc = environment({'MALLOC_PERTURB_': '0'})
synctex_test_dir = meson.current_source_dir() / 'test standalone'
```

For one specific unit named `⟨unit name⟩`, we have
```
test(
  '⟨engine|library⟩/⟨units-dir⟩/⟨unit name⟩',
  find_program('texlua'),
  args: [
    'test.lua',
    '--⟨engine|library⟩',
    '--dir="⟨units-dir⟩"',
    '--unit="⟨unit-name⟩"'
    ],
  workdir: synctex_test_dir,
  env: synctex_no_malloc,
)
```
For all the unit test of one folder
```
test(
  '⟨engine|library⟩/⟨units-dir⟩',
  find_program('texlua'),
  args: [
    'test.lua',
    '--⟨engine|library⟩',
    '--dir="⟨units-dir⟩"'
    ],
  workdir: synctex_test_dir,
  env: synctex_no_malloc,
)
```
Then some `test.lua` script must exist in various `⟨units-dir⟩` and `⟨units-dir⟩/⟨unit name⟩`, see existing ones to see how they work.

## Testing framework

The `auplib` folder contains all the `lua` material for testing.
