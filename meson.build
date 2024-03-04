project('synctex', 'c')

version = '1.3'

pkg = import('pkgconfig')

synctex_sources = files(
  'synctex_parser.c',
  'synctex_parser_utils.c',
)

synctex_headers = files(
  'synctex_parser.h',
  'synctex_parser_advanced.h',
  'synctex_parser_utils.h',
  'synctex_version.h',
)

synctex_dependencies = [ dependency('zlib', version: '>=1.2.8') ]
synctex_name = 'synctex'

synctex_library = library(
  synctex_name,
  synctex_sources,
  install: true,
  dependencies: synctex_dependencies
)

install_headers(synctex_headers, subdir: synctex_name)

pkg.generate(
  name: synctex_name,
  version: version,
  libraries: synctex_name,
  subdirs: synctex_name,
  description: 'SyncTeX parser library',
  url: 'http://github.org/jlaurens/synctex',
  install_dir: join_paths(get_option('prefix'), 'lib', 'pkgconfig'),
)

synctex_src = include_directories('.')
synctex_exe = executable('synctex',
  main,
  include_directories: [ synctex_inc, synctex_src ],
  install: true,
  link_with: [ synctex_lib ],
  dependencies: [ zdep ],
  c_args: [ '-D__SYNCTEX_WORK__' ]
)

synctex_no_malloc_env = environment({'MALLOC_PERTURB_': '0'})
synctex_test = meson.current_source_dir() / 'test'

test(
  'big',
  find_program('texlua'),
  args: [ 'test.lua', '--dir=big'],
  workdir: synctex_test,
  env: synctex_no_malloc_env,
)

test(
  'big/1',
  find_program('texlua'),
  args: [ 'test.lua', '--dir=big', '--unit=1'],
  workdir: synctex_test,
  env: synctex_no_malloc_env,
)

test('first test',
  synctex_exe,
  args: [
    'edit',
    '-o',
    '1:100:100:synctex test files/1/edit query/1.pdf'
  ],
  workdir: meson.current_source_dir(),
)
