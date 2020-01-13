project('strom', 'cpp', ###start
	default_options : ['cpp_std=c++11','prefix=/home/pol02003/tut/distr'],  ###!x
	version : '1.0')
cpp = meson.get_compiler('cpp')
lib_filesystem = cpp.find_library('boost_filesystem', dirs: ['/home/pol02003/lib/static'], required: true)
lib_program_options = cpp.find_library('boost_program_options', dirs: ['/home/pol02003/lib/static'], required: true)
lib_ncl = cpp.find_library('ncl', dirs: ['/home/pol02003/lib/static'], required: true)
lib_beagle = cpp.find_library('hmsbeagle', dirs: ['/home/pol02003/lib'], required: true)    ###!a
incl_beagle = include_directories('/home/pol02003/include/libhmsbeagle-1')                  ###!b
incl_ncl = include_directories('/home/pol02003/include')
incl_boost = include_directories('/home/pol02003/boost_1_70_0')
executable('strom', 'main.cpp', install: true, install_dir: '.', dependencies: [lib_beagle,lib_ncl,lib_program_options,lib_filesystem], include_directories: [incl_beagle,incl_ncl,incl_boost]) ###!c
install_data('strom.conf', install_dir: '.')
#install_data('test.tre', install_dir: '.')
install_data('rbcL.nex', install_dir: '.')
#install_data('datatest.nex', install_dir: '.')
install_data('rbcLjc.tre', install_dir: '.')    ###!e
install_data('go.sh', install_dir: '.')  ###!d
