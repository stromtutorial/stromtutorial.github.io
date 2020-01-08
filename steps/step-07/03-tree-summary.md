---
layout: steps
title: The TreeSummary class
partnum: 7
subpart: 3
description: The `TreeSummary` class will read a tree file, keep track of the number of distinct tree topologies found therein, and count the number of trees having each unique topology.
---
{{ page.description | markdownify }}

This ability is essential for summarizing a posterior sample of trees. It allows one to construct the 95% credible set of tree topologies, for example, and sets the stage for constructing a majority rule consensus tree.

Create a new class named `TreeSummary` and store it in a header file named {% indexfile tree_summary.hpp %}.

Here is the `TreeSummary` class declaration. The class has 2 data members. The vector `_newicks` stores the tree descriptions of all trees read from a tree file. The vector `_treeIDs` stores a map that associates a key in the form of a {% indexshow tree ID %} (the set of all splits in a tree) with a value in the form of a vector of integers, where each integer in the vector is the index of a tree description stored in `_newicks`. Each separate tree ID corresponds to a unique topology, and the associated vector allows us to count and, if desired, print out all trees having that topology.
~~~~~~
{{ "steps/step-07/src/tree_summary.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## Constructor and destructor

The constructor and destructor function do nothing except report that a `TreeSummary` object was created or destroyed, respectively.
~~~~~~
{{ "steps/step-07/src/tree_summary.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The getTree member function

This function returns a shared pointer to a tree built from the tree description stored in `_newicks[index]`. If index is too large, an `XStrom` exception is thrown.
~~~~~~
{{ "steps/step-07/src/tree_summary.hpp" | polcodesnippet:"begin_getTree-end_getTree","" }}
~~~~~~
{:.cpp}

## The getNewick accessor
~~~~~~
{{ "steps/step-07/src/tree_summary.hpp" | polcodesnippet:"begin_getNewick-end_getNewick","" }}
~~~~~~
{:.cpp}

## The clear member function

The clear function simply empties the `_treeIDs` and `_newicks` vectors.
~~~~~~
{{ "steps/step-07/src/tree_summary.hpp" | polcodesnippet:"begin_clear-end_clear","" }}
~~~~~~
{:.cpp}

## The readTreefile member function

This function reads the tree file specified by `filename`, skipping the first skip trees. We make use of the NCL’s `MultiFormatReader` class to find all TAXA blocks. If the NEXUS-formatted tree file does not contain a TAXA block, the NCL will create one for the taxa that it finds in the TREES block. For each TAXA block found, the member function `clear()` is called to reset the `TreeSummary` object. (Typically, there will be only one TAXA block in each tree file, but if there happens to be more than one, only the last will be stored.) The `readTreefile` function then stores each newick tree description found in `_newicks`, and, for each, builds the tree using the `TreeManip::buildFromNewick` function and uses `TreeManip::storeSplits` to create a tree ID (called `splitset`) for the tree. If the tree ID for this tree has never been seen, it creates a new entry in the `_treeIDs` map for this tree ID and associates it with a vector of length 1 containing the index of this tree. If the tree ID has already been seen, it simply adds the current tree index to the vector already associated with that tree ID.
~~~~~~
{{ "steps/step-07/src/tree_summary.hpp" | polcodesnippet:"begin_readTreefile-end_readTreefile","" }}
~~~~~~
{:.cpp}

## The showSummary member function

Finally, add the body of the `showSummary` function, which reports the information stored in `_treeIDs` in a couple of different ways. It first outputs the number of trees it read from the file. Next, it iterates through all distinct tree topologies stored in `_treeIDs`, reporting the index of each tree having that topology. It finishes by producing a table showing the number of trees found for each distinct tree topology, sorted from most frequent topology to the least frequent topology.
~~~~~~
{{ "steps/step-07/src/tree_summary.hpp" | polcodesnippet:"begin_showSummary-end_showSummary","" }}
~~~~~~
{:.cpp}
