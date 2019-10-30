project('strom', 'cpp',
	default_options : ['cpp_std=c++11','cpp_link_args=-static','prefix=/home/paul/Documents/strom/distr'],
	version : '1.0')
cpp = meson.get_compiler('cpp')
lib_ncl = cpp.find_library('ncl', dirs: ['/home/paul/lib/static'], required: true) ###a
incl_ncl = include_directories('/home/paul/include')                            ###b
incl_boost = include_directories('/home/paul/Documents/libraries/boost_1_71_0')
executable('strom', 'main.cpp', install: true, install_dir: '.', dependencies: lib_ncl, include_directories: [incl_ncl,incl_boost])
install_data('test.tre', install_dir: '.')
