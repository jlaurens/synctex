
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

# Testing requires a preliminary step to gather the temporary files in one place.
# We have a command to write a uuid in a text file in the build folder.
# This uuid is used by the `AUP.units` object.

generate_uuid_py = 'utils/generate_uuid.py'

python_exe = find_program('python3', 'python')
uuid_txt = 'uuid.txt'
generated_uuid_txt = meson.current_build_dir() / uuid_txt

r = run_command(
  python_exe,
  generate_uuid_py,
  generated_uuid_txt,
  check: true)
if r.returncode() != 0
    error('No uuid available')
endif

generate_uuid_tgt = custom_target(
  'uuid',   # The *unique* id of the custom target
  build_always_stale : true,                                       # If `true` the target is always considered out of date
  build_by_default   : false,                                       # Causes, when set to true, to
  capture            : false,                                       # There are some compilers that can't be told to write
  input: [generate_uuid_py],
  command            : [python_exe,  '@INPUT@',  generated_uuid_txt, 'TARGET'],  # Command to run to create outputs from inputs
  install            : false,                                       # When true, one or more files of this target are installed during the install step (see `install_dir` for details)
  output             : 'fake'
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

if not fs.is_dir(synctex_auplib_doc_dir)
  message(synctex_auplib_doc_dir+' does not exist')
  runresult = run_command(
    texlua,
    synctex_auplib_dir / 'aup_doc_main.lua',
    '--build_dir="'+meson.current_build_dir()+'"',
    check: true
)
endif

shared_library_env = environment()
shared_library_env.set('MALLOC_PERTURB_', '0')
shared_library_env.prepend('PATH', meson.current_build_dir())
shared_library_args = [
  'test_main.lua',
  '--uuid_txt="'+generated_uuid_txt+'"',
]

test(
  'uuid',
  texlua,
  args: shared_args + ['--update_uuid="'+meson.current_source_dir() / generate_uuid_py+'"'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 10000,
)


shared_library_args = shared_args + '--library'

test(
  'library/mark',
  texlua,
  args: shared_library_args + '--suite=mark',
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 2,
)

test(
  'library/mark/...',
  texlua,
  args: shared_library_args + ['--suite=mark', '--unit=base'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 2,
)

test(
  'library/dump',
  texlua,
  args: shared_library_args + '--suite=dump',
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 2,
)

test(
  'library/dump/...',
  texlua,
  args: shared_library_args + ['--suite=dump', '--unit=form1'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 2,
)

test(
  'library/gh',
  texlua,
  args: shared_library_args + '--suite=gh',
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 3,
)

test(
  'library/gh/...',
  texlua,
  args: shared_library_args + ['--suite=gh', '--unit=gh78'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 4,
)

test(
  'library/input',
  texlua,
  args: shared_library_args + '--suite=input',
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 5,
)

test(
  'library/input/...',
  texlua,
  args: shared_library_args + ['--suite=input', '--unit=many'],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 6,
)

shared_engine_env = environment()
shared_engine_env.set('MALLOC_PERTURB_', '0')
shared_engine_args = shared_args + '--engine'

test(
  'engine/gh',
  texlua,
  args: shared_engine_args + '--suite=gh',
  workdir: synctex_test_dir,
  env: shared_engine_env,
  priority: 5,
)

test(
  'engine/gh/...',
  texlua,
  args: shared_engine_args + ['--suite=gh', '--unit=gh30'],
  workdir: synctex_test_dir,
  env: shared_engine_env,
  priority: 6,
)

test(
  'engine/global_options',
  texlua,
  args: shared_engine_args + ['--suite=global_options'],
  workdir: synctex_test_dir,
  env: shared_engine_env,
  priority: 6,
)

test(
  'library/foo/bar',
  texlua,
  args: shared_library_args + [
    '--suite="foo"',
    '--unit="bar"'
  ],
  workdir: synctex_test_dir,
  env: shared_library_env,
  priority: 7,
)

test('first test',
  synctex_exe,
  args: [
    'edit',
    '-o',
    '1:100:100:synctex test files/1/edit query/1.pdf'
  ],
  workdir: meson.current_source_dir() / synctex_dir,
  priority: 8,
)
