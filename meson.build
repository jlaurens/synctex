project('synctex', 'c')

version = '1.3'

pkg = import('pkgconfig')

headers = files(
  'synctex_parser.h',
  'synctex_parser_advanced.h',
  'synctex_parser_utils.h',
  'synctex_version.h',
)

sources = files(
  'synctex_parser.c',
  'synctex_parser_utils.c',
)

main = files(
  'synctex_main.c',
)

zdep = dependency('zlib', version: '>=1.2.8')

synctex_inc = include_directories('.')
synctex_lib = library('synctex',
  sources,
  install: true,
  dependencies: [ zdep ],
  include_directories: [ synctex_inc ],
)

synctex_dep = declare_dependency(
  include_directories: [ synctex_inc ],
  link_with: synctex_lib,
)

install_headers(headers)

pkg.generate(
  name: 'synctex',
  version: version,
  libraries: 'synctex',
  subdirs: 'synctex',
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

nomalloc = environment({'MALLOC_PERTURB_': '0'})

test('test',
  find_program('texlua'),
  args: [
    'synctex test files/test.lua'
  ],
  workdir: meson.current_source_dir(),
  env: nomalloc,
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
