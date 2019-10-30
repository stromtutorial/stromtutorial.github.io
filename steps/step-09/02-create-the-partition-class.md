---
layout: steps
title: Create the Partition class
partnum: 9
subpart: 2
description: Create the class that interprets user specification and storage of partition information.
---
## Partition class

A `Partition` object will serve as the manager for data subsets defined by the user. It will store the name of each data subset, the range(s) of sites included in each subset, and information about the type of data in each subset.

Create a new C++ class named {% indexcode Partition %} and add it to your project as the header file {% indexfile partition.hpp %}. Below is the class declaration. The body of each member function will be described separately (you should add each of these member function bodies just above the right curly bracket that terminates the namespace block).
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

The class declaration above defines several types that are introduced to simplify member function prototypes and data member definitions that follow:
* `regex_match_t` is used for regular expression match object variables
* `subset_range_t` is a {% indexcode std::tuple %} comprising four `unsigned int`s storing the beginning site, ending site, step size, and partition subset index for a chunk of sites
* `partition_t` is used for the vector of `subset_range_t` objects that place every site into one and only one partition subset
* `datatype_vect_t` is used for the vector of `DataType` objects that store information about the type of data stored in each subset
* `subset_sizes_vect_t` is a vector of unsigned integers that stores the number of sites assigned to each partition subset
* `subset_names_vect_t` is a vector of strings that stores the label that the user assigned to each partition subset
* `SharedPtr` is the shared pointer used for passing around Partition objects

These type definitions make it simpler to define variables of these types and to pass such variables into functions.

### Constructor and destructor
Here are the bodies of the constructor and destructor. As usual, the only thing that we have these functions do is to report when an object of the `Partition` class is created or destroyed, and these lines have been commented out (you can uncomment them at any time for debugging purposes). In addition, the constructor calls the `clear` member function to perform initializations. 

The data member {% indexcode _infinity %} needs some explanation. This data member has type `const unsigned` and as a `const` data member must be initialized before the `Partition` object has been created. Thus, we cannot initialize it in the body of the constructor, because, by that time, the object already exists. Hence, `_infinity` is initialized via the initializer list (before the left curly bracket that opens the constructor body). The data member `_infinity` is set equal to the largest possible `unsigned` value, which is used as a stand-in for the total number of sites until we actually know the total number of sites. The value itself is obtained using the (static) `max` function of the class `std::numeric_limits<unsigned>`. The `#include <limits>` line at the top of the file provides access to the {% indexcode numeric_limits %} class.
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

### Accessor functions
The functions {% indexcode getNumSites %}, {% indexcode getNumSubsets %}, {% indexcode getSubsetName %}, {% indexcode getSubsetRangeVect %}, {% indexcode getDataTypeForSubset %}, and {% indexcode getSubsetDataTypes %} provide access to the values stored in the private data members `_num_sites`, `_num_subsets`, `_subset_names`, `_subset_ranges`, and `_subset_data_types`, respectively, but do not allow you to change those variables.
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_getNumSites-end_getSubsetDataTypes","" }}
~~~~~~
{:.cpp}

### The findSubsetByName member function
This function returns the (0-based) index of the subset in `_subset_names` corresponding to the `subset_name` provided. If no subset by that name can be found, an exception is thrown. Here the {% indexcode std::find %} function is used to search for `subset_name` in `_subset_names`. If found, the returned iterator will _not_ equal `_subset_names.end()`, which represents the position just beyond the last element of the vector. Once the iterator is positioned at the correct element of `_subset_names`, the index is found using the {% indexcode std::distance %} algorithm, which computes the distance between the returned iterator and the first element of the vector.
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_findSubsetByName-end_findSubsetByName","" }}
~~~~~~
{:.cpp}

### The findSubsetForSite member function
This function returns the subset index corresponding to a given site. Subset indices start at 0 and are indexed according to the order in which they are specified. Sites are numbered starting from 1, which is the convention used in, for example, NEXUS formatted data files. 

The information for each chunk of sites is stored in a 4-tuple. A {% indexcode std::tuple %} is a structure that contains a fixed number of values (in our case 4) and is a generalization of {% indexcode std::pair %}, which represents a 2-tuple. The {% indexcode std::get %} template returns a reference to the kth element of the tuple t, where k is specified in the angle brackets and t in parentheses. Each value in the 4-tuple is copied to a local variable for purposes of clarity.
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_findSubsetForSite-end_findSubsetForSite","" }}
~~~~~~
{:.cpp}

### The siteInSubset member function
This function simply returns `true` if the specified site (1,2,...,`_num_sites`) is in the specified subset (0, 1, ..., `_num_subsets`-1), and returns `false` otherwise. It uses `findSubsetForSite` to do the heavy lifting. Note that sites are numbered starting with 1 in NEXUS data files but a 0-based indexing system is used for everything else in our program.
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_siteInSubset-end_siteInSubset","" }}
~~~~~~
{:.cpp}

### The numSitesInSubset member function
This function calculates the number of sites assigned to the subset having index `subset_index`. This involves looping through all the subset ranges in `_subset_ranges` and, for all ranges assigned to `_subset_index`, determining how many sites are included.

This process would be uncomplicated were it not for the third element of each range (the step size or stride), which can be greater than 1 and which necessitates the use of the modulus operator to determine whether the range includes one more site than is suggested by `floor(n/stride)`. For example, suppose `begin_site` is 1, `end_site` is 10, and `stride` is 3. A total of 4 sites are included (sites 1, 4, 7, 10), yet 10/3 is only 3.
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_numSitesInSubset-end_numSitesInSubset","" }}
~~~~~~
{:.cpp}

### The calcSubsetSizes member function
This function returns a vector of subset sizes using the same approach used by `numSitesInSubset` to count the numnber of sites falling in each subset. This function is useful for reporting information about the partition to the user, and these subset sizes are needed (as we shall later see) when normalizing the subset relative rates of substitution.
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_calcSubsetSizes-end_calcSubsetSizes","" }}
~~~~~~
{:.cpp}

### The clear member function
Like other `clear` functions in this tutorial, this function is called by the constructor (but could be called at other times) and returns the `Partition` object to the just-constructed state. Note that the `clear` function creates a `default` partition consisting of a single subset range with begin site `1`, end site `_infinity`, step size `1`, and subset index `0`. As soon as the user adds the first subset definition, this default partition will be deleted. Assuming that the user does not define a partition, then as soon as data are read (i.e. when the `Partition::finalize` member function is called), the `_infinity` will be replaced by the actual total number of sites.
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_clear-end_clear","" }}
~~~~~~
{:.cpp}

### The parseSubsetDefinition member function
This function provides the primary route by which partition subsets are added. It takes a string `s` representing everything after the keyword "subset" in a configuration file and splits `s` at the colon to yield two strings, `before_colon` (e.g. "rbcL[codon,plantplastid]") and `subset_definition` (e.g. "1-20").

The use of `boost::split` to split `s` at the colon character requires including the header file {% indexfile boost/algorithm/string/split.hpp %} and the use of the {% indexcode boost::is_any_of %} predicate requires including the header file {% indexfile boost/algorithm/string/classification.hpp %}. You will find that we indeed did include these headers at the beginning of the {% indexfile partition.hpp %} file:
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"start-end_headers","a,b" }}
~~~~~~
{:.cpp}

The main complication faced by `parseSubsetDefinition` is that the string `before_colon` may be either just the subset label chosen by the user (e.g. "rbcL"), or it may be a subset name followed by a subset data type specification embedded in square brackets (e.g. "rbcL[codon,plantplastid]"). The subset name and the subset data type (if it is there) are extracted through the use of regular expressions using the {% indexcode std::regex_match %} function. The regular expression pattern used is explained below. 
~~~~~~
R"((.+?)\s*(\[(\S+?)\])*)"
   ^^^^^ non-greedy sequence of 1 or more characters
R"((.+?)\s*(\[(\S+?)\])*)"
        ^^^ 0 or more whitespace characters
R"((.+?)\s*(\[(\S+?)\])*)"
           ^^^^^^^^^^^^^ captures subset data type specification if it is there
R"((.+?)\s*(\[(\S+?)\])*)"
            ^^      ^^ literal left/right square brackets (preceded by backslash
                       because brackets have special meaning in regular expressions)
R"((.+?)\s*(\[(\S+?)\])*)"
              ^^^^^^ 1 or more darkspace characters (this should be "nucleotide",
                     "protein", "standard", or "codon" but we will check this later                      
                     because we don't want the entire regular expression to fail if
                     the user has specified an invalid data type here
~~~~~~
{:.bash-output}
First of all, this is an example of a {% indexbold raw literal string %}. The beginning `R"(` and the ending `)"` form a wrapper that identifies this as a raw string and thus these 5 characters are not actually part of the pattern string. The reason we use a raw string here is that regular expression patterns are full of backslash characters, which, in regular expressions, signal that the following character has special significance. For example, if `\d` appears in a regular expression pattern, it means that `d` means "digit character" rather than just the letter `d`. If a raw string is not used, then each backslash character must be "escaped" by preceding it with a second backslash. This leads to lots of double backslash character combinations, which make regular expression patterns, already difficult to comprehend, even more difficult to construct correctly.

The resulting `match_obj` is a vector of length either 2 or 4. It has length 2 if the user did not specify a data type at all ("nucleotide" is assumed), or 4 if the user did specify a data type in square brackets. The reason there are two extra elements is that we captured not only the data type itself, but also the entire expression in square brackets (in order to test whether it was there). Thus, the specification `rbcL[codon,plantplastid]` would result in the following `match_obj` vector:
~~~~~~
match_obj[0] = "rbcL[codon,plantplastid]"
match_obj[1] = "rbcL"
match_obj[2] = "[codon,plantplastid]"
match_obj[3] = "codon,plantplastid"
~~~~~~
{:.bash-output}
whereas the specification `rbcL` (which is effectively the same as `rbcL[nucleotide]`) would result in:
~~~~~~
match_obj[0] = "rbcL"
match_obj[1] = "rbcL"
~~~~~~
{:.bash-output}

If the data type is "codon", then the user may have either specified a particular genetic code name (e.g. "plantplastid") or not, in which case the "standard" (i.e. universal) genetic code is assumed. A further regular expression is used to detect whether the data type is "codon" _and_ a genetic code was specified. Several `else` clauses handle all other possible data types, including the codon data type where the default genetic code is assumed.

The function then adds `subset_name` to the `_subset_names` vector, adds the data type `dt` to `_subset_data_types`, updates `_num_subsets`, then calls `addSubset` to do all of the work involved in interpreting the `subset_definition` string. Note that this function deletes the default partition (created in the constructor): if this function is ever called, it is because the user has defined a partition and thus the default partition is not needed.

Here's the entire function body:
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_parseSubsetDefinition-end_parseSubsetDefinition","" }}
~~~~~~
{:.cpp}

### The addSubset member function
This is a private member function that does the work of breaking up a subset definition into a vector of component ranges (these component ranges are separated by commas in `subset_definition`). This is accomplished by the {% indexcode boost::split %} function, and the resulting components are each submitted to the member function `addSubsetRange`.
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_addSubset-end_addSubset","" }}
~~~~~~
{:.cpp}

## The addSubsetRange member function

This is a private member function, called by the `addSubset` member function, that receives a subset index (an integer greater than or equal to 0) and a range definition, which may be trivial (just a single integer corresponding to one site), a range (e.g. 1-1000) consisting of a beginning and ending site separated by a hyphen, or a more complex range comprising a beginning and ending site as well as a step size, or stride. [Regular expression](https://en.wikipedia.org/wiki/Regular_expression) matching is used to parse the range definition. The regular expression pattern string is
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"regex","" }}
~~~~~~
{:.cpp}
Note that the `R"(...)"` wrapper just tells C++ that this is a raw string (it tells C++ not to escape characters preceded by a backslash). After removing the raw literal bracketing characters, the regular expression pattern is shown below with the 5 potential capture groups indicated below:
~~~~~~
(\d+)\s*(-\s*([0-9.]+)(\\\s*(\d+))*)*
|-1-|   |--------------2-----------|
             |---3---||-----4----| 
                            |-5-|
~~~~~~
{:.bash-output}
The first construct, `(\d+)`, looks for one or more digits, and the parentheses serve to capture this number as regex group 1. Group 2 occupies most of the remainder of the pattern and the terminating `*` means that group 2 may not even be present in a match. This would be the case for trivial ranges consisting of only a single number representing the site position.

Assuming group 2 is present in a match, group 3 is required and captures the ending site position, which follows the required hyphen character (`-`) and zero or more whitespace characters (`\s*`). Group 4 is not required, but, if present, matches a backslash character (`\\`), which must be doubled ("escaped") in order to keep it from acting to make the following character special, followed by a potential space (`\s*`) and then a series of digit characters (`\d+`) captured as group 5. (The fact that backslashes are escaped in this regular expression itself explains why I decided to go with the raw literal string approach; otherwise, backslashes would need to be escaped for _both_ C++ _and_ the regular expression interpreter!)

The function {% indexcode std::regex_match %} searches the supplied `range_definition` for the pattern defined by `re` and stores any captured groups in `match_obj`.

Note that groups 2 and 4 are both optional. These groups are only defined so that we can make them optional. It is groups 1, 3, and 5 that capture the information we need. The three lines that call `extractIntFromRegexMatch` do the work of assigning these pieces of information to the variables `ibegin`, `iend`, `istep`. The second argument to `extractIntFromRegexMatch` is the default value that is used if the capture group specified as the first argument is the empty string.

All that remains is to append the tuple containing the four values `ibegin`, `iend`, `isteo`, and `subset_index` to the vector `_subset_ranges`, as well as update `_num_sites` if the last site included in the range is larger than the current value of `_num_sites`. This last task is complicated by the possibility that the last site included in the range may not equal `iend`! Consider the range `2-10\3`, which translates to the set `{2, 5, 8}`. The last site included is 8, not 10.

Here is the source for the `addSubsetRange` function:
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_addSubsetRange-end_addSubsetRange","" }}
~~~~~~
{:.cpp}

## The extractIntFromRegexMatch member function
This function takes a `regex_match_t` argument and attempts to interpret it as an integer. A `regex_match_t` is an object that captured part of a regular expression match; the match object can be converted to a `std::string` using the object's `str` member function. 

If the attempt to convert the match to an integer succeeds, then the function returns the integer value extracted, assuming that the integer is at least as large as the minimum value specified. If the supplied string is empty, then no attempt is made to interpret it and the supplied minimum value is returned by default.
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_extractIntFromRegexMatch-end_extractIntFromRegexMatch","" }}
~~~~~~
{:.cpp}
The function uses {% indexcode std::stoi %} to extract an integer value from a string. If std::stoi fails to interpret the supplied string as an integer, it throws a {% indexcode std::invalid_argument %} exception, which we catch and follow up with our own {% indexcode XStrom %} exception to explain to the user what went wrong.

## The finalize member function
The finalize function is called once the actual number of sites is known (after the data have been stored). This function performs three important sanity checks. First, it checks whether the number of sites specified is equal to the number of sites determined by the subset definitions. Second, it checks whether any sites have slipped through the cracks and were not assigned to any subset in the partition. Third, it checks whether any sites have been assigned to more than one subset. If any of these sanity checks fail, an `XStrom` exception is thrown, forcing the user to fix their partition definition.
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_finalize-end_finalize","" }}
~~~~~~
{:.cpp}

## The defaultPartition member function
If data will not be partitioned, it makes sense to relieve the user of the responsibility of creating a subset definition. This function may be called if no subset definitions were provided on the command line (or {% indexfile strom.conf %} file). It simply creates a `_subset_ranges` vector containing a single range tuple specifying a range that extends from the first to the last site. It also adds the name `default` to the `_subset_names` vector.
~~~~~~
{{ "steps/step-09/src/partition.hpp" | polcodesnippet:"begin_defaultPartition-end_defaultPartition","" }}
~~~~~~
{:.cpp}

