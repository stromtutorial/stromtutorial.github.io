---
layout: steps
title: Test the Model class
partnum: 11
subpart: 6
description: Add program options to specify the model and test the Model class.
---
{% assign OS = site.operatingsystem %}

## Install the Eigen library

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################
Download [Eigen](eigen.tuxfamily.org) as follows (note that there may be a more recent version than 3.3.7, in which case you should modify the `curl` command accordingly):
~~~~~~
cd ~/Documents/libraries
curl -LO http://bitbucket.org/eigen/eigen/get/3.3.7.tar.gz
tar zxvf 3.3.7.tar.gz
rm 3.3.7.tar.gz
~~~~~~
{:.bash}

## Update the meson.build file
Add an entry named `incl_eigen` pointing to the Eigen install directory, and include `incl_eigen` in the list `include_directories`. Eigen is a header-only library, so we do not have to worry about compiling it, only pointing our program to its installation director so that the header files can be found.
~~~~~~
{{ "steps/step-11/src/meson.build" | polcodesnippet:"","a,b" }}
~~~~~~
{:.meson}

{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################
Download the latest stable release of [Eigen 3](eigen.tuxfamily.org). 

Unpack the downloaded zip or tar.gz file (e.g. {% indexfile 3.3.7.tar.gz %}) to your `libraries` directory. For example, on my computer, I now have this directory path: {% indexfile /Users/plewis/Documents/libraries/eigen-eigen-323c052e1731/Eigen %}
I will refer to the {% indexfile eigen-eigen-323c052e1731 %} directory as {% indexcode EIGEN_ROOT %}. Inside `EIGEN_ROOT` is the directory named `Eigen` (starting with a capital E)

You now need to tell Xcode where the installed Eigen header files are located.

Choose _Xcode > Preferences..._ from the main Xcode window, then click the "Locations" tab, and, finally, "Custom Paths".

Click the `+` button and add name `EIGEN_ROOT`, Display Name `EIGEN_ROOT`, and, for Path, enter the full path to the directory you just created: for example, I entered this:
~~~~~~ 
/Users/plewis/Documents/libraries/eigen-eigen-323c052e1731
~~~~~~
{:.bash-output}

This creates an alias that we can use in place of this path to specify where the NCL libraries and headers reside. You may remember that we already created an alias named `BOOST_ROOT` pointing to the Boost install directory and one named `NCL_ROOT` pointing to the Nexus Class Library install directory. Should we ever move the Eigen library, we need only update this "Custom Paths" path and would not have to modify the Xcode project.

Click on the {% indexshow blue project icon %} :blueproject: labeled strom at the top of the Project Navigator (not the {% indexshow yellow folder icon %} :yellowfolder: also labeled strom), then click on the "Build Settings" tab. Type "header search" into the search box. In the "Header Search Paths" row, double-click on the cell in the column with the {% indexshow blue project icon %} :blueproject:. Click the `+` symbol and add `$(EIGEN_ROOT)`, pressing the enter key when finished. This tells Xcode where to find Eigen header files.

Because Eigen is a header-only library, we do not need to perform any other setup: it's ready to go once Xcode knows where to find it!
{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################
Download the latest stable release of [Eigen 3](http://eigen.tuxfamily.org). 

Unpack the downloaded zip or tar.gz file (e.g. {% indexfile 3.3.7.tar.gz %}) to your `libraries` directory. For example, on my computer, I now have this directory path:
~~~~~~
C:\Users\Paul Lewis\Documents\libraries\eigen-eigen-323c052e1731/Eigen
~~~~~~
{:.bash-output}
I will refer to the {% indexfile eigen-eigen-323c052e1731 %} directory as {% indexcode EIGEN_ROOT %}. Inside `EIGEN_ROOT` is the directory named `Eigen` (starting with a capital E)

You now need to tell VSC19 where the installed Eigen header files are located.

Right-click on the _strom_ project in the Solution Explorer and choose _Properties_ to bring up the _strom Property Pages_ dialog box. Select _VC++ Directories > Include Directories_ and add the {% indexfile eigen-eigen-323c052e1731 %} directory to the list of directores that VSC19 will search when trying to locate header files. Be sure to do this for both Release and Debug versions (switch between the two using the _Configuration_ dropdown at the top of the _strom Property Pages_ dialog) to avoid confusion later.

That's all you need to do: Eigen is a header-only library, so we do not have to compile anything in order to begin using the library.

{% endif %}
[//]: ################################# ENDIF ######################################

## Modify Strom

First, add a data member of type `Model` to the private section of the `Strom` class declaration (e.g. just below `Data::SharedPtr _data;`). Add another data member to hold the expected log-likelihood. The `_expected_log_likelihood` data member and the corresponding `expectedLnL` program option will help us in debugging but will be removed once our program is ready to be released. Finally, add declaration for 3 new private member functions, `processAssignmentString`, `handleAssignmentStrings`, and `splitAssignmentString`. The changes needed to the class declaration are highlighted below:
~~~~~~
{{ "steps/step-11/src/strom.hpp" | polcodesnippet:"begin_strom_class-end_strom_class","a-b,c,d" }}
~~~~~~
{:.cpp}

Initialize the new data members in the body of the `Strom::clear` function:
~~~~~~
{{ "steps/step-11/src/strom.hpp" | polcodesnippet:"begin_clear-end_clear","e,f" }}
~~~~~~
{:.cpp}

### Allow user to set model parameter values

Modify the `Strom::processCommandLineOptions` function (in the {% indexfile strom.hpp %} file) as follows to add options to allow the user to set various model parameters (as well as the expected log-likelihood).
~~~~~~
{{ "steps/step-11/src/strom.hpp" | polcodesnippet:"begin_processCommandLineOptions-end_processCommandLineOptions","g-h,ha-hb,i-j,k,l-m" }}
~~~~~~
{:.cpp}
You will note that I have introduced 8 local variables (`partition_statefreq`, `partition_rmatrix`, `partition_omega`, `partition_ratevar`, `partition_pinvar`, `partition_ncateg`, `partition_relrates`, and `partition_tree`) that serve to capture information provided by the user in the configuration file. As these bits of information are captured, they must be transfered to the model. The first thing we must tell the model is the number of partition subsets that the user defined and the data types of those subsets (using the `Model::setSubsetDataTypes` function). Following that line, there are 8 `handleAssignmentStrings` function calls, one for each of the new quantities that the user can potentially modify using the configuration file.

### The handleAssignmentStrings member function
This function checks to see whether the user specified any option in the configuration file with a name equal to `label`. If so, it calls on processAssignmentString to do all the work of parsing the assignment of a parameter value to different subsets. It also checks whether the user specified a default parameter value to be applied to all subsets unless overridden by a later subset-specific assignment. If the user has supplied a default, it must come first in the config file, otherwise it would overwrite assignments already made to specific subsets. If the user did not specify any assignments using the name provided via `label`, then the supplied `default_definition` is applied.
~~~~~~
{{ "steps/step-11/src/strom.hpp" | polcodesnippet:"begin_handleAssignmentStrings-end_handleAssignmentStrings","" }}
~~~~~~
{:.cpp}

### The processAssignmentString member function
This function handles parsing a particular parameter assignment (e.g. strings such as `rmatrix = first,second:1,1,1,1,1,1` or `statefreq = default:.1,.2,.3,.4`). Each of these assignment statements comprises a list of subsets (or the keyword `default`) and a parameter value (which may be multivariate), separated by a colon (`:`) character. The function `splitAssignmentString` is used to separate out the subsets into a vector of strings named `vector_of_subset_names` and a vector of doubles named `vector_of_values`.

The remainder of the function is just a long nested conditional that calls the specific member function of `_model` responsible for handling each possible parameter assignment. In the case of model parameters that can be linked across data subsets (e.g. `statefreq`, `rmatrix`, `omega`, `pinvar`, `ratevar`, and `relrate`), the parameter value is made into a shared pointer and it is the shared pointer that is assigned to different subsets. In the case of fixed settings (e.g. `ncateg`), actual values are assigned to the specified subsets. If the first element of `vector_of_subset_names` is `default`, then the assignment is made to all subsets; otherwise, each subset listed by the user is looked up by name and the assignment is made to that particular set of data subsets. 
~~~~~~
{{ "steps/step-11/src/strom.hpp" | polcodesnippet:"begin_processAssignmentString-end_processAssignmentString","" }}
~~~~~~
{:.cpp}

### The splitAssignmentString member function
The job of this function is to split a string such as `first,second:1,1,1,1,1,1`, `default:.1,.2,.3,.4`, or `first,third:4` into two vectors: a vector of strings named `vector_of_subset_names` storing the subset names (or just the keyword `default`); and a vector of doubles named `vector_of_values` storing the (possibly multivariate) parameter value. These two vectors are supplied by the calling function (`processAssignmentString`) and passed by reference so that they can be filled by `splitAssignmentString`. In the example given, `vector_of_subset_names` would end up containing two strings (`first` and `second`) and `vector_of_values` would contain 6 double values, all equal to 1.0.

The splitting is accomplished using the {% indexcode boost::split %} function, which produces a vector (`twoparts`) containing 2 strings (`first,second` and `1,1,1,1,1,1`). The `first,second` string is assigned to the variable `comma_delimited_subset_names_string` and the `1,1,1,1,1,1` string is assigned to the variable `comma_delimited_value_string`.

This section shown below checks to see if the parameter value in `comma_delimited_value_string` is enclosed in square brackets (using a regular expression). If so, then `comma_delimited_value_string` is assigned to the part inside the square brackets, and a boolean variable `fixed` is set to `true`, indicating that the user wishes for this parameter to have a fixed value (i.e. not updated during an MCMC analysis, for example).
~~~~~~
{{ "steps/step-11/src/strom.hpp" | polcodesnippet:"begin_bracket_check-end_bracket_check","" }}
~~~~~~
{:.cpp}

If `comma_delimited_value_string` is the string `equal`, then `vector_of_values` is assigned the single value -1.0, which will serve as a signal that `equal` was specified.

Assuming `comma_delimited_value_string` does not containg the string `equal`, then `comma_delimited_value_string` is split, again using `boost::split`, at the commas to yield the returned `vector_of_strings` vector, which is then converted to the double vector `vector_of_values` using {% indexcode std::transform %}. The `std::transform` function transforms each individual string in `vector_of_strings` to a double value that is appended to `vector_of_values` by the {% indexshow anonymous lambda %} function provided. The lambda function takes a string argument `vstr` provided by `std::transform` and converts it to a double value using the {% indexcode std::stod %} function.

The `comma_delimited_subset_names_string` is split to yield the returned `vector_of_subset_names` using `boost::split` once again.

Finally, some sanity checks are performed:
* there should be at least one subset name supplied (but that name could be `default`).
* if no partition was defined, then `vector_of_subset_names` should consist of just one element, and that element should be the string `default`.
* if `default` is among the subset names supplied, then it should be the only subset name supplied.

~~~~~~
{{ "steps/step-11/src/strom.hpp" | polcodesnippet:"begin_splitAssignmentString-end_splitAssignmentString","" }}
~~~~~~
{:.cpp}

## Updating the run function
Change the `Strom::run` function in the {% indexfile strom.hpp %} file to accommodate the new lines in blue:
~~~~~~
{{ "steps/step-11/src/strom.hpp" | polcodesnippet:"begin_run-end_run","n-o,p-q,r-s" }}
~~~~~~
{:.cpp}

{% comment %}
## All that work just to get the same answer!

If you run the program now it should produce the exact same output as it did at the end of the last step. This is because our `Model` class, by default, implements the JC69 model with equal rates. Before moving further on this page, compile and run your program to verify that it does indeed produce the log-likelihood -278.838 as before.
{% endcomment %}

## Run the program using the new Model class

Run your program using the `strom.conf` file you created at the beginning of this step (at the end of the section [Specifying a model](Specifying a model)). 

{% if OS == "win" %}
[//]: ################################## WINDOWS ########################################
If VSC19 instructs you to recompile with `/bigobj`, then open the _Configuration Properties_ by right-clicking the strom project and choosing _Properties_ from the popup menu. Choose _C/C++ > Command Line > Additional Options_ and type `/bigobj` in the text area under _Additional Options_. You will probably only need to do this for the Debug version.
{% endif %}
[//]: ################################# ENDIF ######################################


The addition of the `expectedLnL` line provides the log-likelihood we expect our program to compute. Running the program should produce the expected log-likelihood. Your program now has the capability to compute the likelihood under a model involving data partitioning.
