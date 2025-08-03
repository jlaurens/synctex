
# This file is useless outside of the SyncTeX package
# See the README.md in that directory

project('synctex', 'c', version: 'see synctex_version.h')

synctex_dir = '..'

cc = meson.get_compiler('c')
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

# for PathFindExtension
if host_machine.system() == 'windows'
  shlwapi = cc.find_library('shlwapi')
else
  shlwapi = []
endif

synctex_inc = include_directories(synctex_dir)
synctex_name = 'synctex'
synctex_lib = library(synctex_name,
  synctex_sources,
  install: true,
  dependencies: [ shlwapi, zdep ],
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


test_main_lua = '../test standalone/test_main.lua'

dot_exe = ''
if build_machine.system() == 'windows'
  dot_exe = '.exe'
endif
texlua = find_program('texlua'+dot_exe)

synctex_test_exe = executable('synctex_test',
  synctex_main,
  include_directories: [ synctex_inc ],
  install: false,
  link_with: [ synctex_lib ],
  dependencies: [ zdep ],
  c_args: [ '-DSYNCTEX_STANDALONE', '-DSYNCTEX_TEST' ]
)

base_args = [
  '--build_os="'+build_machine.system()+'"',
  '--build_dir="'+meson.current_build_dir()+'"',
]

session_id_target = custom_target(
  'session_id',
  build_always_stale : true,
  command            : [ texlua, meson.current_source_dir() / test_main_lua, '--setup']+base_args,
  output             : 'session_id'
)

shared_library_env = environment()
shared_library_env.set('MALLOC_PERTURB_', '0')
shared_library_env.prepend('PATH', meson.current_build_dir())

synctex_test_dir = meson.current_source_dir() / synctex_dir / 'test standalone'

synctex_auplib_dir = synctex_test_dir / 'auplib' / ''
synctex_auplib_doc_dir = synctex_auplib_dir / 'doc'

if not fs.is_dir(synctex_auplib_doc_dir)
  message(synctex_auplib_doc_dir+' does not exist')
  runresult = run_command(
    texlua,
    synctex_auplib_dir / 'aup_doc_main.lua',
    '--build_dir="'+meson.current_build_dir()+'"',
    check: true
)
endif

# Whether the synctex executable is available
# Uses the old test files
test(
  'first test',
  synctex_exe,
  args: [
    'edit',
    '-o',
    '1:100:100:synctex test files/1/edit query/1.pdf'
  ],
  workdir: meson.current_source_dir() / synctex_dir,
  priority: 100000,
  is_parallel : true,
  depends: [session_id_target]
)
test(
  'second test',
  synctex_test_exe,
  args: [
    'edit',
    '-o',
    '1:100:100:synctex test files/1/edit query/1.pdf'
  ],
  workdir: meson.current_source_dir() / synctex_dir,
  priority: 100000,
  is_parallel : true,
  depends: [session_id_target],
)

shared_library_env = environment()
shared_library_env.set('MALLOC_PERTURB_', '0')
shared_library_env.prepend('PATH', meson.current_build_dir())

shared_args = [
  'test_main.lua',
] + base_args

shared_library_args = shared_args + '--library'

name = 'library/config'
test(
  name,
  texlua,
  args: shared_library_args + ['--name="'+name+'"'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 100000-2111,
  is_parallel : true,
  depends: [session_id_target],
)

name = 'library/dump/...'
test(
  name,
  texlua,
  args: shared_library_args + ['--name="'+name+'"', '--suite=dump', '--unit=form1'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 100000-2221,
  is_parallel : true,
  depends: [session_id_target],
)

name = 'library/dump'
test(
  name,
  texlua,
  args: shared_library_args + ['--name="'+name+'"', '--suite=dump'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 100000-2211,
  is_parallel : true,
  depends: [session_id_target],
)

name = 'library/mark/...'
test(
  name,
  texlua,
  args: shared_library_args + ['--name="'+name+'"', '--suite=mark', '--unit=base'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 100000-2321,
  is_parallel : true,
  depends: [session_id_target],
)

name = 'library/mark'
test(
  name,
  texlua,
  args: shared_library_args + ['--name="'+name+'"', '--suite=mark'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 100000-2311,
  is_parallel : true,
  depends: [session_id_target],
)

name = 'library/input'
test(
  name,
  texlua,
  args: shared_library_args + ['--name="'+name+'"', '--suite=input'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 100000-2411,
  is_parallel : true,
  depends: [session_id_target],
)

name = 'library/input/...'
test(
  name,
  texlua,
  args: shared_library_args + ['--name="'+name+'"', '--suite=input', '--unit=many'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 100000-2421,
  is_parallel : true,
  depends: [session_id_target],
)

name = 'library/gh/...'
test(
  name,
  texlua,
  args: shared_library_args + ['--name="'+name+'"', '--suite=gh', '--unit=gh78'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 100000-2511,
  is_parallel : true,
  depends: [session_id_target],
)

name = 'library/gh'
test(
  name,
  texlua,
  args: shared_library_args + ['--name="'+name+'"', '--suite=gh'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 100000-2521,
  is_parallel : true,
  depends: [session_id_target],
)

shared_engine_env = environment()
shared_engine_env.set('MALLOC_PERTURB_', '0')
shared_engine_args = shared_args + '--engine'

name = 'engine/minimal'
test(
  name,
  texlua,
  args: shared_engine_args + ['--name="'+name+'"', '--suite=minimal'],
  workdir: synctex_test_dir,
  env: shared_engine_env,
  priority: 100000-1111,
  is_parallel : true,
  depends: [session_id_target],
)

name = 'engine/gh/...'
test(
  name,
  texlua,
  args: shared_engine_args + ['--name="'+name+'"', '--suite=gh', '--unit=gh30-84'],
  workdir: synctex_test_dir,
  env: shared_engine_env,
  priority: 100000-1211,
  is_parallel : true,
  depends: [session_id_target],
)

name = 'engine/gh'
test(
  name,
  texlua,
  args: shared_engine_args + ['--name="'+name+'"', '--suite=gh'],
  workdir: synctex_test_dir,
  env: shared_engine_env,
  priority: 100000-1221,
  is_parallel : true,
  depends: [session_id_target],
)

name = 'engine/global_options'
test(
  name,
  texlua,
  args: shared_engine_args + ['--name="'+name+'"', '--suite=global_options'],
  workdir: synctex_test_dir,
  env: shared_engine_env,
  priority: 100000-1311,
  is_parallel : true,
  depends: [session_id_target],
)

# Testing for development:
# The `AUP` testing framework recognizes some special arguments
# Calling sequence:
# meson test -C build --test-args='--debug=10000 --local...=... --dev_suite=... --dev_unit=...' "dev" 

shared_dev_env = environment()
shared_dev_env.set('MALLOC_PERTURB_', '0')
shared_dev_args = shared_args + '--dev'

# Testing the pdftex engine
# This is useful when developing pdftex
# Usage from the directory containing `meson.build`:
#
#  meson test -C Build --test-args="--debug=10 --local=JL" "dev"
#
# Explanation of test arguments
# --local=JL Read the `local/.../.../user/JL.lua`
#
# Other engines are tested as well
#
name = 'dev'
test(
  name,
  texlua,
  args: shared_dev_args + ['--name="'+name+'"'],
  workdir: synctex_test_dir,
  env: shared_dev_env,
  priority: 100000-3000,
  is_parallel : true,
  depends: [session_id_target],
)

name = 'last test'
test(
  name,
  texlua,
  args: shared_args + [ '--name="'+name+'"', '--teardown'],
  workdir: synctex_test_dir,
  priority: 100000-10000,
  is_parallel : true,
  depends: [session_id_target],
)

name = 'parse decimal integer'
test_parse_int_exe = executable(
  name,
  synctex_dir / 'test C' / 'test_parse_int.c',
  include_directories: [ synctex_inc ],
  install: false,
  link_with: [ synctex_lib ],
  dependencies: [ zdep ]
)
test(
  'Parse decimal integers',
  test_parse_int_exe
)
