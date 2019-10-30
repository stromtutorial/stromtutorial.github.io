---
layout: steps
title: Create the Data class
partnum: 9
subpart: 3
description: Create a class that can read sequence data files and store the data compactly.
---
{% assign OS = site.operatingsystem %}
Create a new C++ class named `Data`{% indexhide Data class %} and add it to your project as the header file {% indexfile data.hpp %}. Below is the class declaration. The body of each member function will be described separately (you should add each of these member function bodies just above the right curly bracket that terminates the namespace block).
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

The class declaration above defines a number of types that are introduced to simplify member function prototypes and data member definitions that follow:
* `taxon_names_t` is used for variables that store a vector of taxon names
* `state_t` is used for storing discrete character state codes
* `pattern_vect_t` is used for variables that store site patterns (vectors of state codes)
* `monomorphic_vect_t` is used for variables that store information about the state present at each constant site pattern
* `partition_key_t` is used for variables that map site patterns to partition subsets 
* `pattern_map_t` is used for variables that map pattern counts to site patterns
* `data_matrix_t` is used for variables that store vectors of site patterns
* `pattern_map_vect_t` is used for a vector of pattern maps, one for each subset
* `pattern_counts_t` is used for variables that store the number of sites having each data pattern
* `subset_end_t` is used for a variable that stores (one past) the last pattern in each subset
* `npatterns_vect_t` is used to store a vector of counts of the number of distinct data patterns in each subset
* `begin_end_pair_t` is used to store a `std::pair` containing the beginning and ending pattern (index into the columns of `_data_matrix`) for a given subset
* `SharedPtr` is a shared pointer to an object of type `Data`

These type definitions make it simpler to define variables of these types and to pass such variables into functions. Most of these are used at the bottom of the class declaration to declare variables that will store information read from a data file (`_pattern_counts`, `_partition_key`, `_pattern_map_vect`, `_taxon_names`, `_data_matrix`, and `_subset_end`).

## Constructor and destructor
Here are the bodies of the constructor and destructor. As usual, the only thing that we have these functions do is to report when an object of the `Data` class is created or destroyed, and these lines have been commented out (you can uncomment them at any time for debugging purposes).
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The setPartition member function
The `Data` class needs access to the partition information that the user supplied in the {% indexfile strom.conf %} file. This information is stored in a {% indexcode Partition %} object, and this function provides a way to assign a `Partition` object to this `Data` object.
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_setPartition-end_setPartition","" }}
~~~~~~
{:.cpp}

## Obtaining partition information
The next 3 member functions provide access: (1) to the `Partition` object (`getPartition`, through a shared pointer); (2) to the number of data subsets in the partition (`getNumSubsets`); and (3) to the names assigned to the data subsets by the user (`getSubsetName`).
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_getPartition-end_getSubsetName","" }}
~~~~~~
{:.cpp}

## Accessors
Next come 5 member functions (`getPartitionKey`, `getPatternCounts`, `getMonomorphic`, `getTaxonNames`, and `getDataMatrix`) that are all {% indexshow accessors %}. 
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_getPartitionKey-end_getDataMatrix","" }}
~~~~~~
{:.cpp}

## The getSubsetBeginEnd member function
The `getSubsetBeginEnd` function returns a `std::pair` in which the `first` and `second` elements are the beginning and ending pattern index in `_data_matrix` for the subset index supplied as an argument. The beginning index is the index to the first pattern in the focal subset, while the ending index is one _beyond_ the last pattern in the subset (i.e. similar to C++ iterators).
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_getSubsetBeginEnd-end_getSubsetBeginEnd","" }}
~~~~~~
{:.cpp}

## The clear member function
Here is the body of the `clear` function, which simply empties data members that are vectors or maps.
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_clear-end_clear","" }}
~~~~~~
{:.cpp}

## The getNumPatterns member function
The function `getNumPatterns` returns the total number of patterns stored in all subsets (which is simply the length of any row of the `_data_matrix` vector after the data have been compressed into unique patterns and pattern counts). 
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_getNumPatterns-end_getNumPatterns","" }}
~~~~~~
{:.cpp}

## The calcNumPatternsVect member function
The  `calcNumPatternsVect` function returns a vector containing the number of patterns in each subset. The name begins with "calc" to indicate that the return value is a vector that is constructed on the fly (and thus involves more work than simply returning a stored value). 
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_calcNumPatternsVect-end_calcNumPatternsVect","" }}
~~~~~~
{:.cpp}

## The getNumStatesForSubset member function
The `getNumStatesForSubset` function returns the number of states characterizing a specified data subset (e.g. 4 for nucleotide data, 61 for codons using the standard code, 20 for protein data, etc.). 
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_getNumStatesForSubset-end_getNumStatesForSubset","" }}
~~~~~~
{:.cpp}

## The getNumPatternsInSubset member function
The `getNumPatternsInSubset` function returns the number of distinct patterns stored in a specified subset. 
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_getNumPatternsInSubset-end_getNumPatternsInSubset","" }}
~~~~~~
{:.cpp}

## The getNumTaxa member function
The `getNumTaxa` function is an accessor that simply returns the number of taxa. 
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_getNumTaxa-end_getNumTaxa","" }}
~~~~~~
{:.cpp}

## The calcSeqLen member function
The function `calcSeqLen` returns the total number of sites stored, but it cannot simply return the number of elements in a row of the `_data_matrix` vector because that would equal the number of unique patterns, not the number of sites, assuming that `compressPatterns` has been called. The {% indexcode std::accumulate %} function sums the values in a vector between two elements delimited by the iterators provided in the first two arguments. The vector begin accumulated is `_pattern_counts`. The third argument supplies the initial value (zero in this case).
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_calcSeqLen-end_calcSeqLen","" }}
~~~~~~
{:.cpp}

## The calcSeqLenInSubset member function

The function `calcSeqLenInSubset` returns the number of sites stored in the subset whose index is provided. We again use the `std::accumulate` function, but need to provide it with the correct begin and end iterators. The indices representing the first and (one beyond the) last pattern in the  subset is returned as the `std::pair` `x` by `getSubsetBeginEnd`. The iterator to the beginning of the needed range is equal to `_pattern_counts.begin()` plus `x.first`. The end iterator is obtained by advancing `_pattern_counts.begin()` by `x.second` units. 

For example, if the first subset contains 5 patterns, then the first pattern in the second subset is obtained by moving an iterator 5 positions away from the iterator positioned at `_pattern_counts.begin()`. Note that `_pattern_counts.begin()` is not moved at all if the focal subset is the first subset.
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_calcSeqLenInSubset-end_calcSeqLenInSubset","" }}
~~~~~~
{:.cpp}

## The buildSubsetSpecificMaps member function
This function fills a map associating site patterns (keys) with counts (values). There is one such map constructed for each partition subset, and the data member `_pattern_map_vect` is a vector of such pattern maps. A {% indexbold site pattern %} is a vector of states present in all taxa. For example, if there were 4 taxa, then AAAA would be a common site pattern, as would CCCC, GGGG, and TTTT. These constant site patterns (same state present in all taxa) are more common in most data sets than variable site patterns such as AAGG or ACGT. Because the likelihood is identical for two instances of the exact same site pattern, it makes sense to just store such patterns once (and calculate the likelihood for them once) but keep track of how many instances there were of the pattern in the original data set. Such pooling cannot be done across partition subsets, however, because normally some component of the model differs among subsets and thus the likelihood must be computed separately for the same site pattern in different subsets. Hence, we compress each data subset into a pattern map and store a separate map for each subset in `_pattern_map_vect`.

The `buildSubsetSpecificMaps` function first obtains the vector of site ranges from the `Partition` object and clears `_pattern_map_vect` in preparation for refilling it from scratch. Each tuple in the `tuples` vector specifies a range of sites along with the index of the subset to which that range of sites belongs. The `site` loop constructs a pattern (vector of `ntaxa` states) from each site in the range and adds that pattern to the map stored at `_pattern_map_vect[site_subset]`. The work of adding the pattern to the correct map is handle by the function `updatePatternMap` (see below).

A final loop computes and stores the total number of site patterns across all data subsets in the variable `npatterns`, which is the returned value of this function.
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_buildSubsetSpecificMaps-end_buildSubsetSpecificMaps","" }}
~~~~~~
{:.cpp}

## The updatePatternMap member function
This private member function adds 1 to the count stored for the supplied pattern in the supplied partition subset in the `_pattern_map_vect` vector. The function first asks (using the `lower_bound` function of the standard `map` class) what is the first key that equals `pattern`? If there is no entry for the supplied `pattern`, the `lower_bound` function will return `_pattern_map_vect[subset].end()`, in which case `pattern` will be added as a key to `_pattern_map_vect[subset]` with the value initially set to 1 (because this is the first time this pattern has been seen in this subset). If the `lower_bound` function returns an iterator not equal to `_pattern_map_vect[subset].end()`, then the iterator is situated at an actual map item and the count associated with this key is incremented by 1.
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_updatePatternMap-end_updatePatternMap","" }}
~~~~~~
{:.cpp}

## The compressPatterns member function
The member function `compressPatterns` calls `buildSubsetSpecificMaps` to store all data currently in `_data_matrix` in `_pattern_map_vect`, then it replaces the contents of `_data_matrix` with just the unique patterns and stores the number of sites exhibiting each pattern in the `_pattern_counts` data member. Once all the data has been thus compressed into unique site patterns and their counts, the `_pattern_map_vect` is cleared as it is now storing redundant information.

You will notice that before any real work is done, some sanity checks are performed. The first check is to make sure there is data stored (it makes no sense to compress an empty data matrix). Second, the number of stored sites is obtained and used to _finalize_ the `Partition` object. The call to `Partition::finalize` performs other checks, such as confirming that the number of sites specified by the user-defined data partition is the same as the number of sites stored in `_data_matrix`, and that no sites have failed to be assigned to any partition subset.

Some of the details (e.g. the determination of which patterns are constant) will be postponed until the way state codes are stored is explained.
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_compressPatterns-end_compressPatterns","" }}
~~~~~~
{:.cpp}

## The storeTaxonNames member function
This is a helper function used by the `getDataFromFile` member function (discussed below). It stores the taxon labels found in a Nexus taxa block in the vector `_taxon_names`. It is possible for there to be more than one taxa block in a data file. If more than one taxa block is encountered, this function checks that the order and labels for taxa in the second and subsequent taxa blocks are identical to those stored from the first taxa block. If not, an `XStrom` exception is thrown.
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_storeTaxonNames-end_storeTaxonNames","" }}
~~~~~~
{:.cpp}

## The storeData member function
This is a helper function used by the `getDataFromFile` member function (discussed below). It stores the data from a Nexus data or character block and ensures that the data type specified in any partition definition is consistent with the data type of the data/characters block. Details of how data is stored in `_data_matrix` are provided in the documentation for `getDataFromFile`.
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_storeData-end_storeData","" }}
~~~~~~
{:.cpp}

## The getDataFromFile member function
This member function makes use of the NCL (Nexus Class Library) to read and parse the data file. The advantage of using the NCL is that we do not have to deal with all the vagaries in format that people (and programs) use when creating NEXUS-formatted data file. The comment at the beginning of the function provides the [web address of the primary documentation for the NCL](http://phylo.bio.ku.edu/ncldocs/v2.1/funcdocs/index.html). Please refer to this web site if you want a more complete explanation of the NCL functions used here. I will explain the salient features below the code block.
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_getDataFromFile-end_getDataFromFile","" }}
~~~~~~
{:.cpp}

We use the NCL's `MultiFormatReader` class to create an object that can parse a NEXUS-formatted data file. `MultiFormatReader` is a class defined by the Nexus Class Library (NCL). Your program knows about it because of the `#include "ncl/nxsmultiformat.h"` at the top of the file. 
{% if OS == "linux" %}
The actual code for the functions and classes provided by the NCL will not be stored in your program's executable file; it will be loaded on demand from a separate Dynamic Link Library (provided by the file {% indexfile libncl.so %}) after your program begins to execute.
{% elsif OS == "mac" %}
The actual code for the functions and classes provided by the NCL will not be stored in your program's executable file; it will be loaded on demand from a separate Dynamic Link Library (provided by the file {% indexfile libncl.dylib %}) after your program begins to execute.
{% elsif OS == "win" %}
The actual code for the functions and classes provided by the NCL will not be stored in your program's executable file; it will be loaded on demand from a separate Dynamic Link Library (provided by the file {% indexfile libncl.dll %}) after your program begins to execute.
{% endif %}

The `ReadFilepath` function is used to read in the data file whose name is specified in the parameter `filename`. Note that we've supplied the argument `MultiFormatReader::NEXUS_FORMAT` to the `ReadFilepath` function: the NCL can read other common data file formats besides NEXUS, so you could modify your program to read (for example) FASTA-formatted data files by supplying a different argument (i.e. `MultiFormatReader::FASTA_DNA_FORMAT`). (A listing of supported formats may be found in the {% indexfile nxsmultiformat.h %} header file.)

The catch block,
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_catch-end_catch","" }}
~~~~~~
{:.cpp}
is executed only if the NCL encountered a problem reading the file. The {% indexshow ellipsis (...) %} indicates that any exception thrown by `ReadFilepath` will be caught here. The `catch` code deletes all data stored thus far by calling the `DeleteBlocksFromFactories()` function and then re-throws the exception so that your program can catch and report the problem.

When the NCL reads a NEXUS data file, it looks for a DATA or CHARACTERS block and stores the data therein. If the creator of the data file has added a TAXA block, the NCL will associate the sequence data from the DATA or CHARACTERS block with the taxon names from that TAXA block. If no TAXA block was found in the file, the NCL will create a virtual TAXA block using the taxon names in the DATA or CHARACTERS block. In any case, our function first enumerates all TAXA blocks (real or vitual) found and then, for each TAXA block, we can enumerate all DATA or CHARACTER blocks stored under that TAXA block.

The main loop is shown below:
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_mainloop-end_mainloop","" }}
~~~~~~
{:.cpp}

For the first TAXA block read, the taxon names are stored (by the `storeTaxonNames` member function) in the data member `_taxon_names`. For subsequent TAXA blocks, `storeTaxonNames` will simply check to make sure that taxon names are identical to those provided in the first TAXA block. 

For each CHARACTER block read (DATA blocks will also be returned by the `GetNumCharactersBlocks` function), the data in the matrix is stored in the data member `_data_matrix` by the member function `storeData`. 

### How NCL stores data
The NCL stores DNA data not as `A`, `C`, `G`, and `T`, but instead as state codes: 0 for `A`, 1 for `C`, 2 for `G`, and 3 for `T`. An entry in the NEXUS data file that specifies complete ambiguity (e.g. `N` or `{ACGT}`) would be stored as 4. Missing data and gaps are stored as -1 and -2, respectively. Ambiguities that do not represent completely missing data are stored as a value larger than 4, and the function {% indexcode NxsDiscreteDatatypeMapper::GetStateSetForCode %} must be used to unpack this value into a set of states considered possible (see the code block for the `storeData` member function to see how this is done).

### How our program stores data
Our program stores the state for each taxon/site combination as a {% indexbold bit field %}. Consider storing the DNA states A, C, G, T, ? (completely missing), R (i.e. a purine, either A or G), and Y (a pyrimidine, either C or T). We need only 4 bits (each holding a 0 or 1) to store any possible DNA state.
~~~~~~
DNA  binary  decimal
 A    0001      1
 C    0010      2
 G    0100      4
 T    1000      8
 ?    1111     15
 R    0101      5
 Y    1010     10
~~~~~~
{:.bash-output}
Thus, if NCL stores 3 (for T), we will store 8 instead. If NCL stores 4 (missing data), we will store 15 instead. This allows us to handle any type of ambiguity thrown at us. Keep in mind, however, that our state codes are different than NCL's state codes.

You will remember that our program allows a subset to contain codons rather than nucleotides. There are 64 possible codons, so in order to have a bit for each possible codon state, it is necessary to use a variable that occupies 64 bits of memory. For this reason, our program defines `state_t` to be unsigned long, which (conveniently) occupies 64 bits on at 64-bit computer. As a result, our program will only be able to accommodate data subsets of type codon if it is compiled on a 64-bit computer. The following lines inside `storeData` check to make sure that the number of states does not exceed the number of bits in a variable of type `state_t`. If so, an exception is thrown: {% indexhide 64 bit requirement %}
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_bitcheck-end_bitcheck","" }}
~~~~~~
{:.cpp}

### Compressing the data
The final statement in this function calls the `compressPatterns()` member function, which eliminates duplicate patterns from `_data_matrix` and stores the counts of each pattern in `_pattern_counts`. If each sequence has 1000 nucleotides in the data file, then, before calling `compressPatterns()`, each row of `_data_matrix` would be a vector containing 1000 state codes and `_pattern_counts` would be a vector containing 1000 elements each equalling 1. Suppose that there are only 180 distinct site patterns. After calling `compressPatterns()`, each row of `_data_matrix` would now have only 180 state codes and `_pattern_counts` would be 180 elements long (and now many of these numbers are greater than 1).

Now that you understand how states are stored, this loop in the `compressPatterns` member function, which visits each state code (`sc`) in a given pattern (`pc.first`), can be explained:
~~~~~~
{{ "steps/step-09/src/data.hpp" | polcodesnippet:"begin_compresspatterns_mainloop-end_compresspatterns_mainloop","" }}
~~~~~~
{:.cpp}

The local variable `p` keeps track of the pattern index over all subsets. Within each `subset`, `pc` is set in turn to each stored pair from `_pattern_map_vect[subset]`. The first member of each pair is a vector states representing the pattern itself; the second member of each pair is the count of the number of sites having that pattern.

For each pattern, `sc` is set in turn to each state in the pattern and stored in _data_matrix row `t`, column `p`, where `t` is the index of the state within the pattern (`t` stands for "taxon" because a pattern is just the state possessed by each taxon for a particular site). Before this `sc` loop begins, the local variable `constant_state` is set equal to the first state in the pattern. For each subsequent state, `constant_state` is updated using a bitwise AND operation. If `constant_state` makes it through all states in the pattern and is still greater than 0, it means that the pattern is at least potentially constant (a constant or monomorphic pattern is one in which all taxa have the same state).

For example, suppose the pattern for a 4-taxon problem is "AAAA". Here is the succession of values possessed by `constant_state` (showing only the relevant 4 bits):
~~~~~~
A 0001
A 0001 = 0001 & 0001
A 0001 = 0001 & 0001
A 0001 = 0001 & 0001
~~~~~~
{:.bash-output}

Note that `constant_state` equals 1 in the end. Now consider a variable pattern, "AAGG":
~~~~~~
A 0001
A 0001 = 0001 & 0001
G 0000 = 0001 & 0100
G 0000 = 0000 & 0100
~~~~~~
{:.bash-output}

Variable patterns set all bits to zero in `constant_state`. What about a pattern involving some missing data, "CC??"?
~~~~~~
C 0010
C 0010 = 0010 & 0010
? 0010 = 0010 & 1111
? 0010 = 0010 & 1111
~~~~~~
{:.bash-output}

This pattern is considered constant for state C because all the unambiguous states are C. Consider one final example, "RR??":
~~~~~~
R 0101
R 0101 = 0101 & 0101
? 0101 = 0101 & 1111
? 0101 = 0101 & 1111
~~~~~~
{:.bash-output}

In this case, `constant_state` ends up being greater than 0, so it is not a variable site, yet there is no unambiguous state in the entire pattern. In this case, the pattern is consistant with a constant state containing all As or all Gs.

The element of `_monomorphic` corresponding to pattern index `p` is set to the final value of `constant_state`. Thus `_monomorphic` can later used to determine for which states this pattern is potentially constant. This information will be used later to compute the likelihood for the invariable sites rate heterogeneity model.

{% comment %}
NOTE: THIS SECTION IS OBSOLETE AND HAS BEEN COMMENTED OUT
IT IS BEING PRESERVED IN CASE THE KERNIGHAN METHOD IS USED ELSEWHERE
A pattern is assumed to be constant unless proven otherwise (`bool constant_pattern = true`). The so-called {% indexbold Kernighan method %} (from the 2nd. ed. of Kernighan and Ritchie. 1988. The C Programming Language.) is used to determine whether a state code has more than 1 bit set (and is thus ambiguous). 

Consider the following ambiguous state code representing R (a purine: A and G are both possible):
~~~~~~
binary  decimal
0101      5
TGCA
~~~~~~
{:.bash-output}
Perform a bitwise AND operation for this state code (5) and the value one less, i.e. 5 - 1 = 4:
~~~~~~
binary  decimal
0101      5
0100      4
---------------
0100      4  (bitwise AND)
~~~~~~
{:.bash-output}
This operation removes the rightmost bit, but that was not the only bit set, so the result is still greater than zero, indicating that this is an ambiguous state.

Now consider an _unambiguous_ state code representing G:
~~~~~~
binary  decimal
0100      4
TGCA
~~~~~~
{:.bash-output}
Perform a bitwise AND operation for this state code (4) and the value one less, i.e. 4 - 1 = 3:
~~~~~~
binary  decimal
0100      4
0011      3
---------------
0000      0  (bitwise AND)
~~~~~~
{:.bash-output}
This operation always removes the rightmost bit from the larger number, and, in this case, that was the only bit set, so the result equals zero, indicating that this is an unambiguous state.

Given that the state is unambiguous, we ask whether it is the first unambiguous state in this pattern (in which case we set `first` equal to it) or a second or later unambiguous state (in which case we compare to `first` and, if not the same, declare the pattern to be variable).

If the pattern is determined to be constant, the constant state code (`first`) is stored in the data member vector `_monomorphic`, which contains 0 for each variable pattern. Note that `_monomorphic` will also contain 0 for any pattern that contains only ambiguous states because, for a pattern that has no unambiguous states, `first` will still be set to 0. The `_monomorphic` vector will later be used to compute likelihoods for the zero-rate category in the case of a {% indexshow proportion of invariable sites (+I) rate heterogeneity model %}.
{% endcomment %}

### Releasing memory
The `_pattern_map_vect` data member is used by `compressPatterns()` but is cleared before that function returns. It serves as a temporary work space, and does not hold onto its contents once `_data_matrix` and `_pattern_counts` are rebuilt. For the same reason (we have all the data stored in our own data structures now), the function `MultiFormatReader::DeleteBlocksFromFactories` is called to delete data stored by NCL.
