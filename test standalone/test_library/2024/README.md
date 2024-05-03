# Automated tests

Tests for the library

## Structure

Test units are logically gathered in first level subfolders of `test_engine` or `test_library` directories. The name of these subfolders are designated by `⟨units-dir⟩`.

## Declaration

Each folder `⟨units-dir⟩` corresponds to entries in `meson.build`.
First some shared variables:
```
synctex_no_malloc = environment({'MALLOC_PERTURB_': '0'})
synctex_test_engine_dir = meson.current_source_dir() / 'test_engine'
synctex_test_library_dir = meson.current_source_dir() / 'test_library'
```
Then variables for the `⟨units-dir⟩`
```
synctex_test_engine_⟨units-dir⟩_dir = synctex_test_engine_dir / '⟨units-dir⟩'
```
or
```
synctex_test_library_⟨units-dir⟩_dir = synctex_test_library_dir / '⟨units-dir⟩'
```

For one specific unit named `⟨unit name⟩`, we have
```
test(
  '⟨units-dir⟩/⟨unit name⟩',
  find_program('texlua'),
  args: [ 'test.lua', '--unit="⟨unit name⟩"' ],
  workdir: synctex_test_⟨engine|standalone⟩_⟨units-dir⟩_dir,
  env: synctex_nomalloc,
)
```

```
test(
  '⟨engine|standalone⟩_⟨units_i⟩',
  find_program('texlua'),
  args: [ 'test.lua' ],
  workdir: synctex_test_⟨engine|standalone⟩_⟨units_i⟩,
  env: synctex_no_malloc,
)
```
Then the `test.lua` script must exist, see existing ones to see how they work.

## Testing framework

### Main file

The `test_main.lua` is the entry point of the framework.

### Contents

The testing logic is collected in different files of this directory.
All files are prefixed with `aup_` for practical reasons.
