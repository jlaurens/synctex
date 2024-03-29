
# This file is useless outside of the SyncTeX package
# See the README.md

project('synctex', 'c', version: 'see synctex_version.h')

synctex_dir = '..'

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

synctex_inc = include_directories(synctex_dir)
synctex_name = 'synctex'
synctex_lib = library(synctex_name,
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

synctex_name = 'synctex'

pkg.generate(
  name: synctex_name,
  version: synctex_version,
  libraries: synctex_name,
  subdirs: synctex_name,
  description: 'SyncTeX parser library',
  url: 'http://github.org/jlaurens/synctex',
  install_dir: get_option('prefix') / 'lib' / 'pkgconfig',
)

synctex_exe = executable(synctex_name,
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
  install: false,
  link_with: [ synctex_lib ],
  dependencies: [ zdep ],
  c_args: [ '-DSYNCTEX_STANDALONE', '-DSYNCTEX_TEST' ]
)

dot_exe = ''
if build_machine.system() == 'windows'
  dot_exe = '.exe'
endif
texlua = find_program('texlua'+dot_exe)

synctex_no_malloc = environment({'MALLOC_PERTURB_': '0'})
synctex_test_dir = meson.current_source_dir() / synctex_dir / 'test standalone'

synctex_auplib_dir = synctex_test_dir / 'auplib' / ''
synctex_auplib_doc_dir = synctex_auplib_dir / 'doc'

fs = import('fs')
if not fs.is_dir(synctex_auplib_doc_dir)
  message(synctex_auplib_doc_dir+' does not exist')
  runresult = run_command(
    texlua,
    synctex_auplib_dir / 'aup_doc_main.lua',
    '--build_dir="'+meson.current_build_dir()+'"',
    check: true
)
endif


shared_library_args = [
  'test_main.lua',
  '--build_dir="'+meson.current_build_dir()+'"',
  '--library'
]

test(
  'library/gh',
  texlua,
  args: shared_library_args + '--suite=gh',
  workdir: synctex_test_dir,
  env: synctex_no_malloc,
)

test(
  'library/gh/...',
  texlua,
  args: shared_library_args + ['--suite=gh', '--unit=gh78'],
  workdir: synctex_test_dir,
  env: synctex_no_malloc,
)

shared_engine_args = [
  'test_main.lua',
  '--build_dir="'+meson.current_build_dir()+'"',
  '--engine'
]

test(
  'engine/gh',
  texlua,
  args: shared_engine_args + '--suite=gh',
  workdir: synctex_test_dir,
  env: synctex_no_malloc,
)

test(
  'engine/gh/...',
  texlua,
  args: shared_engine_args + ['--suite=gh', '--unit=gh30'],
  workdir: synctex_test_dir,
  env: synctex_no_malloc,
)

test(
  'library/foo/bar',
  texlua,
  args: shared_library_args + [
    '--suite="foo"',
    '--unit="bar"'
  ],
  workdir: synctex_test_dir,
  env: synctex_no_malloc,
)

test('first test',
  synctex_exe,
  args: [
    'edit',
    '-o',
    '1:100:100:synctex test files/1/edit query/1.pdf'
  ],
  workdir: meson.current_source_dir() / synctex_dir,
)
