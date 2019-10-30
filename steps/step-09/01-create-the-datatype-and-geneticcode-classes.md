---
layout: steps
title: Create the DataType and GeneticCode classes
partnum: 9
subpart: 1
description: Create two classes needed for storing command line arguments that define partition subsets.
---
## Defining a data partition

We will ultimately want to be able to {% indexshow partition %} our data: assign one model to one subset of sites and a different model to the remaining sites. This tutorial adopts the definition of the term {% indexbold partition %} as a wall or set of walls that subdivide something. In this case, a partition refers to the way in which sites are divided into subsets, each getting its own model. The individual subsets are often (confusingly) also called partitions, but I will use the term {% indexbold data subset %} to represent a "room" and reserve partition for the "walls" separating the rooms. A {% indexshow partition %} or a {% indexshow partitioning %} will be used to denote a particular number and placement of walls.

For example, suppose we want to partition the sequences for a protein coding gene into first, second, and third codon positions. We will use three {% indexbold subset %} commands to do this, either on the command line or (more reasonably) in our {% indexfile strom.conf %} file:
~~~~~~
subset = first:1-60\3
subset = second:2-60\3
subset = third:3-60\3
~~~~~~
{:.bash-output}
These `subset` commands provide arbitrary names (`first`, `second`, and `third`) for the three data subsets composing the partition, and also indicate which sites fall in each subset. The `\3` indicates the step size, also known as the stride, which is by default 1. Thus, `2-60\3` means `2, 2+3, 2+6, 2+9, ...`.

The `Strom` class will take responsibility for grabbing the `first:1-60\3`, `second:2-60\3`, and `third:3-60\3 `strings from the command line or {% indexfile strom.conf %} file; the `Partition` class handles interpreting these strings. The `Partition` class will store a vector of tuples, called `_subset_ranges`, with each tuple comprising four integers: begin site (inclusive), end site (inclusive), step size, and subset index. Each tuple in `_subset_ranges` assigns a subset index (0, 1, 2, ..., `_num_subsets`-1) to a chunk of sites, where a chunk can be either a single site (begin=end), a range of contiguous sites (end > begin, step = 1), or a range of non-contiguous sites that can be defined by stepping from begin to end by steps greater than 1.

For example, assume the following `subset` statements are present in the {% indexfile strom.conf %} file:
~~~~~~
subset = a:1-1234
subset = b:1235,2001-2050
subset = c:1236-2000
subset = d:2051-3000\2
subset = e:2052-3000\2
~~~~~~
{:.bash-output}
The `_subset_ranges` vector would end up looking like this (sorted by beginning site, not subset index):
~~~~~~
index          tuple
  0     (   1, 1234, 1, 0)
  1     (1235, 1235, 1, 1)
  2     (1236, 2000, 1, 2)
  3     (2001, 2050, 1, 1)
  4     (2051, 3000, 2, 3)
  5     (2052, 3000, 2, 4)
~~~~~~
{:.bash-output}
The `_subset_names` vector holds the subset names defined in the `part` commands:
~~~~~~
index   name
  0     "a"
  1     "b"
  2     "c"
  3     "d"
  4     "e"
~~~~~~
{:.bash-output}

### Specifying the data type for a subset

The above examples of partition definitions are fine as long as we can assume that data is always in the form of sequences of nucleotides. What if we wanted to interpret the nucleotide sequences as codons rather than individual nucleotides? It would also be helpful to be explicit about other possible data types, such as protein (amino acid sequences) or discrete morphological data. To handle these cases, we will add an optional data type specification to our subset definition. If the data type is omitted by the user, then that subset is assumed to contain nucleotide sequence data.

For example, the following definitions are possible:
~~~~~~
subset = rbcL[codon,plantplastid]:1-20
subset = tufA[protein]:21-40
subset = morph[standard]:41-45
subset = rDNA:46-1845
~~~~~~
{:.bash-output}
The subset named `rbcL` holds data for 20 {% indexbold codon %} "sites" (but note that the 20 codon sites correspond to 60 nucleotide sites in the data file). The associated {% indexbold genetic code %} is supplied after the word `codon` (the comma separator is required). 

The subset named `tufA` holds data for 20 amino acid sites. 

The subset labeled `morph` holds data from 5 discrete morphological characters. 

Finally, the subset `rDNA` holds data for 1800 nucleotide sites (the nucleotide data type is assumed because the data type was not specified).

We will implement the {% indexcode subset %} command in {% indexfile strom.hpp %} as a command line (or control file) option, but for now let's create the `DataType` and `GeneticCode` classes that will handle storing part of the information provided by the user in these `subset` commands. Both of these classes will assist the `Partition` class (next up), which parses the string that the user supplies to the subset command and stores all information about data partitioning.

## GeneticCode class

{% indexcode GeneticCode %} is a lightweight object that stores information about the genetic codes that are recognized by the program. Its main function is to translate an index into the list of all 64 codons into an index into a list of codons that are relevant to the particular genetic code assumed. Stop codons are not considered valid states in a codon model and are thus removed, so (for example) the standard ("universal") genetic code has only 61 states even though there are 64 possible codons (3 are stop codons). GeneticCode objects also provide the amino acid corresponding to each codon, which is important in determining synonymous vs. non-synonymous transitions when constructing the instantaneous rate matrix.

Create a new C++ class named `GeneticCode` and add it to your project as the header file {% indexfile genetic_code.hpp %}. Below is the class declaration. The body of each member function will be described separately (you should add each of these member function bodies just above the right curly bracket that terminates the namespace block).
~~~~~~
{{ "steps/step-09/src/genetic_code.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

### Constructors and destructor
Two constructors are provided: the first accepts no arguments and sets itself up to represent the standard genetic code; the second accepts a genetic code name string and sets itself to represent that genetic code. Both constructors call the member function `buildGeneticCodeTranslators` (described below) to do all of the work except setting` _genetic_code_name`.
~~~~~~
{{ "steps/step-09/src/genetic_code.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

### The getGeneticCodeName member function
This function can be used to obtain the name of the genetic code currently represented by this object.
~~~~~~
{{ "steps/step-09/src/genetic_code.hpp" | polcodesnippet:"begin_getGeneticCodeName-end_getGeneticCodeName","" }}
~~~~~~
{:.cpp}

### The useGeneticCode member function
This function can be used to change which genetic code the `GeneticCode` object represents after construction. This is in fact the same method that the constructors use.
~~~~~~
{{ "steps/step-09/src/genetic_code.hpp" | polcodesnippet:"begin_useGeneticCode-end_useGeneticCode","" }}
~~~~~~
{:.cpp}

### The getNumNonStopCodons member function
This returns the number of codons that are not stop codons in the genetic code represented by this `GeneticCode` object. This number is simply the length of the `_codons` vector because that vector stores only non-stop codons.
~~~~~~
{{ "steps/step-09/src/genetic_code.hpp" | polcodesnippet:"begin_getNumNonStopCodons-end_getNumNonStopCodons","" }}
~~~~~~
{:.cpp}

### The getStateCode member function
Given an index into a vector of all 64 codons (`triplet_index`), this function returns the index of the same triplet in the `_codons` vector, which is a list of all non-stop codons for the genetic code represented by this `GeneticCode` object. This function will be used to map the 64-state codes used by the Nexus Class Library (NCL) onto the states used by our codon models (codon models do not include the stop codons as states).
~~~~~~
{{ "steps/step-09/src/genetic_code.hpp" | polcodesnippet:"begin_getStateCode-end_getStateCode","" }}
~~~~~~
{:.cpp}

### The getAminoAcidAbbrev member function
This function returns the single-letter abbreviation for an amino acid given its (0-offset) index in the `_all_amino_acids` vector.
~~~~~~
{{ "steps/step-09/src/genetic_code.hpp" | polcodesnippet:"begin_getAminoAcidAbbrev-end_getAminoAcidAbbrev","" }}
~~~~~~
{:.cpp}

### The copyCodons member function
This function copies its own`_codons` vector to the supplied vector reference `codon_vect`. This function will later be used by codon models, which need to know all valid codon states for the appropriate genetic code.
~~~~~~
{{ "steps/step-09/src/genetic_code.hpp" | polcodesnippet:"begin_copyCodons-end_copyCodons","" }}
~~~~~~
{:.cpp}

### The copyAminoAcids member function
This function copies its own`_amino_acids` vector to the supplied vector reference `aa_vect`. This function will later be used by codon models, which need to know the amino acid for each codon state for the appropriate genetic code in order to determine whether a particular transition represents a synonymous or nonsynonymous substitution.
~~~~~~
{{ "steps/step-09/src/genetic_code.hpp" | polcodesnippet:"begin_copyAminoAcids-end_copyAminoAcids","" }}
~~~~~~
{:.cpp}

### The buildGeneticCodeTranslators member function
This function sets up all data members based on the current value of the data member _genetic_code_name. The data member `_amino_acid_map` provides the index of a particular amino acid into the `_all_amino_acids` vector given its single letter abbreviation. The `_codons` and `_amino_acids` data members are vectors containing all non-stop codons and the corresponding amino acids, respectively, for the chosen genetic code. Finally, _genetic_code_map provides the codon state code for a nucleotide triplet index. For example, in the standard genetic code, TTT is the 64th triplet but its state code is 61 because 3 triplets represent stop codons and are not counted as states.
~~~~~~
{{ "steps/step-09/src/genetic_code.hpp" | polcodesnippet:"begin_buildGeneticCodeTranslators-end_buildGeneticCodeTranslators","" }}
~~~~~~
{:.cpp}

### The getRecognizedGeneticCodeNames member function
This is a static member function (which means it can be called even if no objects of the GeneticCode class have been constructed) that builds and returns a vector of genetic code names. The genetic code names used are the keys of the `_definitions` map, which, as a static variable, must be instantiated in a source code file (not a header file). In our case, the only source code file is {% indexfile main.cpp %}, which is where all static variables are initialized.
~~~~~~
{{ "steps/step-09/src/genetic_code.hpp" | polcodesnippet:"begin_getRecognizedGeneticCodeNames-end_getRecognizedGeneticCodeNames","" }}
~~~~~~
{:.cpp}

### The isRecognizedGeneticCodeName member function
This is a static function that returns `true` if the supplied string `name` represents a valid genetic code name stored as a key in the (static) `_definitions` vector. Note that `name` is converted to lower case before the lookup because all keys in `_definitions` are lower case.
~~~~~~
{{ "steps/step-09/src/genetic_code.hpp" | polcodesnippet:"begin_isRecognizedGeneticCodeName-end_isRecognizedGeneticCodeName","" }}
~~~~~~
{:.cpp}

### The ensureGeneticCodeNameIsValid member function
This is a static member function that checks whether the supplied string `name` represents a recognized genetic code. If not, a list of valid genetic code names is printed to standard out and an `XStrom` exception is thrown.
~~~~~~
{{ "steps/step-09/src/genetic_code.hpp" | polcodesnippet:"begin_ensureGeneticCodeNameIsValid-end_ensureGeneticCodeNameIsValid","" }}
~~~~~~
{:.cpp}

## DataType class

{% indexcode DataType %} is a lightweight object that stores the type of data for a partition along with a small amount of associated information, such as the number of discrete states for a data type and the genetic code in the case of the codon data type. A `DataType` object will be associated with each data subset defined in the `Partition` object.

Create a new C++ class named `DataType` and add it to your project as the header file {% indexfile datatype.hpp %}. Below is the class declaration. The body of each member function will be described separately (you should add each of these member function bodies just above the right curly bracket that terminates the namespace block).

~~~~~~
{{ "steps/step-09/src/datatype.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

### Constructor and destructor
The only thing out of the ordinary here is the fact that the constructor sets the data type to be nucleotide by default. This is done by calling the `setNucleotide` member function.
~~~~~~
{{ "steps/step-09/src/datatype.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

### Setters
The four functions `setNucleotide`, `setCodon`, `setProtein`, and `setStandard` are used to change the data type to one of the four recognized types: nucleotide, codon, protein, and standard. The data member `_datatype` is set to a unique value for each data type, and `_num_states` is set to the appropriate number of states. The codon data type makes no sense without an associated genetic code, which is set to `standard` (i.e. the "universal" code) by default and can be changed later using the `setGeneticCode` member function. The number of states for the standard data type (not to be confused with the standard genetic code!) is set to 2 initially and can be dhanged later using the `setStandardNumStates` member function.
~~~~~~
{{ "steps/step-09/src/datatype.hpp" | polcodesnippet:"begin_setNucleotide-end_setStandard","" }}
~~~~~~
{:.cpp}

### Querying the data type
The four member functions `isNucleotide`, `isCodon`, `isProtein`, and `isStandard` allow you to query the data type to determine which of the four possible data types it represents.
~~~~~~
{{ "steps/step-09/src/datatype.hpp" | polcodesnippet:"begin_isNucleotide-end_isStandard","" }}
~~~~~~
{:.cpp}

### Changing the default genetic code and (for standard datatype) number of states
The `setGeneticCodeFromName` member function takes a string argument representing the genetic code to associate with the codon data type. The string suppled should be one of the 17 names returned by the `GeneticCode::getRecognizedGeneticCodeNames` function: `standard`, `vertmito`, `yeastmito`, `moldmito`, `invertmito`, `ciliate`, `echinomito`, `euplotid`, `plantplastid` , `altyeast`, `ascidianmito`, `altflatwormmito`, `blepharismamacro`, `chlorophyceanmito` , `trematodemito`, `scenedesmusmito`, and `thraustochytriummito`. These are defined as the keys of the static map `GeneticCode::_definitions`, which is initialized in the {% indexfile main.cpp %} file. 

The `setGeneticCode` member function allows you to set the genetic code using an existing GeneticCode object by passing in a shared pointer to the existing object.

The `setStandardNumStates` member function takes an integer argument representing the number of discrete character states for a standard data type and sets `_num_states` to that value. Note that it also sets `_datatype`, which means you can use this function instead of `setStandard`, which sets `_num_states` to 2.
~~~~~~
{{ "steps/step-09/src/datatype.hpp" | polcodesnippet:"begin_setGeneticCodeFromName-end_setStandardNumStates","" }}
~~~~~~
{:.cpp}

### The getDataType, getNumStates, and getGeneticCode member functions
These are accessor functions that return the values stored in the private data members `_datatype`, `_num_states`, and `_genetic_code`, respectively.
~~~~~~
{{ "steps/step-09/src/datatype.hpp" | polcodesnippet:"begin_getDataType-end_getGeneticCode","" }}
~~~~~~
{:.cpp}

### The getDataTypeAsString and translateDataTypeToString functions
The `translateDataTypeToString` function is declared static, which means it can be called without reference to any particular `DataType` object. It is used to translate the arbitrary integer code for a datatype into a meaningful name (e.g., "nucleotide", "codon", "protein", or "standard").

The `getDataTypeAsString` function simply calls `translateDataTypeToString`, passing its stored `_datatype` value. This function can be used to query a particular `DataType` object for the name of its particular data type. For the codon data type, the string returned also specifies the name of the associated genetic code.
~~~~~~
{{ "steps/step-09/src/datatype.hpp" | polcodesnippet:"begin_getDataTypeAsString-end_translateDataTypeToString","" }}
~~~~~~
{:.cpp}


