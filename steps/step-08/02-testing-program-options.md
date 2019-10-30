---
layout: steps
title: Testing program options
partnum: 8
subpart: 2
description: Test the command line option capability by specifying the tree file to be processed on the command line.
---
{% assign OS = site.operatingsystem %}
Modify {% indexfile main.cpp %} to contain the following code.
~~~~~~
{{ "steps/step-08/src/main.cpp" | polcodesnippet:"start-end","" }}
~~~~~~
{:.cpp}

The main function now creates a `Strom` object, then passes `argc` and `argv` off to the `Strom` object's `processCommandLineOptions` function, and finally calls the `Strom` object's `run` method. Any exceptions thrown are caught and their error messsages displayed to the user. 

The {% indexfile main.cpp %} file will not change very much from now on. Any new options will be added to the `Strom` class instead.

## Static data members

Some data members (`_program_name`, `_major_version`, and `_minor_version`) of the `Strom` class were declared static. We must therefore initialize these in {% indexfile main.cpp %}, as it is our only source code file, just like we have been doing for `Node::_smallest_edge_length`.

## Compiling the Boost `program_options` library
The extra capability we've added to our program requires us to:
* compile the Boost `program_options` library,
* tell our development environment where to find it for linking purposes
Let's take these one at a time

### Compile the Boost `program_options` library

{% if OS == "linux" %}
[//]: ################################### LINUX ##########################################
Navigate to where you previously ([in step 5]({{ site.baseurl }}/steps/step-05/02-test-makenewick.html)) installed the Boost headers (_~/Documents/libraries/boost_1_71_0_) and enter the following command:
~~~~~~
./bootstrap.sh --with-toolset=gcc --with-libraries=program_options,filesystem,system
~~~~~~
{:.bash}

The {% indexfile bootstrap.sh %} script will fairly quickly return, instructing you to type `./b2` to perform the actual compilation.
~~~~~~
./b2 cxxflags="-std=c++11" -d 2
~~~~~~
{:.bash}
Note that I've added `cxxflags="-std=c++11"` to ensure that the Boost libraries are compiled under the same C++ _dialect_ used to compile the main strom program. This will ensure that the libraries can be linked to the strom executable. I've also specified `-d 2` to specify debugging level 2, which causes the Boost build system to display the compile and link commands it is using (this helps when trying to diagnose what happened if the compiling or linking fails).

Assuming the compilation was successful, you should now see something similar to the following output:
~~~~~
The Boost C++ Libraries were successfully built!

The following directory should be added to compiler include paths:

    /home/plewis/Documents/libraries/boost_1_71_0

The following directory should be added to linker library paths:

    /home/plewis/Documents/libraries/boost_1_71_0/stage/lib
~~~~~
{:.bash-output}
Copy the files {% indexfile libboost_program_options.a %}, {% indexfile libboost_filesystem.a %}, and {% indexfile libboost_system.a %} to the ~/lib/static folder.

{% elsif OS == "mac" %}
[//]: ################################### MAC ##########################################
Open the Terminal or iTerm2 app and navigate to where you previously installed the Boost headers (_~/Documents/libraries/boost_1_71_0_), then enter the following command:
~~~~~~
./bootstrap.sh --with-toolset=clang --with-libraries=program_options,filesystem,system
~~~~~~
{:.bash}

Note that I've added `filesystem` and `system` to the list of Boost libraries to compile. We will make use of these later, so we might as well compile them now so that we do not need to rerun the {% indexfile bootstrap.sh %} script again.

The {% indexfile bootstrap.sh %} script will fairly quickly return, instructing you to type `./b2` to perform the actual compilation.
~~~~~~
./b2 cxxflags="-std=c++11" -d 2
~~~~~~
{:.bash}
Note that I've added `cxxflags="-std=c++11"` to ensure that the Boost libraries are compiled under the same C++ _dialect_ used to compile the main strom program. This will ensure that the libraries can be linked to the strom executable. I've also specified `-d 2` to specify debugging level 2, which causes the Boost build system to display the compile and link commands it is using (this helps when trying to diagnose what happened if the compiling or linking fails).

Assuming the compilation was successful, you should now see something similar to the following output:
~~~~~
The Boost C++ Libraries were successfully built!

The following directory should be added to compiler include paths:

    /home/plewis/Documents/libraries/boost_1_71_0

The following directory should be added to linker library paths:

    /home/plewis/Documents/libraries/boost_1_71_0/stage/lib
~~~~~
{:.bash-output}
Copy the files {% indexfile libboost_program_options.a %}, {% indexfile libboost_filesystem.a %}, and {% indexfile libboost_system.a %} to the ~/lib/static folder.

{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################
You have previously ([step 5]({{ site.baseurl }}{% link steps/step-05/02-test-makenewick.md %})) downloaded and unpacked the Boost C++ library into your libraries directory; however, some of the libraries within Boost that we will be using need to be compiled, so let’s do that now. Start by double-clicking the {% indexfile bootstrap.bat %} file inside your `BOOST_ROOT` directory. This will create the compile engine that Boost uses (called {% indexfile b2.exe %}) and will deposit a file named {% indexfile project-config.jam %} in the `BOOST_ROOT` directory. This file will contain 3 lines, but edit it (use [NotePad++](https://notepad-plus-plus.org/) for this) and add one additional line that specifies the libraries we will need to compile (we only need _program_options_ now, but we'll go ahead and compile _filesystem_ and _system_ now to save having to do this later):
~~~~~~
import option ; 
using msvc ; 
option.set keep-going : false ; 
libraries = --with-program_options --with-filesystem --with-system ;
~~~~~~
{:.bash-output}
*Important!* Be sure to leave a space before the semicolon (`;`) that terminates each line.

Not adding the “libraries” line will result in a very long wait because Boost will proceed to compile all libraries that need to be compiled. There is nothing wrong with compiling everything (you may need it later), so feel free to just use the default {% indexfile project-config.jam %} if you want.

To start the compile process, shift-right-click on the {% indexfile boost_1_71_0 %} directory and choose _Open PowerShell window here_ from the popup menu, then type
~~~~~~
.\b2.exe cxxflags="/std:c++14" -d 2
~~~~~~
{:.bash-output}
If all goes well, you should see something similar to the following output when the program exits:
~~~~~~
The Boost C++ Libraries were successfully built!

The following directory should be added to compiler include paths:

    C:\Users\Paul Lewis\Documents\libraries\boost_1_71_0

The following directory should be added to linker library paths:

    C:\Users\Paul Lewis\Documents\libraries\boost_1_71_0\stage\lib
~~~~~~
{:.bash-output}

### Create a directory to store static libraries

Create a new subdirectory of your libraries directory and name it {% indexfile static %}. Copy the following 6 files from the {% indexfile boost_1_71_0\stage\lib %} directory to the new {% indexfile static %} directory. 
~~~~~~
libboost_program_options-vc142-mt-gd-x64-1_71.lib
libboost_program_options-vc142-mt-x64-1_71.lib

libboost_filesystem-vc142-mt-gd-x64-1_71.lib
libboost_filesystem-vc142-mt-x64-1_71.lib

libboost_system-vc142-mt-gd-x64-1_71.lib
libboost_system-vc142-mt-x64-1_71.lib
~~~~~~
{:.bash-output}
These are the debug (containing "gd" in the name) and release versions of the 3 Boost libraries we compiled. These files that I am asking you to copy are the 64-bit versions (Boost also compiled 32-bit versions), so I am presuming you are using a 64-bit version of Windows here.

{% comment %}
### Tell VSC19 which libraries to link

Right-click on the _strom_ project in the Solution Explorer and choose _Properties_. In the Configuration Properties, choose _Linker > Input > Additional Dependencies_. Click on the button at the end of the _Additional Dependencies_ row and click on the _&lt;Edit...&gt;_ button. Now add the names of the three static libraries you just created in the text box provided (one per line):
~~~~~~
libboost_filesystem-vc142-mt-x64-1_71.lib
libboost_system-vc142-mt-x64-1_71.lib
libboost_program_options-vc142-mt-x64-1_71.lib
~~~~~~
{:.bash-output}
{% endcomment %}

### Tell VSC19 where the libraries are located

VSC19 needs to know where to find the Boost libraries that you compiled. Open the _Configuration Properties_ dialog box again and choose _VC++ Directories > Library Directories_. Click the button at the end of the row to reveal the _&lt;Edit...&gt;_ button, click that to edit, and add the path to your static library directory. For example, I entered this:
~~~~~~
C:\Users\Paul Lewis\Documents\libraries\static
~~~~~~
{:.bash-output}

{% endif %}
[//]: ################################# ENDIF ######################################

{% if OS == "linux" %}
[//]: ################################### LINUX ##########################################
### Tell meson where to find {% indexfile libboost_program_options.a %} for linking purposes
Here is the meson.build file we've been building up, with the highlighted lines either added or modified to accommodate program options:
~~~~~~
{{ "steps/step-08/src/meson.build" | polcodesnippet:"","a,b-c" }}
~~~~~~
{:.meson}
Note the meson variable `lib_program_options` stores the location of the {% indexfile libboost_program_options.a %}, and this variable is added to the `lib_ncl` variable to form a list that is supplied to the `dependencies` argument to the executables command.

The last line in the {% indexfile meson.build %} file copies the {% indexfile test.tre %} file to the {% indexfile distr %} directory so that it will be available in the same directory as the strom executable when you test the program.

{% comment %}
### Tell strom where to find {% indexfile libboost_program_options.a %} at run time

Modify your {% indexfile gostrom.sh %} script in the install directory ({% indexfile ~/Documents/strom/distr %}) to allow the {% indexfile libboost_program_options.so %} file to be found at run time. Note that I've also added {% indexcode $@ %} after `./strom` on the last line. This causes all command line arguments passed into the {% indexfile gostrom.sh %} script to be passed along to `strom` itself.
~~~~~~
#!/bin/bash

export LD_LIBRARY_PATH="$HOME/Documents/libraries/boost_1_71_0/stage/lib:$HOME/lib/ncl"
./strom $@
~~~~~~
{:.bash-output}
{% endcomment %}

{% elsif OS == "mac" %}
[//]: ################################### MAC ##########################################
### Tell Xcode about program options

In the Xcode main menu, choose _View_ > _Navigators_ > _Show Project Navigator_, then click on the {% indexshow blue project icon %} :blueproject: labeled _strom_ at the top of the Project Navigator. Click on the _Build Phases_ tab. Click the `+` sign in _Link Binary With Libraries_ and (after clicking the _Add Other..._ button) navigate to the directory {% indexfile ~/lib/static %} and select the _libboost_program_options.a_, _libboost_filesystem.a_, and _libboost_system.a_ files. This informs Xcode where to find the compiled library code for purposes of linking the library into your program's executable file.

You have already specified the `$(BOOST_ROOT)` custom path in the "Header Search Paths" section of Build Settings, and you've specified {% indexfile ~/lib/static %} in the Library Search Paths section of Build Settings, so there is nothing more that needs to be done.

{% elsif OS == "win" %}
[//]: ################################### WINDOWS ######################################
{% endif %}

## Running strom
{% if OS == "linux" %}
[//]: ################################### LINUX ######################################
If you run the program now by typing simply `./strom`, from your {% indexfile ~/Documents/strom/distr %} directory, you should see an error message:
~~~~~~
Exception: the option '--treefile' is required but missing
~~~~~~
{:.bash-output}

Why did we get this error message? Try running the program like this from the command line:
~~~~~~
./strom --treefile test.tre
~~~~~~
{:.bash}
Because the name of the tree file is no longer hard-coded in the main function, the program forces you to supply these file names.
~~~~~~
./gostrom.sh --treefile test.tre
~~~~~~
{:.bash}

{% elsif OS == "mac" %}
[//]: ################################### MAC ######################################
### Specifying command line arguments
To specify command line arguments when your program is run inside the debugger, follow these steps:

At the top of the Xcode window, to the right of the :arrow_forward: (run program in debugger) and :stop_button: (stop program running in debugger) buttons, click on the button labeled "strom" (attached to the button labeled "My Mac").

Choose "Edit Scheme..." from the popup menu, then choose the "Arguments" tab and click the `+` button under "Arguments Passed On Launch".

Click the `+` button 1 more time so that there are places for a total of 2 arguments
* Type `--treefile` into the first slot
* Type `test.tre` into the second slot

You can now close the dialog box using the _Close_ button at the bottom right and run the program.

{% elsif OS == "win" %}
[//]: ################################### WINDOWS ######################################
{% comment %}
You will n eed to set breakpoints on each of the 2 lines in {% indexfile strom.hpp %} containing the statement `std::exit(1)` in order to stop the output window from disappearing before you can view it (these 2 lines are in the `Strom::processCommandLineOptions` function).
{% endcomment %}
To specify command line arguments when your program is run inside the debugger, follow these steps:

Right-click on the _strom_ project inside the _strom_ solution in the Solution Explorer and choose _Properties_ from the menu that pops up.
Click on the _Debugging_ item in _Configuration Properties_
In the _Command Arguments_ field, type 
~~~~~~
--treefile test.tre
~~~~~~
{:.bash-output}
Close the dialog box using the OK button at the bottom right and you should be good to go!
{% endif %}

## Expected output
Here is the output you should see if everything is working correctly:
~~~~~~
{{ "steps/step-08/test/reference-output/output-a.txt" | polcodesnippet:"","" }}
~~~~~~
{:.bash-output}

If the lines in your output are flooded with lines such as "Creating Node object" and "Destroying Node object", then you failed to heed the advise in the section "Lost at sea" at the very bottom of the [Test the TreeSummary class]({{ "../step-07/04-test-tree-summary" | releative_url }}) page.

You may have wondered about this line:
~~~~~~
Note: configuration file (strom.conf) not found
~~~~~~
{:.bash-output}
The Boost `program_options` library makes it possible to store command line options inside a configuration file rather than entering them from the command line. If you create a file named {% indexfile strom.conf %} inside the install directory ({% indexfile ~/Documents/strom/distr %}) containing these two lines...
~~~~~~
{{ "steps/step-08/test/strom0.conf" | polcodesnippet:"","" }}
~~~~~~
{:.bash-output}

{% if OS == "linux" %}
[//]: ################################### LINUX ######################################
you should be able to run strom *without* specifying any command line options. You may wish to create the {% indexfile strom.conf %} file inside your {% indexfile src %} directory and add a line to your {% indexfile meson.build %} file to copy it to the install directory.
{% elsif OS == "mac" %}
[//]: ################################### MAC ######################################
you should be able to run the program *without* specifying any command line options (try unchecking or deleting both `--treefile` and `test.tre` rows under "Arguments Passed On Launch").
{% elsif OS == "win" %}
[//]: ################################### WINDOWS ######################################
{% endif %}
Note that in the config file the convention is to use key/value pairs, whereas on the command line, `--` is the convention. Be sure to use one key/value pair per line, and note that lines starting with `#` are ignored (this feature can be used to insert comments into your configuration files).

