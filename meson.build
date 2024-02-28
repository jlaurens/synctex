project('synctex', 'c')

version = '1.3'

pkg = import('pkgconfig')

subdir('src')
subdir('include')

zdep = dependency('zlib', version: '>=1.2.8')

inc = include_directories('include')
lib = library('synctex',
  sources,
  install: true,
  dependencies: [ zdep ],
  include_directories: [ inc ],
)

dep = declare_dependency(
  include_directories: [ inc ],
  link_with: lib,
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

src = include_directories('src')
exe = executable('synctex',
  'src/synctex_main.c',
  include_directories: [ inc, src ],
  install: true,
  link_with: [ lib ],
  dependencies: [ zdep ],
  c_args: [ '-D__SYNCTEX_WORK__' ]
)

test('first test',
  exe,
  args: [
    'edit',
    '-o',
    '1:100:100:test/1/edit query/1.pdf'
  ],
  workdir: meson.current_source_dir(),
)
