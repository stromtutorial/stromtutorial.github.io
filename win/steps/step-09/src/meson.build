project('strom', 'cpp', ###start
	default_options : ['cpp_std=c++11','cpp_link_args=-static','prefix=/home/paul/Documents/strom/distr'],
	version : '1.0')
cpp = meson.get_compiler('cpp')
lib_filesystem = cpp.find_library('boost_filesystem', dirs: ['/home/paul/lib/static'], required: true)  ###!a
lib_program_options = cpp.find_library('boost_program_options', dirs: ['/home/paul/lib/static'], required: true)
lib_ncl = cpp.find_library('ncl', dirs: ['/home/paul/lib/staticlib/static'], required: true)
incl_ncl = include_directories('/home/paul/include')
incl_boost = include_directories('/home/paul/Documents/libraries/boost_1_71_0')
executable('strom', 'main.cpp', install: true, install_dir: '.', dependencies: [lib_ncl,lib_program_options,lib_filesystem], include_directories: [incl_ncl,incl_boost])    ###!b
install_data('strom.conf', install_dir: '.')    ###!c
install_data('test.tre', install_dir: '.')
install_data('rbcL.nex', install_dir: '.')  ###!near_end
install_data('datatest.nex', install_dir: '.')  ###!end
