project('synctex', 'c')

version = '1.21'

pkg = import('pkgconfig')

sources = files(
    'synctex_parser.c',
    'synctex_parser_utils.c',
)

zdep = dependency('zlib', version : '>=1.2.8')

lib = library('synctex',
    sources,
    install : true,
    dependencies : [ zdep ]
)

install_headers('synctex_parser.h', subdir : 'synctex')

pkg.generate(
    name : 'synctex',
    version : version,
    libraries : 'synctex',
    subdirs : 'synctex',
    description : 'SyncTeX parser library',
    url : 'http://www.tug.org/synctex',
    install_dir : join_paths(get_option('prefix'), 'lib', 'pkgconfig'),
)

exe = executable('synctex',
    'synctex_main.c',
    install : true,
    link_with : [ lib ],
    dependencies : [ zdep ],
    c_args : [ '-D__SYNCTEX_WORK__' ]
)