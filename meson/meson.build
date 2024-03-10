project('synctex', 'c', version: 'see synctex_version.h')

synctex_dir = meson.current_source_dir() / '..'

fs = import('fs')

foreach line: fs.read(synctex_dir / 'synctex_version.h').split('\n')
  if line.contains('SYNCTEX_VERSION_STRING')
    synctex_version = line.split('"')[1]
  elif line.contains('SYNCTEX_CLI_VERSION_STRING')
    synctex_cli_version = line.split('"')[1]
  endif
endforeach

pkg = import('pkgconfig')

headers = files(
  synctex_dir / 'synctex_parser.h',
  synctex_dir / 'synctex_parser_advanced.h',
  synctex_dir / 'synctex_parser_utils.h',
  synctex_dir / 'synctex_version.h',
)

synctex_sources = files(
  synctex_dir / 'synctex_parser.c',
  synctex_dir / 'synctex_parser_utils.c',
)

synctex_main = files(
  synctex_dir / 'synctex_main.c',
)

zdep = dependency('zlib', version: '>=1.2.8')

synctex_inc = include_directories('..')
synctex_lib = library('synctex',
  synctex_sources,
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
  version: synctex_version,
  libraries: 'synctex',
  subdirs: 'synctex',
  description: 'SyncTeX parser library',
  url: 'http://github.org/jlaurens/synctex',
  install_dir: get_option('prefix') / 'lib' / 'pkgconfig',
)

synctex_exe = executable('synctex',
  synctex_main,
  include_directories: [ synctex_inc ],
  install: true,
  link_with: [ synctex_lib ],
  dependencies: [ zdep ],
  c_args: [ '-DSYNCTEX_STANDALONE' ]
)

synctex_test_exe = executable('synctex_test',
  synctex_main,
  include_directories: [ synctex_inc ],
  install: true,
  link_with: [ synctex_lib ],
  dependencies: [ zdep ],
  c_args: [ '-DSYNCTEX_STANDALONE', '-DSYNCTEX_TEST' ]
)

test('first test',
  synctex_exe,
  args: [
    'edit',
    '-o',
    '1:100:100:synctex test files/1/edit query/1.pdf'
  ],
  workdir: synctex_dir,
)
