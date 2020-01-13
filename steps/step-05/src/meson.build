project('strom', 'cpp', ###start
	default_options : ['cpp_std=c++11','cpp_link_args=-static','prefix=/home/paul/Documents/strom/distr'],
	version : '1.0')
incl_boost = include_directories('/home/paul/Documents/libraries/boost_1_71_0')
executable('strom', 'main.cpp', install: true, install_dir: '.', include_directories: incl_boost)   ###end
