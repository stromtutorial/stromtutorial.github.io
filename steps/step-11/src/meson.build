project('strom', 'cpp',
	default_options : ['cpp_std=c++11','prefix=/home/paul/Documents/strom/distr'],
	version : '1.0')
cpp = meson.get_compiler('cpp')
lib_filesystem = cpp.find_library('boost_filesystem', dirs: ['/home/paul/lib/static'], required: true)
lib_program_options = cpp.find_library('boost_program_options', dirs: ['/home/paul/lib/static'], required: true)
lib_ncl = cpp.find_library('ncl', dirs: ['/home/paul/lib/static'], required: true)
lib_beagle = cpp.find_library('hmsbeagle', dirs: ['/home/paul/lib'], required: true)
incl_beagle = include_directories('/home/paul/include/libhmsbeagle-1')
incl_ncl = include_directories('/home/paul/include')
incl_boost = include_directories('/home/paul/Documents/libraries/boost_1_71_0')
incl_eigen = include_directories('/home/paul/Documents/libraries/eigen-eigen-323c052e1731') ###!a
executable('strom', 'main.cpp', install: true, install_dir: '.', dependencies: [lib_beagle,lib_ncl,lib_program_options,lib_filesystem], include_directories: [incl_beagle,incl_ncl,incl_boost,incl_eigen])  ###!b
install_data('strom.conf', install_dir: '.')
#install_data('test.tre', install_dir: '.')
install_data('rbcL.nex', install_dir: '.')
#install_data('datatest.nex', install_dir: '.')
install_data('rbcLjc.tre', install_dir: '.')
install_data('go.sh', install_dir: '.')
