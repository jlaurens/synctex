project('synctex', 'c')

version = '1.3'

pkg = import('pkgconfig')

subdir('src')
subdir('include')

zdep = dependency('zlib', version: '>=1.2.8')

synctex_inc = include_directories('include')
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

synctex_src = include_directories('src')
synctex_exe = executable('synctex',
  main,
  include_directories: [ synctex_inc, synctex_src ],
  install: true,
  link_with: [ synctex_lib ],
  dependencies: [ zdep ],
  c_args: [ '-D__SYNCTEX_WORK__' ]
)

test('first test',
  synctex_exe,
  args: [
    'edit',
    '-o',
    '1:100:100:test/1/edit query/1.pdf'
  ],
  workdir: meson.current_source_dir(),
)
