---
layout: steps
title: The Strom class
partnum: 8
subpart: 1
description: The `Strom` class encapsulates the program itself, providing "global" variables and access to command line options.
---
By the time you are finished with this lesson, your program will be able to obtain the name of the tree file to be processed from the user via command line options. It will also be possible to obtain the program version and help using command line switches.

Create a new header file named {% indexfile strom.hpp %} containing the following code:
~~~~~~
{{ "steps/step-08/src/strom.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## Constructor
~~~~~~
{{ "steps/step-08/src/strom.hpp" | polcodesnippet:"begin_constructor-end_constructor","" }}
~~~~~~
{:.cpp}
The constructor contains an output statement that notifies us when a `Strom` object is created. Note that this line is commented out. Constructor and destructor comments will be commented out from now on, but feel free to uncomment them if, for debugging purposes, you ever need to know when objects of a particular class are created or destroyed. Please do not comment out the `clear` function call, however. That ensures that a just-constructed `Strom` object is in a consistent state. The `clear` function may be used to return a `Strom` object to its just-constructed state if desired.

## Destructor
~~~~~~
{{ "steps/step-08/src/strom.hpp" | polcodesnippet:"begin_destructor-end_destructor","" }}
~~~~~~
{:.cpp}
Our usual destructor function just notifies us (if its single line is uncommented) that a `Strom` object has been destroyed.

## The clear function
~~~~~~
{{ "steps/step-08/src/strom.hpp" | polcodesnippet:"begin_clear-end_clear","" }}
~~~~~~
{:.cpp}
The `clear` function currently just sets each of the two non-static data members to the empty string.

## The processCommandLineOptions function
~~~~~~
{{ "steps/step-08/src/strom.hpp" | polcodesnippet:"begin_processCommandLineOptions-end_processCommandLineOptions","" }}
~~~~~~
{:.cpp}
This function does the work of extracting information provided by the user on the command line.

This function begins by creating two objects defined by the Boost `program_options` library (which is available because we included the {% indexfile boost/program_options.hpp %} header at the top of the {% indexfile strom.hpp %} file):

The `desc` object stores information about the available program options. This object figures out whether what the user types is valid, and can provide a usage string that shows the user what options are available and what to type to specify each option.

The `vm` object is what actually stores the information provided by the user.

Our function then adds options to the `desc` object. Options that are stand-alone (e.g. "help" and "version") are simply specified by providing the name of the option, a single-letter abbreviation (if desired), and a string description of the option. For example, we specify the "help" option by providing "help" as the name, "h" as the abbreviation, and "produce help message" as the description. If the user desired help, she or he could type either `-h` or `--help` on the command line when running the program and obtain string descriptions of each option along with what tokens to type in order to activate that option. The convention is that single letter abbreviations of program options are preceded by a single hyphen, while program option full names are preceded by two hyphens.

The treefile option is more complicated because it will be followed by a string (the tree file name). We will also go ahead and provide for specifying a data file name on the command line, even though our program cannot yet read data files. The code
~~~~~~
boost::program_options::value(&_data_file_name)
~~~~~~
{:.bash-output}
provides the memory address (the `&` symbol means "address of") where the value provided by the user will be stored. The program options library can tell from the fact that the variable `_data_file_name` is an object of type `std::string` that it should expect the user to type a string at the command line for this option. 
{% comment %}
The program will abort with an error message if the user tries to enter, say, a number instead of a string for either the `datafile` or the `treefile` option.
{% endcomment %}

Note that the `value` function returns an object that has associated member functions that may be called. In this case, we can call the member function `required()` for the `treefile` option. This has the effect of ensuring that the user supplies a value for this option. If this option is not provided, the program will abort and tell the user that a value must be supplied for the `treefile` option.

The `parse_command_line` function call takes the supplied command line arguments (`argc` holds the number of these, while `argv` is an array of strings that holds what the user typed), along with the `desc` object, and parses the options, storing the results in the `vm` object.

The `try`/`catch` block encloses a call to the `parse_config_file` function. This function attempts to read a file named {% indexfile strom.conf %} containing command line options in the form _key_ = _value_, one per line. If the {% indexfile strom.conf %} file is not found, the `catch` block is entered and a note is issued to the user letting them know that the {% indexfile strom.conf %} file was not found. This is not an error, since the user may enter all options on the command line, but when the number of options grows large, being able to store them all in a config file is very convenient for the user, avoiding really long, complicated command line invocations.

You will recognize the first two arguments to the `parse_config_file` function: it is obvious that this function needs the name of the config file to search for ("{% indexfile strom.conf %}") and it needs the `desc` object so that it knows what options to expect in the config file, but what about that third argument: `false`? This last argument tells the `parse_config_file` function that we do not want to allow any options in the configuration file that have not been defined in the `desc` object. If the program discovers `sample_every = 100` in the configuration file but `sample_every` has not been specified in the `desc` object, then `strom` will refuse to run. This is good behavior for now because it prevents users from thinking that an option is being used and understood by strom when in fact that line in the config file is being ignored.

The `notify` function call takes the parsed command line, now stored in the map variable `vm`, and sets the appropriate `Strom` object data members. For example, `_data_file_name` will be set to the value stored in `vm["datafile"]` and `_tree_file_name` will be set to the value stored in `vm["treefile"]`. You could do this yourself (e.g. `_data_file_name = vm["datafile"].as<std::string>();`), but the notify function makes it easy.

After calling `parse_command_line` and `notify`, we first check to see if the user asked for help. If so, we ignore any other command line options that may have been supplied by the user and show the usage string generated when a `desc` object is output.

Assuming that the user did not type `-h` or `--help`, we next check to see whether `-v` or `--version` was specified. If so, we again ignore any other command line options and output the program version number and then exit.

If the user did not ask for either help or version, we must assume they wish to run the program. In order for the program to run, we need to have a tree file name. If this was not specified, the program exits with a suitable error message.

## The run function
The `Strom::run` function can now take over all the duties that the main function performed before. Because `_data_file_name` and `_tree_file_name` are data members, and we've guaranteed that the user has supplied a value for the tree file name on the command line, we are free to use `_tree_file_name` in place of the hard coded literal string we used before.
~~~~~~
{{ "steps/step-08/src/strom.hpp" | polcodesnippet:"begin_run-end_run","" }}
~~~~~~
{:.cpp}

