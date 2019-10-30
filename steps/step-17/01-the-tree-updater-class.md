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

The `TreeUpdater` class updates three edge lengths and the tree topology using the Larget and Simon (1999) LOCAL move for unrooted trees.

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

Recall that the `reset` function is called at the end of the `Updater::update` function to prepare the updater for the next update. Any temporary variables needed by an updater during the update process need to be re-initialized in this function, and this `TreeUpdater` has many of these temporary variables, most of which are needed to keep track of how to revert if the proposed tree is rejected.
~~~~~~
{{ "steps/step-17/src/tree_updater.hpp" | polcodesnippet:"begin_reset-end_reset","" }}
~~~~~~
{:.cpp}

## The calcLogTopologyPrior member function

The tree topology prior is a discrete uniform prior distribution over all possible unrooted tree topologies. If there are _N_ distinct unrooted tree topologies, then the prior probability of each of these topologies is 1/_N_. The number _N_ can be obtained as follows:
~~~~~~
N = (2*n-5)!/[2^(n-3) (n-3)!]
~~~~~~
{:.bash-output}
where _n_ equals the number of taxa. For rooted trees, the same formula may be used if _n_+1 is substituted for _n_. This function calculates and returns the logarithm of 1/_N_, namely -log(_N_). We make use of the {% indexbold lgamma function %}, which returns the log of the gamma function (not to be confused with the Gamma distribution). Note that `lgamma(n+1)` returns the log of `n!` (`n` factorial).
~~~~~~
{{ "steps/step-17/src/tree_updater.hpp" | polcodesnippet:"begin_calcLogTopologyPrior-end_calcLogTopologyPrior","" }}
~~~~~~
{:.cpp}

## The chooseRandomChild member function

The chooseRandomChild member function is used by `proposeNewState` (see below) to choose nodes to complete the random 3-edge focal segment of the tree updated by the Larget-Simon proposal. If `avoid` is specified, then that node will not be returned as the chosen node. If `parent_included` is `true`, then the parent of the specified node will be considered one of the possible children.
~~~~~~
{{ "steps/step-17/src/tree_updater.hpp" | polcodesnippet:"begin_chooseRandomChild-end_chooseRandomChild","" }}
~~~~~~
{:.cpp}

## The calcLogPrior member function

Now that we’ve created the `calcLogTopologyPrior` function and (previously, in the `Updater` class) the `calcEdgeLengthPrior` function, the `calcLogPrior` function simply needs to call those two functions and return the sum of the values they calculate.
~~~~~~
{{ "steps/step-17/src/tree_updater.hpp" | polcodesnippet:"begin_calcLogPrior-end_calcLogPrior","" }}
~~~~~~
{:.cpp}

## The proposeNewState member function

This function, which overrides one of the 4 pure virtual functions defined by the base class `Updater`, is unfortunately rather long and tedious owing to the fact that the first step in the Larget-Simon LOCAL move, which involves choosing 3 adjacent random internal edges, leads to 8 distinct possible proposed states. I’ve tried to make following the code in this function easier by creating diagrams in the comments showing the relationships among all the temporary variables, but to fully understand this function, you will need to read and understand the description of this move in the papers by Larget and Simon (1999) and the note about calculating the Hastings ratio in Holder et al. (2005).
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

