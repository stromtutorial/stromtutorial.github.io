---
layout: steps
title: Test Likelihood
partnum: 10
subpart: 2
description: Test the Likelihood class by reading and storing a tree as well as data from nexus-formatted files.
---
{% assign OS = site.operatingsystem %}
## Create a tree file
To test the `Likelihood` class, we need both a data set and a tree. You have already created a data file named {% indexfile rbcL.nex %}. Here is a tree file containing the maximum likelihood tree for the data in {% indexfile rbcL.nex %}. The log-likelihood of this tree under the JC69 model is `-278.83767`. 

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################
Create a file containing the text below and name it {% indexfile rbcLjc.tre %}.
{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################
Create a file containing the text below and name it {% indexfile rbcLjc.tre %}. This file does not need to be in your project, so you can use any text editor to create it (e.g. [BBEdit](https://www.barebones.com/products/bbedit/)).
{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################
Create a file containing the text below inside your `Data Files` filter and name it {% indexfile rbcLjc.tre %}. 
{% endif %}
[//]: ################################# ENDIF ######################################
Here are the contents of the file:
~~~~~~
{{ "assets/data/rbcLjc.tre" | polcodesnippet:"","" }}
~~~~~~
{:.bash-output}

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################
In order to get {% indexfile rbcLjc.tre %} installed to the same directory as the strom executable file, you need to add this line to the end of your {% indexfile meson.build %} file:
~~~~~~
{{ "steps/step-10/src/meson.build" | polcodesnippet:"start-!e","e" }}
~~~~~~
{:.meson}

{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################

{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################
Be sure that the {% indexfile rbcLjc.tre %} file was saved in your project directory (i.e. the same directory in which you saved {% indexfile rbcL.nex %}).

{% endif %}
[//]: ################################# ENDIF ######################################

## Specifying the data and tree files

You will need to specify the data file and tree file either on the command line (using `--datafile rbcL.nex --treefile rbcLjc.tre`) or create a text file named {% indexfile strom.conf %} containing these two lines:
~~~~~~
{{ "steps/step-10/test/strom-a.conf" | polcodesnippet:"","" }}
~~~~~~
{:.bash-output}

## Modifying the Strom class

Add the highlighted lines to the `Strom` class declaration in {% indexfile strom.hpp %}.
~~~~~~
{{ "steps/step-10/src/strom.hpp" | polcodesnippet:"start-end_class_declaration","a,c,d-e" }}
~~~~~~
{:.cpp}

Initialize `gpu` and `ambigmissing` in the `clear` function:
~~~~~~
{{ "steps/step-10/src/strom.hpp" | polcodesnippet:"begin_clear-end_clear","f-g" }}
~~~~~~
{:.cpp}

Add the `required()` call in `Strom::processCommandLineOptions` to the `treefile` command line option and add options for `gpu` and `ambigmissing`:
~~~~~~
{{ "steps/step-10/src/strom.hpp" | polcodesnippet:"begin_processCommandLineOptions-end_processCommandLineOptions","treefilerequired,gpuoption,ambigmissingoption" }}
~~~~~~
{:.cpp}

You could remove the `required()` call from all command line options; I only use it here because this means `program_options` takes care of informing the user if they forget to specify a data or tree file on the command line or in the {% indexfile strom.conf %} file. Because the program, as currently configured, will only work if given both a data file and a tree file, it makes sense to require these options.

Replace the body of the `Strom::run` function in the file {% indexfile strom.hpp %} with this revised version.
~~~~~~
{{ "steps/step-10/src/strom.hpp" | polcodesnippet:"begin_run-end_run","" }}
~~~~~~
{:.cpp}

## Make Likelihood a friend of Tree and Node 

The `defineOperations` member function body above accesses private data members of both the `Tree` and `Node` classes. To avoid compile-time errors, you will thus need to declare the `Likelihood` class to be a friend of both `Tree` and `Node`. In the {% indexfile node.hpp %} file, you should uncomment the 2 lines highlighted below:
~~~~~~
{{ "steps/step-10/src/node.hpp" | polcodesnippet:"start-end_friends","a,b" }}
~~~~~~
{:.cpp}
Uncomment the same 2 lines in the {% indexfile tree.hpp %} file:
~~~~~~
{{ "steps/step-10/src/tree.hpp" | polcodesnippet:"start-end_friends","a,b" }}
~~~~~~
{:.cpp}

## Installing BeagleLib
Our Likelihood class depends on BeagleLib to do all the heavy lifting with respect to calculating transition probabilities and the log-likelihood itself. Our next task is thus to install BeagleLib and tell our program where to find the shared library for linking and at run time.

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################
Download the file {% indexfile beagle-lib-master.zip %} from the [BeagleLib GitHub site](https://github.com/beagle-dev/beagle-lib) as follows:
~~~~~~
cd 
curl -LO https://github.com/beagle-dev/beagle-lib/archive/v3.1.2.tar.gz
~~~~~~
{:.bash}

Navigate into the `libraries` directory and unpack {% indexfile v3.1.2.tar.gz %} there:
~~~~~~
cd ~/Documents/libraries
tar zxvf ~/v3.1.2.tar.gz
~~~~~~
{:.bash}

Now build the library, specifying the following on the command line:
* `--prefix=$HOME` to make the compiled library end up in {% indexfile $HOME/lib %}
* `--with-jdk=no` because we will not need the Java version of the library (without this, you will run into problems if Java is not installed)
* `CXXFLAGS="-std=c++11"` to compile according to the C++11 standard (without this, you may have trouble linking the library to your executable, which will use C++11 features)
{% comment %}
* `--enable-static` to create static library files that can be linked directy into your executable.
{% endcomment %}
~~~~~~
cd beagle-lib-3.1.2
./autogen.sh
./configure --prefix=$HOME --with-jdk=no CXXFLAGS="-std=c++11"
make
make install
~~~~~~
{:.bash}
If the `./autogen.sh` command generates an error (`autoreconf: not found`), you will need to install Gnu autotools and autoreconf:
~~~~~~
sudo apt install autotools-dev
sudo apt install dh-autoreconf
~~~~~~
{:.bash}

You can ignore (for the moment at least) the "{% indexshow OpenCL not found %}" and "{% indexshow NVIDIA CUDA nvcc compiler not found %}" warnings issued by configure. We will only be using the CPU version of the library for this tutorial. If you later want to take advantage of GPU resources, you will need to supply either OpenCL or NVIDIA and recompile BeagleLib.

{% comment %}
## Copy the BeagleLib libraries to the static library

Copy the BeagleLib static libraries to ~/lib/static:
~~~~~~
cd ~/lib
cp libhmsbeagle.a ../static
cp libhmsbeagle-cpu.a ../static
cp libhmsbeagle-cpu-sse.a ../static
~~~~~~
{:.bash}
{% endcomment %}

## Tell meson about BeagleLib

Now that you've built BeagleLib, you tell meson about it in your meson.build file as follows (affected lines highlighted):
~~~~~~
{{ "steps/step-10/src/meson.build" | polcodesnippet:"","x,a,b,c,d" }}
~~~~~~
{:.meson}

## BeagleLib requires dynamic linking

The first highlighted line in the listing above for meson.build differs from before in that we've removed `,'cpp_link_args=-static'`. BeagleLib uses a plugin system to swap in the version (CPU vs GPU) of the library that you ask for when you initialize BeagleLib. This plugin system requires dynamic linking: the relevant portion of the library is linked to your executable when needed at runtime rather than statically linking it at compile time. Thus, our application can no longer be entirely statically linked, as BeagleLib is an exception. Even without the `-static` link argument, the linker will still statically link the other libraries to your executable (program_options, filesystem, and ncl); the only library not statically linked will be BeagleLib.

## Telling your program where to find the BeagleLib dynamic link library

You specified `--prefix=$HOME` when you ran configure prior to compiling BeagleLib, so when you run make install, the resulting products will be stored in the `lib` subdirectory of your home directory (i.e. `~/lib`). If you were to run strom now, it would fail with a message like this:
~~~~~~
./strom: error while loading shared libraries: libhmsbeagle.so.1: cannot open shared object file: No such file or directory
~~~~~~
{:.bash-output}
The bottom line is that you must either install the BeagleLib "so" (shared object) files in a standard location or use the environmental variable LD_LIBRARY_PATH to specify the location. We will use the latter approach below, but, if you have superuser privileges on your system, you may wish to leave off the `--prefix=$HOME` from your configure call so that the libraries are installed in the standard location (`/usr/lib`). In this case, you will need to call `make install` as superuser (or use `sudo make install`) because installing things in `/usr/lib` can only be done as the superuser.

### Creating a go script

The easiest way to ensure that LD_LIBRARY_PATH is set before calling strom is to create a bash script that sets it before calling strom. In your strom source directory (where {% indexfile main.cpp %} resides), create a file named {% indexfile go.sh %} with the following contents:
~~~~~~
#!/bin/bash

LD_LIBRARY_PATH="$HOME/lib:$LD_LIBRARY_PATH" ./strom $@
~~~~~~
{:.bash}

The first line of this script tells the operating system to run it using the bash interpreter. The only other line sets the LD_LIBRARY_PATH environmental variable to the location where you installed the BeagleLib dynamic link library files (and also merges in any LD_LIBRARY_PATH that already exists) and then calls strom. The `$@` causes bash to pass along any command line arguments supplied to the {% indexfile go.sh %} script on to the strom executable. This allows you to override settings in the {% indexfile strom.conf %} file, if needed.

In order for your {% indexfile go.sh %} script to be treated as an executable file and not just a plain text file, use the `chmod` command as follows (navigating first to the {% indexfile src %} directory):
~~~~~~
chmod +x go.sh
~~~~~~
{:.bash}

## IMPORTANT

From this point on, when I tell you to run strom, what I really mean is to run {% indexfile go.sh %}!

The last line highlighted in the {% indexfile meson.build %} file shown above is the line that copies your {% indexfile go.sh %} script to the distribution directory. You should go ahead and build strom with this new meson.build file so that all the elements needed are in place in your distribution directory.

{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################

### Download BeagleLib

Start by downloading the file {% indexfile beagle-lib-master.zip %} from the [BeagleLib GitHub site](https://github.com/beagle-dev/beagle-lib). Unzip the library and move the resulting directory, `beagle-lib-master`, to your `libraries` directory, which I will assume is {% indexfile ~/Documents/libraries %}.

Open a terminal window and navigate into your `beagle-lib-master` directory:
~~~~~~
cd ~/Documents/libraries/beagle-lib-master
~~~~~~
{:.bash}

### Building BeagleLib

Now build the library, specifying the following on the command line:
* `--prefix=$HOME` to make the compiled library end up in {% indexfile $HOME/lib %}
* `--with-jdk=no` because we will not need the Java version of the library (without this, you will run into problems if Java is not installed)
* `CXXFLAGS="-std=c++11"` to compile according to the C++11 standard (without this, you may have trouble linking the library to your executable, which will use C++11 features)
* `--enable-static` to create static library files that can be linked directy into your executable.

~~~~~~
./autogen.sh
./configure --prefix=$HOME --with-jdk=no CXXFLAGS="-std=c++11" --enable-static
make
make install
~~~~~~
{:.bash}

If the `./autogen.sh` command generates an error (`autoreconf: not found`), you will need to install Gnu autotools and autoreconf. The easiest way to do this is to first install [Homebrew](https://brew.sh):
~~~~~~
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
~~~~~~
{:.bash}
and then use homebrew to install autotools:
~~~~~~
brew install autoconf
brew install automake
~~~~~~
{:.bash}

You can ignore (for the moment at least) the "{% indexshow OpenCL not found %}" and "{% indexshow NVIDIA CUDA nvcc compiler not found %}" warnings issued by configure. We will only be using the CPU version of the library for this tutorial. If you later want to take advantage of GPU resources, you will need to supply either OpenCL or NVIDIA and recompile BeagleLib.

Move all files in {% indexfile ~/lib %} that have names beginning with "libhmsbeagle" and having an {% indexfile *.a %} file name extension into your {% indexfile ~/lib/static %} folder.

### Tell Xcode about BeagleLib

Once you have compiled BeagleLib, you need to tell Xcode about it so that it can be linked into your executable. 

In the Xcode main menu, choose _View_ > _Navigators_ > _Show Project Navigator_, then click on the {% indexshow blue project icon %} :blueproject: labeled _strom_ at the top of the Project Navigator. Click on the _Build Phases_ tab. Click the `+` sign in _Link Binary With Libraries_ and (after clicking the _Add Other..._ button) navigate to the directory {% indexfile ~/lib/static %} and select the _libhmsbeagle.a_, _libhmsbeagle-cpu.a_, and _libhmsbeagle-cpu-sse.a_ files. This informs Xcode where to find the compiled library code for purposes of linking the library into your program's executable file.

{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################

Start by downloading the file {% indexfile beagle-lib-master.zip %} from the [BeagleLib GitHub site](https://github.com/beagle-dev/beagle-lib). Unzip the library (using 7-zip) and move the resulting directory, `beagle-lib-master`, to your libraries directory (and delete the original zip file). The directory you just created will be referred to hereafter as {% indexfile BEAGLELIB_ROOT %}. On my computer, {% indexfile BEAGLELIB_ROOT %} corresponds to this directory:
~~~~~~
C:\Users\Paul Lewis\Documents\libraries\beagle-lib-master
~~~~~~ 
{:.bash-output}

Navigate to {% indexfile BEAGLELIB_ROOT %} and open the {% indexfile BEAGLELIB_ROOT\project\beagle-vs-2017\libhmsbeagle.sln %} Visual Studio solution. Visual Studio Community (VSC19) will issues some warnings and will say "One or more projects in the solution were not loaded correctly…", but then will give you the opportunity to upgrade, which you should do.

Click on Solution Explorer tab on the left side of VSC19, then click once on the _libhsmbeagle_ project to select it.

Choose _Build libhmsbeagle_ under the _Build_ menu.

The last line of the output window should say "Build: 2 succeeded, 0 failed, 0 up-to-date, 0 skipped". If instead it says "Build: 1 succeeded, 1 failed, 0 up-to-date, 0 skipped", it probably means that you do not have Java installed. In this case, you will need to prevent VSC19 from attempting to build the JNI part of the libhmsbeagle project. Expand (using the right-pointing triangle symbol) the _libhmsbeagle_ project within the _libhmsbeagle_ solution in the Solution Explorer. Within the _libhmsbeagle_ project you should see a _libhmsbeagle_ folder containing 2 subfolders named JNI and plugin, along with files named {% indexfile beagle.cpp %}, {% indexfile beagle.h %}, {% indexfile BeagleImpl.h %} and {% indexfile platform.h %}. Expand the JNI folder and for each of the 2 files inside ({% indexfile beagle_BeagleJNIWrapper.cpp %} and {% indexfile beagle_BeagleJNIWrapper.h %}), right-click the file, choose _Properties_, then choose the value _yes_ for _Excluded From Build_. When you are done, both of these files will have a tiny red minus sign symbol beside their name in the Solution Explorer.

Once compilation is completed, you should find a {% indexfile BEAGLELIB_ROOT\project\beagle-vs-2017\x64\Debug %} folder containing several files, including {% indexfile libhmsbeagle.lib %} {% indexfile hmsbeagle64D.dll %}, and {% indexfile hmsbeagle-cpu64D-31.dll %}. The two dynamic link library (dll) files contain code that is loaded when needed by the operating system (that is, most of the functionality in BeagleLib will not be in our program strom, but strom has access to it when it needs it). We will tell our strom project about the library (lib) file because it allows strom to know what functions are available to it in the DLLs.

Before leaving the libhmsbeagle Solution, switch to _Release_ using the Solution Configuration dropdown at the top and build again to create a {% indexfile Release %} folder and release versions of the library.

Copy the debug versions of the dynamic link libraries (DLLs) to the location of your strom debug executable (and the release versions of the DLLs to the location of your strom release executable) so that they will be found at run time. The two debug-version DLLs that you need to copy are named {% indexfile hmsbeagle64D.dll %} and {% indexfile hmsbeagle-cpu64D-31.dll %} and are located in {% indexfile BEAGLELIB_ROOT/project/beagle-vs-2017/x64/Debug %}. Likewise, the two release-version DLLs that you need to copy are named {% indexfile hmsbeagle64.dll %} and {% indexfile hmsbeagle-cpu64-31.dll %} and are located in {% indexfile BEAGLELIB_ROOT\project\beagle-vs-2017\x64\Release %}.The places they should be copied to are the {% indexfile x64\Debug %} and {% indexfile x64\Release %} directories, respectively, inside your strom solution directory. For example, on my computer, this is what my strom Debug and Release directories look like after copying these files (note that I am only showing the directory paths and the two files added to each directory; there are many other files in each directory, including the strom executable):
~~~~~~
C:\Users\Paul Lewis\source\repos\strom\x64\Debug
    hmsbeagle64D.dll
    hmsbeagle-cpu64D-31.dll
C:\Users\Paul Lewis\source\repos\strom\x64\Release
    hmsbeagle64.dll
    hmsbeagle-cpu64-31.dll
~~~~~~
{:.bash-output}

You can now close the {% indexfile BEAGLELIB_ROOT\project\beagle-vs-2017\libhmsbeagle.sln %} solution and open the strom solution again.

In the Solution Explorer within the _strom_ solution, right-click on the _strom_ project and choose _Properties_. In the _VC++ Directories_ section, add _BEAGLELIB_ROOT_ to _Include Directories_ (remember that we are using _BEAGLELIB_ROOT_ to stand for the full path to the directory containing the BeagleLib code, so you should not type the word _BEAGLELIB_ROOT_ directly; instead, use the _..._ button to locate the BeagleLib directory).

While still in the _VC++ Directories_ section, add {% indexfile BEAGLELIB_ROOT\project\beagle-vs-2017\Debug %} to _Library Directories_.

In the _Linker > Input_ section, add {% indexfile libhmsbeagle.lib %} to _Additional Dependencies_.

{% endif %}
[//]: ################################# ENDIF ######################################

## Run strom

Go ahead and run strom now. I'll discuss the output a bit at a time in the sections below. 

{% comment %}
Because we've added another library that needs to be found at run-time, you'll need to modify your {% indexfile gostrom.sh %} script in the install directory ({% indexfile ~/Documents/strom/distr %}) again:
~~~~~~
#!/bin/bash

export LD_LIBRARY_PATH="$HOME/Documents/libraries/boost_1_71_0/stage/lib:$HOME/lib:$HOME/lib/ncl"
./strom $@
~~~~~~
{:.bash-output}

Go ahead and run `strom` using your {% indexfile gostrom.sh %} script. 
{% endcomment %}

### Reading and storing the data
The `run` function first creates a `Data` object and stores a shared pointer to it in the private data member `_data`. After informing the `Data` object about any partition that the user set up, the `run` function reads the file whose name is stored in `_data_file_name` ({% indexfile rbcL.nex %}) and spits out a summary of the data therein. We have not set up data partitioning so, this summary just reports a single data subset containing 60 sites compressed into 21 distinct site patterns: 
~~~~~~
*** Reading and storing the data in the file rbcL.nex
storing implied block: TAXA
storing read block: DATA

Number of taxa: 14
Number of partition subsets: 1
  Subset 1 (default)
    data type: nucleotide
    sites:     60
    patterns:  21
~~~~~~
{:.bash-output}

### Creating a likelihood calculator
The `run` function next creates a `Likelihood` object and stores a shared pointer to that object in the private data member `_likelihood`. It then calls the `Likelihood::beagleLibVersion` function to show which version of BeagleLib we're using and the `Likelihood::availableResources` function to show a summary of resources available to BeagleLib, which on my laptop is just the default CPU plugin of BeagleLib. Next, the `Likelihood::setData` function is called to make the compressed data from the Data object available. Finally, the `Likelihood::initBeagleLib` function is called to construct the BeagleLib machinery needed to do the likelihood calculations:
~~~~~~
*** Resources available to BeagleLib 3.1.2:
0: CPU

*** Creating the likelihood calculator
Created BeagleLib instance 0 (4 states, 1 rate, 1 subset)
~~~~~~
{:.bash-output}

### Reading and storing a tree
Next, a `TreeSummary` object is created and stored in the shared pointer private data member `_tree_summary` and its `readTreefile` function is called to read the tree file whose name is `_tree_file_name` ({% indexfile rbcLjc.tre %}), which you just created. A `Tree` shared pointer named `tree` is created and pointed to the first (and only) tree read from {% indexfile rbcLjc.tre %}. This results in the following output:
~~~~~~
*** Reading and storing the first tree in the file rbcLjc.tre

Warning:  
 A TAXA block should be read before the TREES block (but no TAXA block was found).  Taxa will be inferred from their usage in the TREES block.
at line 5, column (approximately) 1 (file position 33)
storing implied block: TAXA
storing read block: TREES
~~~~~~
{:.bash-output}

### TAXA block warning
If the {% indexshow TAXA block warning %} produced by NCL bothers you, you have a couple of choices:

1. You can provide a TAXA block in your tree file {% indexfile rbcLjc.tre %}
2. You can suppress warnings when you read the tree file.

The first option has the advantage that it preserves the ability of NCL to warn you of possible problems, and is arguably the better solution. To take this route, add the following TAXA block to the {% indexfile rbcLjc.tre %} file between the opening `#nexus` line and the `begin trees;` statement:
~~~~~~
begin taxa;
  dimensions ntax=14;
  taxlabels
		Atractomorpha_echinata
		Bracteacoccus_aerius
		Bracteacoccus_minor
		Chlorotetraedron_incus
		Chromochloris_zofingiensis
		Kirchneriella_aperta
		Mychonastes_homosphaera
		Neochloris_aquatica
		Ourococcus_multisporus
		Pediastrum_duplex
		Pseudomuriella_schumacherensis
		Rotundella_rotunda
		Scenedesmus_obliquus
		Stigeoclonium_helveticum;
end;
~~~~~~
{:.bash-output}

Implementing the second option involves changing this (highlighted) line in your `TreeSummary::readTreefile` function (file {% indexfile tree_summary.hpp %}):
~~~~~~
{{ "steps/step-10/src/tree_summary.hpp" | polcodesnippet:"begin_readTreeFile-after_nexusReader","nexusReader" }}
~~~~~~
{:.cpp}
Instead of specifying {% indexcode NxsReader::WARNINGS_TO_STDERR %} here, you can instead specify {% indexcode NxsReader::IGNORE_WARNINGS %}. The alternatives are specified in the `WarningHandlingMode` enum in {% indexfile nxsreader.h %} (a source file in the Nexus Class Library). The only two other possibilities are `NxsReader::WARNINGS_TO_STDOUT` and `NxsReader::WARNINGS_ARE_ERRORS` (not advisable because it will cause your program to abort at even the slightest warning).

### Calculating the likelihood of the tree
Finally, the `calcLogLikelihood` function of the `Likelihood` object is called to obtain the log-likelihood. This is output along with a hard-coded statement of what the log-likelihood is expected to be so that it is easy to confirm that the program is working:
~~~~~~
*** Calculating the likelihood of the tree
log likelihood = -278.83767
      (expecting -278.83767)
~~~~~~
{:.bash-output}
It is, admittedly, a little silly to let the user enter any data and tree file name on the command line (or {% indexfile strom.conf %} file) and then print out a pre-ordained expectation of the result; however, our only purpose here is to test whether the likelihood machinery we've set up is working: our program is not ready to be distributed for general use yet!

### Catching exceptions
Most of the body of the `run` function is wrapped in a `try` block. If anything goes wrong and an `XStrom` exception is thrown, the program will jump immediately to the corresponding `catch` block and the message stored by the exception object will be displayed.

## Test likelihood calculation on partitioned data
If we partition the data but use the same model for all subsets, we should get the same total log-likelihood. Add 3 lines to your {% indexfile strom.conf %} file to partition the data into 3 equal-sized subsets each containing sites corresponding to a different codon position:
~~~~~~
{{ "steps/step-10/test/strom-b.conf" | polcodesnippet:"","" }}
~~~~~~
{:.bash-output}
Now run your program again. You should see the following output, which lists details for the three subsets (note that there are more total site patterns now because site patterns cannot be combined if the same pattern is seen in different subsets). The log-likelihood is the same as for the unpartitioned case. We will need to add the ability to specify a substitution model (the next step in the tutorial) in order to apply different models to different subsets.
~~~~~~
Partition subset first comprises sites 1-60\3 and has type nucleotide
Partition subset second comprises sites 2-60\3 and has type nucleotide
Partition subset third comprises sites 3-60\3 and has type nucleotide
Starting...
Current working directory: "/home/paul/Documents/strom/distr"

*** Reading and storing the data in the file rbcL.nex
storing implied block: TAXA
storing read block: DATA

Number of taxa: 14
Number of partition subsets: 3
  Subset 1 (first)
    data type: nucleotide
    sites:     20
    patterns:  7
  Subset 2 (second)
    data type: nucleotide
    sites:     20
    patterns:  5
  Subset 3 (third)
    data type: nucleotide
    sites:     20
    patterns:  17

*** Resources available to BeagleLib 3.1.2:
0: CPU

*** Creating the likelihood calculator
Created BeagleLib instance 0 (4 states, 1 rate, 3 subsets)

*** Reading and storing the first tree in the file rbcLjc.tre

Warning:
 A TAXA block should be read before the TREES block (but no TAXA block was found).  Taxa will be inferred from their usage in the TREES block.
at line 5, column (approximately) 1 (file position 33)
storing implied block: TAXA
storing read block: TREES

*** Calculating the likelihood of the tree
log likelihood = -278.83767
      (expecting -278.83767)

Finished!
~~~~~~
{:.bash-output}

