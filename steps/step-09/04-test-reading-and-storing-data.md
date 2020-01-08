---
layout: steps
title: Test Reading and Storing Data
partnum: 9
subpart: 4
description: Test the Data class by reading and storing data from a nexus-formatted data file.
---
{% assign OS = site.operatingsystem %}

## Add the subset command line option
To test the new `Partition` and `Data` classes, we'll need to modify {% indexfile strom.hpp %} to input partition subset specifications from the user. Begin by including some additional header files in {% indexfile strom.hpp %} and a shared pointer to a `Partition` object in the `Strom` class declaration:
~~~~~~
{{ "steps/step-09/src/strom.hpp" | polcodesnippet:"start-end_class_declaration,end","a,b,c,d,l" }}
~~~~~~
{:.cpp}

Next, modify the `Strom::processCommandLineOptions` function as follows:
~~~~~~
{{ "steps/step-09/src/strom.hpp" | polcodesnippet:"begin_processCommandLineOptions-end_processCommandLineOptions","e,f-h,i-j" }}
~~~~~~
{:.cpp}
Note that I've moved the `required()` call in `Strom::processCommandLineOptions` from the treefile command line option to the datafile command line option:
~~~~~~
{{ "steps/step-09/src/strom.hpp" | polcodesnippet:"!f-!g","" }}
~~~~~~
{:.cpp}
You could remove the `required()` call from all command line options; I only use it here because this means `program_options` takes care of informing the user if they forget to specify a data file on the command line or in the {% indexfile strom.conf %} file and, in this particular version, we are testing only whether our program can successfully read in data from a file; we are not testing anything about trees, so the user does not need to specify a tree file to test this version of the program.

The section at the bottom (highlighted in blue) that processes a subset specification provided by the user is worth a little explanation. We start by making sure the data member `_partition`, which is a shared pointer to a `Partition`, actually points to something. The `shared_ptr` `reset` function creates a new `Partition` object and sets `_partition` to point to it. You'll notice that I've highlighted a line at the top of the function that declares a variable named `partition_subsets` that is a vector of strings. This _vector_ is provided to the options description for the subset option. If a single string variable were provided, only the last subset specified would be saved. However, because we specified a containiner, Boost Program Options will save all subset options specified by the user! We can thus iterate through this string vector, passing each of these strings to the `Partition` `parsSubsetDefinition` member function for processing.

Now add the following (highlighted) line to the `Strom::clear` function:
~~~~~~
{{ "steps/step-09/src/strom.hpp" | polcodesnippet:"begin_clear-end_clear","k" }}
~~~~~~
{:.cpp}
This line ensures that a newly-constructed `Strom` object has a default partition in place.

## Create a data file

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################
Before running it, however, you will need to create the file {% indexfile rbcL.nex %}. This file does not need to be in your project, so you can use any text editor to create it (e.g. TextEditor, nano, vi, etc.).

{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################
Before running it, however, you will need to create the file {% indexfile rbcL.nex %}. This file does not need to be in your project, so you can use any text editor to create it (e.g. [BBEdit](https://www.barebones.com/products/bbedit/)).

{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################
Before running it, however, you will need to create a data file. Right-click on your _Data Files_ filter inside the _strom_ project (you created this filter before in order to save the {% indexfile test.tre %} file) and choose _Add > New Item..._ from the popup menu. Select _Utility_ from the _Visual C++_ menu on the left, then _Text File (.txt)_ form the main panel. Name the file {% indexfile rbcL.nex %}.

{% endif %}
[//]: ################################# ENDIF ######################################
Here is a Nexus-formatted data file containing 60 sites from the large subunit of the gene encoding the enzyme RuBisCO (rbcL). Save this as the contents of {% indexfile rbcL.nex %}:
~~~~~~
{{ "assets/data/rbcLshort.nex" | polsnippet:"1-29","" }}
~~~~~~
{:.bash-output}

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################
In order to get {% indexfile rbcL.nex %} installed to the same directory as the strom executable file, we need to add one more `install_data` line to the very end of our {% indexfile meson.build %} file:
~~~~~~
{{ "steps/step-09/src/meson.build" | polcodesnippet:"start-!near_end","a,b,c,near_end" }}
~~~~~~
{:.meson}
I've also highlighted 3 other lines here. The new line that defines the dependency `lib_filesystem` and the addition of this dependency to the `executable` command are both needed in order to show the current working directory (see below). Also highlighted is a line that copies the `strom.conf` to the install directory.

{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################

{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################
Right-clicking the _strom_ project in the Solution Explorer pane, choosing _Properties_, then _Debugging_, and finally looking at _Working Directory_ reveals that a program started running in the debugger will use _$(ProjectDir)_ as its working directory. The directory pointed to by _$(ProjectDir)_ is the directory in which the project file {% indexfile strom.vcxproj %} resides. This is therefore the directory in which you should have created the {% indexfile rbcL.nex %} file if you used the default _Location_ in the _Add New Item_ dialog box. Check, using Windows Explorer, that {% indexfile rbcL.nex %} is in the same directory as {% indexfile strom.vcxproj %}. If not, please move it to that directory before running the program.

{% endif %}
[//]: ################################# ENDIF ######################################

## Specifying the data file

You will need to specify the data file and partition information either on the command line (using `--datafile rbcL.nex --subset first:1-60\3 --subset second:2-60\3 --subset third:3-60\3`) or (better) create a text file named {% indexfile strom.conf %} containing these lines:
~~~~~~
{{ "steps/step-09/test/strom-a.conf" | polcodesnippet:"","" }}
~~~~~~
{:.bash-output}

## Finding the current working directory
We've already arranged for the data file to be in the working directory when strom is run, but users of your program may not know where strom is expecting data files to reside. You can help your future users by getting the program to tell you its current working directory when it starts up. We accomplish this by placing the following (highlighted) line just inside the `Strom::run` function:
~~~~~~
{{ "steps/step-09/src/strom.hpp" | polcodesnippet:"begin_run-!cwd","cwd" }}
~~~~~~
{:.cpp}

This line is what required us to compile the Boost `file_system` and `system` libraries. To use the `current_path` function, you'll note that we included the {% indexfile boost/filesystem.hpp %} header at the top of the {% indexfile strom.hpp %} file. 

## Modifying the Strom::run function

Replace the `try` block in the `Strom::run` function with the highlighted lines below (and also add the highlighted line reporting the current working directory):
~~~~~~
{{ "steps/step-09/src/strom.hpp" | polcodesnippet:"begin_run-end_run","cwd,begin_try-end_try" }}
~~~~~~
{:.cpp}
The new code creates a `Data` object, informs it of the partition set up by the user in the config file or command line, and then reads the data file whose name was provided by the user via the `--datafile` command line option or `datafile` config file option.

## Adding genetic code definitions to main.cpp

The `_definitions` data member of the `GeneticCode` class was declared static, so we need to initialize it outside of the `GeneticCode` class in a source code (cpp) file. As {% indexfile main.cpp %} is the only source code file in our project (all others are header files), we must modify {% indexfile main.cpp %} to initialize {% indexfile _definitions %}.
~~~~~~
{{ "steps/step-09/src/main.cpp" | polcodesnippet:"","start_definitions-end_definitions" }}
~~~~~~
{:.cpp}
  
## Expected output
After running the program, the output should contain these lines, indicating that there were 14 taxa, 3 partition subsets, 60 sites divided equally into each subset, and a total of 9 + 5 + 17 = 31 data patterns found in the {% indexfile rbcL.nex %} file:
~~~~~~
{{ "steps/step-09/test/reference-output/output-a.txt" | polcodesnippet:"","" }}
~~~~~~
{:.bash-output}

## Something to try

To further test the data aspect of your program, create a file {% indexfile datatest.nex %} containing the DATA block from the {% indexfile rbcL.nex %} file plus two other blocks: (1) a CHARACTERS block containing the amino acid translation of the rbcL nucleotide data (as a test of reading protein data); and (2) a CHARACTERS block containing 5 (contrived) discrete morphological characters:
~~~~~~
{{ "assets/data/datatest.nex" | polsnippet:"1-64","" }}
~~~~~~
{:.bash-output}

Now modify your {% indexfile strom.conf %} file to look like this:
~~~~~~
{{ "steps/step-09/test/strom-b.conf" | polcodesnippet:"","" }}
~~~~~~
{:.bash-output}
Note that I've commented out the commands used for reading {% indexfile rbcL.nex %} by placing a hash (`#`) character at the beginning of each line. I've added a `datafile` command to read {% indexfile datatest.nex %} and `subset` commands to interpret the nucleotide data as codons, the protein data as type protein, and the morphological data as type standard. Note the site specification 1-20 rather than 1-60 for the first subset statement (storing 20 codons for each taxon rather than 60 nucleotides).

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################
You should also add the following highlighted line to the bottom of your {% indexfile meson.build %} file if you want {% indexfile datatest.nex %} to be copied to the installation directory when strom is built.
~~~~~~
{{ "steps/step-09/src/meson.build" | polcodesnippet:"","end" }}
~~~~~~
{:.meson}
{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################

{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################

{% endif %}
[//]: ################################# ENDIF ######################################

Running the program again should now yield the following output:
~~~~~~
{{ "steps/step-09/test/reference-output/output-b.txt" | polcodesnippet:"","" }}
~~~~~~
{:.bash-output}


