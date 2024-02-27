project('synctex', 'c')

version = '1.21'

pkg = import('pkgconfig')

sources = files(
    'synctex_parser.c',
    'synctex_parser_utils.c',
)

headers = files(
    'synctex_parser.h',
    'synctex_parser_advanced.h',
    'synctex_parser_utils.h',
    'synctex_version.h',
)

zdep = dependency('zlib', version : '>=1.2.8')

lib = library('synctex',
    sources,
    install : true,
    dependencies : [ zdep ]
)

install_headers(headers, subdir : 'synctex')

pkg.generate(
    name : 'synctex',
    version : version,
    libraries : 'synctex',
    subdirs : 'synctex',
    description : 'SyncTeX parser library',
    url : 'http://github.org/jlaurens/synctex',
    install_dir : join_paths(get_option('prefix'), 'lib', 'pkgconfig'),
)

exe = executable('synctex',
    'synctex_main.c',
    install : true,
    link_with : [ lib ],
    dependencies : [ zdep ],
    c_args : [ '-D__SYNCTEX_WORK__' ]
)

test('first test',
    exe,
    args : [
        'edit',
        '-o',
        '1:100:100:synctex test files/1/edit query/1.pdf'
    ],
    workdir : meson.current_source_dir(),
)