---
layout: steps
title: The TreeUpdater class
partnum: 17
subpart: 1
description: Create an updater that modifies the tree topology and edge lengths. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

The `TreeUpdater` class updates three edge length proportions and (potentially) the tree topology using a modified version of the Larget and Simon (1999) LOCAL move for unrooted trees.

Create a new header file named {% indexfile tree_updater.hpp %} and copy into it the following class declaration.
~~~~~~
{{ "steps/step-17/src/tree_updater.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## The constructor and destructor

This class can simply use the base class (`Updater`) version of the clear function in the constructor and, as usual, the destructor has no work to do.
~~~~~~
{{ "steps/step-17/src/tree_updater.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The reset member function

Recall that the `reset` function is called at the end of the `Updater::update` function to prepare the updater for the next update. Any temporary variables needed by an updater during the update process need to be re-initialized in this function, and this `TreeUpdater` has several of these temporary variables, most of which are needed to keep track of how to revert if the proposed tree is rejected.
~~~~~~
{{ "steps/step-17/src/tree_updater.hpp" | polcodesnippet:"begin_reset-end_reset","" }}
~~~~~~
{:.cpp}

## The calcLogPrior member function

Now that we’ve created the `calcLogTopologyPrior` function and (previously, in the `Updater` class) the `calcEdgeLengthPrior` function, the `calcLogPrior` function simply needs to call those two functions and return the sum of the values they calculate.
~~~~~~
{{ "steps/step-17/src/tree_updater.hpp" | polcodesnippet:"begin_calcLogPrior-end_calcLogPrior","" }}
~~~~~~
{:.cpp}

## The proposeNewState member function

This function, which overrides one of the 3 pure virtual functions defined by the base class `Updater`, is unfortunately rather long and tedious owing to the fact that the first step in the Larget-Simon LOCAL move, which involves choosing 3 adjacent random internal edges, leads to 8 distinct possible proposed states. I’ve tried to make following the code in this function easier by creating diagrams in the comments showing the relationships among all the temporary variables, but to fully understand this function, you will need to read and understand the description of this move in the papers by Larget and Simon (1999). Keep in mind that we are modifying the original method; specifically we do not expand or contract the 3-edge segment before moving one of the attached nodes to a different place. This means that the Hastings ratio is 1.0 because the move is now symmetric, and the discussion of the Hastings ratio in Holder et al. (2005) is no longer relevant. The reason we can get away with this is that we will next create a `TreeLengthUpdater` class that will handle scaling the tree up or down, so we do not need to do any rescaling in the `TreeUpdater` proposal.
~~~~~~
{{ "steps/step-17/src/tree_updater.hpp" | polcodesnippet:"begin_proposeNewState-end_proposeNewState","" }}
~~~~~~
{:.cpp}

## The revert member function

This is the last of the 4 pure virtual functions defined by the base class `Updater` that we must override. The `proposeNewState` function stored  the particular path (of 8 possible paths) that was followed in generating the proposed tree in the data member `_case`, so here we simply need to reconstruct the original tree from knowledge of the path followed and whether the topology was modified.
~~~~~~
{{ "steps/step-17/src/tree_updater.hpp" | polcodesnippet:"begin_revert-end_revert","" }}
~~~~~~
{:.cpp}

## Literature Cited

Holder, Mark T., P. O. Lewis, D. L. Swofford, and B. Larget. 2005. Hastings ratio of the LOCAL proposal used in Bayesian phylogenetics. Systematic Biology 54(6): 961-965. [DOI: 10.1080/10635150500354670](https://doi.org/10.1080/10635150500354670)

Larget, B., and Simon, D. 1999. Markov Chain Monte Carlo Algorithms for the Bayesian Analysis of Phylogenetic Trees. Molecular Biology and Evolution 16:750–759. [DOI: 10.1093/oxfordjournals.molbev.a026160](https://doi.org/10.1093/oxfordjournals.molbev.a026160)

