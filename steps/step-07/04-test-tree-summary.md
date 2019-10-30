---
layout: steps
title: Test the TreeSummary class
partnum: 7
subpart: 4
description: To test the `TreeSummary` class, we need for the `main` function to create a `TreeSummary` object and call its `readTreeFile` and `showSummary` functions. We also need a tree file to read, and need to link in the Nexus Class Library to do the work of parsing this tree file.
---
{% assign OS = site.operatingsystem %}
{{ page.description | markdownify }}

## Create a tree file

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################
Create a file named {% indexfile test.tre %} and fill it with the following text:

{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################
Create a file named {% indexfile test.tre %} (use the "Empty" template under the "Other" category for this file because it is not a header file) and fill it with the following text:

{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################
Create a new filter in your Solution Explorer named _Data Files_ by right-clicking on the _strom_ solution, then choosing _Add > New Filter_. Filters in VSC19 are folder aliases that help you organize files.

Create a file named {% indexfile test.tre %} by right-clicking on your new _Data Files_ filter and choosing _Add > New Item... > Utility > Test file (.txt)_. Fill your new {% indexfile test.tre %} file with the following text:

{% endif %}
[//]: ################################# ENDIF ######################################
~~~~~~
{{ "assets/data/test.tre" | polcodesnippet:"","" }}
~~~~~~
{:.cpp}

## Modify the `main` function

Modify the {% indexfile main.cpp %} file to look like this:
~~~~~~
{{ "steps/step-07/src/main.cpp" | polcodesnippet:"start-end","" }}
~~~~~~
{:.cpp}

Note that a `try...catch` combination is used to catch and report any exceptions raised by the Nexus Class Library when it attempts to read and interpret the tree file. For example, if the file {% indexfile test.tre %} is missing, the program will say
~~~~~~
"test.tre" does not refer to a valid file.
Program aborted due to errors encountered reading tree file.
~~~~~~
{:.bash-output}
Exceptions are small objects that are "thrown" when something unexpected happens within the `try` block. The `catch` block specifies what particular {% indexshow exceptions %} it is prepared to handle (in this case an object of type `NxsException`). The `NxsException` object is stored in the variable `x`, and the `what()` member function of `x` returns the message string stored when the exception was generated. Because exceptions normally represent situations from which recovery is impossible, the catch block calls the `std::exit` function in order to terminate the program after the message is output on the standard error stream.

## Installing the Nexus Class Library
{% if OS == "linux" %}
[//]: ##################################### LINUX #######################################
When you compile strom, you will receive the following error:
~~~~~~
In file included from ../main.cpp:2:0:
../tree_summary.hpp:15:32: fatal error: ncl/nxsmultiformat.h: No such file or directory
compilation terminated.
~~~~~~
{:.bash-ouptut}

The `readTreefile` function in our `TreeSummary` class makes use of the Nexus Class Library's {% indexcode MultiFormatReader %} class, and the compiler is simply telling us that we haven't yet told it where to find the code for NCL's MultiFormatReader. Before we can compile strom again, we must install the Nexus Class Library and tell meson where to find it.

Download the NCL gzipped tar file as follows:
~~~~~~
cd
curl -LO https://sourceforge.net/projects/ncl/files/NCL/ncl-2.1.18/ncl-2.1.18.tar.gz
~~~~~~
{:.bash}

Navigate into the `libraries` directory and unpack {% indexfile ncl-2.1.18.tar.gz %} there:
~~~~~~
cd ~/Documents/libraries
tar zxvf ~/ncl-2.1.18.tar.gz
~~~~~~
{:.bash}

Navigate into the ncl-2.1.18 directory and build the NCL using the traditional configure script followed by `make install`. The `--prefix` option passed to the configure script results in the NCL being installed in your home directory. Leaving off the `--prefix` option would result in NCL being installed for all users in your system, which is arguably preferable as long as you have the necessary privileges to install libraries systemwide, but this tutorial assumes you do not have superuser powers and thus shows you how to do everything locally. The `CXXFLAGS="-std=c++11"` part specifies that the C++11 _dialect_ should be used when compiling NCL. This will ensure that the NCL library can be linked to strom, which is also being compiled under C++11.
~~~~~~
cd ~/Documents/libraries/ncl-2.1.18
./configure --prefix=$HOME CXXFLAGS="-std=c++11"
make install
~~~~~~
{:.bash}

## Create a directory for static library files

The `make install` step above creates a directory {% indexfile lib %} inside your home directory. Navigate to the {% indexfile ~/lib %} directory and create a directory named {% indexfile static %} there, and copy the {% indexfile libncl.a %} file to that directory:
~~~~~~
cd ~/lib
mkdir static
cd static
cp ~/lib/ncl/libncl.a .
~~~~~~
{:.bash}

Why not simply use {% indexfile libncl.a %} where it was created? Compilers in linux operating systems prefer to create dynamic link (a.k.a. shared object) libraries. These are separate files (with the filename extension `.so`) that are linked to your program at run time. In order to force the libraries to be linked at compile time so that our program is not split up over several files, it is necessary to separate the static libraries (e.g. {% indexfile libncl.a %}) from the shared object files (e.g. {% indexfile libncl-2.1.18.so %}) so that the compiler can't even find the shared object version when linking is carried out.

## Update your meson.build file

Modify your {% indexfile meson.build %} file to tell the compiler where the installed NCL header files are located ({% indexfile ~/include %}) and where to find the NCL library ({% indexfile ~/lib/static/libncl.a %}).
~~~~~~
{{ "steps/step-07/src/meson.build" | polcodesnippet:"",""}}
~~~~~~
{:.meson}

The line beginning `incl_ncl` points meson to the location of NCL's header files in the same way `incl_boost` points to the location of the Boost headers. The following two lines point meson to the location of the {% indexfile libncl.a %} static library and store this information in a meson variable called `lib_ncl`:
~~~~~~
{{ "steps/step-07/src/meson.build" | polcodesnippet:"a-b",""}}
~~~~~~
{:.meson}
The `cpp` variable is first set to the meson object that stores information about the C++ compiler. The `find_library` function of the compiler object is then used to locate the file {% indexfile libncl.so %}. The convention for library file naming is to follow the pattern `libXXX.so`, so all that `find_library` needs to know is the `XXX` part (`ncl` here) and a list of directories (`['/home/plewis/lib/ncl']` here) to include in the search. The resulting `lib_ncl` variable is passed to the `dependencies` option of the `executables` command.

The last line,
~~~~~~
{{ "steps/step-07/src/meson.build" | polcodesnippet:"end",""}}
~~~~~~
{:.meson}
copies the tree file {% indexfile test.tre %} to the directory where the program is installed by meson. Specifying `'.'` for `install_dir` means that {% indexfile test.tre %} will be copied directly to the directory specified by the `prefix` variable defined in the `default_options` section of the project command starting on the first line of {% indexfile meson.build %}. If we had specified `install_dir: 'data'`, then {% indexfile test.tre %} would be copied to `/home/plewis/Documents/strom/distr/data` instead.

{% elsif OS == "mac" %}
[//]: ##################################### MAC #######################################
When you compile strom, you will receive the following error:
~~~~~~
Unknown type name 'NxsException'
~~~~~~
{:.bash-ouptut}

The `readTreefile` function in our `TreeSummary` class makes use of the Nexus Class Library's {% indexcode MultiFormatReader %} class, and the compiler is simply telling us that we haven't yet told it where to find the code for NCL's `NxsException` class used in the {% indexfile main.cpp %} file. Before we can compile strom again, we must install the Nexus Class Library and tell Xcode where to find it.

Download the NCL gzipped tar file {% indexfile ncl-2.1.18.tar.gz %} or zip file {% indexfile ncl-2.1.18.zip %} from the [SourceForge web site](https://sourceforge.net/projects/ncl/files/NCL/ncl-2.1.18/).

Navigate into your _~/Documents/libraries_ directory and unpack the file you downloaded there.

Open Terminal or iTerm, navigate into the {% indexfile ncl-2.1.18 %} directory and build the NCL using the traditional configure script followed by `make install`. The `--prefix` option passed to the configure script results in the NCL being installed in your home directory. Leaving off the `--prefix` option would result in NCL being installed for all users in your system, which is arguably preferable as long as you have the necessary privileges to install libraries systemwide, but this tutorial assumes you do not have superuser powers and thus shows you how to do everything locally.
~~~~~~
cd ~/Documents/libraries/ncl-2.1.18
./configure --prefix=$HOME
make install
~~~~~~
{:.bash}

## Create a directory for static library files

The `make install` step above creates a directory {% indexfile lib %} inside your home directory. Navigate to the {% indexfile ~/lib %} directory and create a directory named {% indexfile static %} there, and copy the {% indexfile libncl.a %} file to that directory:
~~~~~~
cd ~/lib
mkdir static
cd static
cp ~/lib/ncl/libncl.a .
~~~~~~
{:.bash}

Why not simply use {% indexfile libncl.a %} where it was created? Compilers in unix-based operating systems prefer to create dynamic link (a.k.a. shared object) libraries. These are separate files (with the filename extension `.so` or `.dylib`) that are linked to your program at run time. In order to force the libraries to be linked at compile time so that our program is not split up over several files, it is necessary to separate the static libraries (e.g. {% indexfile libncl.a %}) from the shared object files (e.g. {% indexfile libncl-2.1.18.so %}) so that the compiler can't even find the shared object version when linking is carried out.

## Inform Xcode of the existence of NCL

You now need to tell Xcode where the installed NCL header files are located ({% indexfile ~/include %}) and where to find the compiled library.

Choose _Xcode > Preferences..._ from the main menu, then choose the "Locations" tab, then click the "Custom Paths" subtab.

Click the `+` button and add a new custom path item named `NCL_ROOT` with display name `NCL_ROOT`. On my computer, I entered the path `/Users/plewis/include/ncl` (but you should of course modify this for your case).

Click the red button at the top left to close the preferences dialog box.

In the Xcode main menu, choose _View_ > _Navigators_ > _Show Project Navigator_, then click on the {% indexshow blue project icon %} :blueproject: labeled _strom_ at the top of the Project Navigator. Click on the _Build Phases_ tab. Click the `+` sign in _Link Binary With Libraries_ and (after clicking the _Add Other..._ button) navigate to the directory {% indexfile ~/lib/static %} and select the _libncl.a_ file. This informs Xcode where to find the compiled library code for purposes of linking the library into your program's executable file. Linking means to establish pointers from function calls in your program to the start of the that function in the library, so that when strom calls, for example, `nexusReader.ReadFilepath` in `TreeSummary::readTreeFile`, the code for the `ReadFilepath` function will be executed.

Click on _Build Settings_, _All_ (not _Basic_ or _Customized_), _Levels_ (not _Combined_), and then type "Header Search Paths" into the _Search_ box to filter out all build settings except the one we are interested in at the moment. In the "Header Search Paths" row, double-click on the column with the {% indexshow blue project icon %} :blueproject: (this is the column for the project settings, the other column labeled strom contains target-specific settings). Click the `+` symbol and add `$(NCL_ROOT)`, pressing the enter key when finished. This tells Xcode where to find NCL-specific header files, such as the {% indexfile  nxsmultiformat.h %} header included near the top of the {% indexfile tree_summary.hpp %} file.

Now search for "Library Search Paths" in _Build Settings_. In the "Library Search Paths" row, double-click on the column with the {% indexshow blue project icon %} :blueproject: (this is the column for the project settings, the other column labeled strom contains target-specific settings). Click the `+` symbol and add `$(HOME)/lib/static`, pressing the enter key when finished. This tells Xcode where to find the {% indexfile libncl.a %} file when it reaches the linking step.

{% elsif OS == "win" %}
[//]: ##################################### WINDOWS #####################################
Download the Nexus Class Library (NCL) zip archive using the green download button at the [NCL SourceForge site](https://sourceforge.net/projects/ncl/). Unzip the {% indexfile ncl-2.1.18.zip %} file into your libraries directory. (Again, I strongly recommend using the program 7-zip to unzip the downloaded zip file. Move the zip file into your libraries directory, right-click it, then choose _7-zip > Extract Here_. Afterwards, delete the zip file - the icon with zipper - to avoid confusion.)

On my machine, the directory path now looks like this:
~~~~~~
C:\Users\Paul Lewis\Documents\libraries\ncl-2.1.18
~~~~~~
{:.bash-output}
In the navigation bar of Windows Explorer, the path looks like this:
~~~~~~
This PC > Documents > libraries > ncl-2.1.18
~~~~~~
{:.bash-output}

There is no batch file or VSC19 project for building NCL in Windows, so we will just add it directly to our strom solution as a project named _ncl_.

* Right-click on the _strom_ solution (not the _strom_ project) and choose _Add > New Project..._. Select _Library_ from the _Project type_ dropdown. Now select _Static Library_ from the main panel. Click the _Next_ button and name the new project _ncl_. Your _strom_ solution now contains 2 projects: _ncl_ and _strom_.

* Right-click on your new _ncl_ project and choose _Add > Existing Item..._, navigate to _This PC > Documents > libraries > ncl-2.1.18 > ncl_ and select all files except {% indexfile Makefile.am %} and {% indexfile Makefile.in %}. Drag the `.cpp` files into the project's _Source Files_ filter and drag the `.h `files into the _Header Files_ filter. You should now have 41 files total, with 18 inside _Source Files_ and 23 inside _Header Files_.

* Bring up the _Properties_ panel for the project by right-clicking on the _ncl_ project and choosing _Properties_ from the popup menu. Click _C/C++_, then _Preprocessor_, and add `_CRT_SECURE_NO_WARNINGS` and `_SCL_SECURE_NO_WARNINGS` to the _Preprocessor Definitions_. This will prevent errors related to NCL’s use of functions of which VSC19 does not approve. Because the NCL is not our code, we are effectively just telling VSC19 to look the other way. Do not close the _Properties_ panel just yet.

* While you are in the Properties panel for the _ncl_ project (if you've already closed it, please open it again), go to _VC++ Directories > Include Directories_, click the button at the end of the row, then click _&lt;Edit...&gt;_, create a new line and type in (or select using the `...` button) the path to your NCL library folder. For example, I entered this:
~~~~~~
C:\Users\Paul Lewis\Documents\libraries\ncl-2.1.18
~~~~~~
{:.bash-output}

* Still in the Properties panel for the _ncl_ project, go to _C/C++ > Precompiled Headers > Precompiled Header_ and choose _Not Using Precompiled Headers_.

* We need to make changes to a couple of files in the NCL to avoid compiler errors. First, you will need to make a couple of changes to the file {% indexfile nxsstring.cpp %}, which is part of the source code for the NCL library. Find the section (starting at line 250) containing the Windows-specific preprocessor definitions below and change `define std::sprintf std::sprintf_s `to `define sprintf sprintf_s` (i.e. remove `std::` from two places in that line):
~~~~~~
#if defined(_MSC_VER)
#	pragma warning(disable:4786)
#	pragma warning(disable:4291)
#	if _MSC_VER >= 1500
#		include 
#		if !defined(vsnprintf)
#			define vsnprintf _vsnprintf_s
#		endif
#		define std::sprintf std::sprintf_s
#   else
#       define vsnprintf _vsnprintf
#   endif
#endif
~~~~~~
{:.bash-output}

* Go to line 491 (or search for `std::sprintf `and remove the `std::` from this line too, leaving only this on line 491:
~~~~~~
sprintf(fmtstr, "%%.%df", p);
~~~~~~
{:.bash-output}

* Edit the file {% indexfile nxstaxablock.cpp %}, line 279, adding `(unsigned)` right before `taxLabels.size()`. This line should now look like this:
~~~~~~
e << "Number of stored labels (" << (unsigned)taxLabels.size() << ") exceeds the NTax specified (" << dimNTax<<").";
~~~~~~
{:.bash-output}

* Finally, add a reference to your _ncl_ project in your _strom_ project. To do this, right-click on your _strom_ project, choose _Add > Reference..._ from the popup menu, then check the _ncl_ project, which should be the only thing available to check. Choosing _Build > Build Solution_ from the main menu will build both the _ncl_ and _strom_ projects, and _strom_ should link in the static library created by the _ncl_ project.

{% endif %}
[//]: ##################################### ENDIF #######################################

{% comment %}
{% if OS == "linux" %}
[//]: ##################################### LINUX #######################################
### One more hurdle

Your program should now compile, but when you try running it, you will encounter one more hurdle:
~~~~~~
/home/paul/Documents/strom/distr/strom: error while loading shared libraries: libncl-2.1.18.so: cannot open shared object file: No such file or directory
~~~~~~
{:.bash-output}
Our modification to the {% indexfile meson.build %} file allowed the compiler to link function calls (such as `nexusReader.ReadFilepath` in {% indexfile tree_summary.hpp %}) to code for that function in the shared library {% indexfile libncl.so %}; however, when the program is running it needs to know where {% indexfile libncl.so %} is so that it can load that functionality at run time. It is as if someone showed you how to use a hammer, but now that you are faced with a task that requires a hammer, you discover that you don't know where the hammer is! Ordinarily, programs look in standard system locations (e.g. {% indexfile /usr/local/lib %}) for shared libraries, but we installed {% indexfile libncl.so %} in a non-standard location (namely {% indexfile ~/lib/ncl %}), so we must give it this non-standard location somehow. We will do this by creating a shell script named {% indexfile gostrom.sh %} that will provide the location of the shared library in an environmental variable ({% indexcode LD_LIBRARY_PATH %}) and then run the program. I will show you how to create this small file by typing the text directly into the cat command, but you can also create it using your favorite text editor. Note that there must be space around and between the `-` and `>` after {% indexcode cat %}, and you must use Ctrl-d (d key pressed while the control key is down) to tell cat that you are finished typing:
~~~~~~
cd ~/Documents/strom/distr
cat - > gostrom.sh
#!/bin/bash

export LD_LIBRARY_PATH="$HOME/lib/ncl"
./strom
~~~~~~
{:.bash-output}
Once you have pressed Ctrl-d to finish, the contents of {% indexfile gostrom.sh %} should look like this:
~~~~~~
#!/bin/bash

export LD_LIBRARY_PATH="$HOME/lib/ncl"
./strom
~~~~~~
{:.bash-output}

Make {% indexfile gostrom.sh %} executable as follows:
~~~~~~
chmod +x gostrom.sh
~~~~~~
{:.bash}

You can now run strom using {% indexfile gostrom.sh %} as follows:
~~~~~~
./gostrom.sh
~~~~~~
{:.bash}
{% elsif OS == "mac" %}
[//]: ##################################### MAC #######################################
### One more hurdle

Your program should now compile, but when you try running it, you will encounter one more hurdle. The program will report
~~~~~~
Program aborting due to errors encountered reading tree file.
"test.tre" does not refer to a valid file.
~~~~~~
{:.bash-output}
The working directory is buried in a secret location deep inside the Developer folder on your computer, but you've created {% indexfile test.tre %} inside the same folder containing all the other source code files you've created. Actually, the location is not secret, but it is buried deep. To find it, open _Xcode > Preferences..._ from the main Xcode menu, then choose the "Locations" tab and note the folder listed in gray text in the "Derived Data" section. If you go to this folder, you will find a folder beginning with "strom-" but ending in a string of random letters. Enter this folder and then click down into Build, then Products, and finally Debug, and there you will find a file named strom. This is your executable. Copy `test.tre` to this same folder and run strom as usual from the IDE and you will find that it now works!

This is a lot of trouble to go to, however, so let's set up Xcode to use the folder containing our source code as the working directory. To do this, bind the button labeled "strom" on the toolbar at the top of the Xcode window. (This button is joined to another button labeled "My Mac".) Click this button and choose "Edit Scheme..." from the popup menu. Now click the "Options" tab and check the box labeled "Use custom working directory". This will enable the edit field below. Specify in this edit field the folder containing {% indexfile test.tre %}. You can use your mouse to choose this folder if you click the tiny folder icon on the right side of the edit field. The file {% indexfile test.tre %} should now be found by strom if you run it from the IDE using the right-pointing triangle button in the XCode top toolbar.

{% elsif OS == "win" %}
[//]: ##################################### WINDOWS ######################################
{% endif %}
[//]: ##################################### ENDIF ########################################
{% endcomment %}

## Expected output

The expected output is shown below. Keep in mind that tree topologies are ordered arbitrarily; the important thing to note is whether 7 of the trees in the file have one topology, 4 have a distinctly different topology, and the final 2 trees have a third topology distinct from the first two.
~~~~~~
{{ "steps/step-07/test/reference-output/output.txt" | polcodesnippet:"","" }}
~~~~~~
{:.bash-output}

## Lost at sea
Important! The lines of output above will be lost in a sea of constructor and destructor notifications unless you comment those out. Visit the `Node`, `Tree`, `Split`, `TreeManip`, and `TreeSummary` classes and use `//` to comment out the `std::cout << "Constructing a XXXX" << std::endl;` statement inside the constructor and the `std::cout << "Destroying a XXXX" << std::endl;` statement inside the destructor. In future steps, these "reporter" output lines will be created already commented out so that they can be easily turned on if needed during debugging but not clutter the output during normal operation.

