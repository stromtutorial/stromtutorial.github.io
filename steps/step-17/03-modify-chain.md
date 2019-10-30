---
layout: steps
title: Modify the Chain Class
partnum: 17
subpart: 3
description: Add the TreeUpdater and TreeLengthUpdater to the Chain function createUpdaters. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

Our new updater `TreeUpdater` and `TreeLengthUpdater` classes must be added to the `Chain` class in order to update the tree topology and edge lengths during the course of an MCMC analysis.

The changes that need to be made to {% indexfile chain.hpp %} are in bold, blue text. They are very similar to the changes you made to the `Chain` class to add the other updaters.

First, include the header files for `TreeUpdater` and `TreeLengthUpdater` at the top of `chain.hpp`.
~~~~~~
{{ "steps/step-17/src/chain.hpp" | polcodesnippet:"start-end_headers","a-b" }}
~~~~~~
{:.cpp}

Now modify the `Chain::createUpdaters` function to add `TreeUpdater` and `TreeLengthUpdater` to the mix.
~~~~~~
{{ "steps/step-17/src/chain.hpp" | polcodesnippet:"begin_createUpdaters-end_createUpdaters","c-d,begin_tree_updaters-end_tree_updaters" }}
~~~~~~
{:.cpp}

As with the other updaters, we are hard-coding the prior for tree topology and edge lengths. In this case, the tree length prior is an Exponential distribution with mean 10, and the prior on edge length proportions is a flat Dirichlet distribution. The prior on tree topology is Discrete Uniform over the space of all possible tree topologies (and tree topologies are assumed to be unrooted and binary). You should know enough at this point to add options to the `Strom` class to allow the prior parameters to be set by the user.
