project('parser', 'c', version: '0.1', license: 'GPL')

add_global_arguments('-O3', '-Wall', '-Wextra', '-pedantic',
                    '-Wold-style-definition', '-Iinclude/', language: 'c')

sources = ['src/main.c', 'src/listener.c', 'src/parser.c', 'src/util.c']

executable('ids', sources, include_directories : include_directories('include'))
