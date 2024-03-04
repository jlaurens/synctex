# Automated tests

## Structure

Test units are logically gathered in first level subfolders of this directory. The name of these folders are designated by `⟨units⟩`.

## Declaration

Each folder `⟨units⟩` corresponds to entries in `meson.build`.
We define a function for all the tests
```
```

First some variables:
```
synctex_no_malloc = environment({'MALLOC_PERTURB_': '0'})
synctex_test_⟨units⟩ = meson.current_source_dir() / 'test/⟨units⟩/'
```

For one specific unit named `⟨unit name⟩`, we have
```
test(
  '⟨units⟩/⟨unit name⟩',
  find_program('texlua'),
  args: [ 'test.lua', '--unit="⟨unit name⟩"' ],
  workdir: synctex_test_⟨units⟩,
  env: synctex_nomalloc,
)
```

```
test(
  '⟨units_i⟩',
  find_program('texlua'),
  args: [ 'test.lua' ],
  workdir: synctex_test_⟨units_i⟩,
  env: synctex_no_malloc,
)
```
