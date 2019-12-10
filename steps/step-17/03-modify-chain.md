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
{{ "steps/step-17/src/chain.hpp" | polcodesnippet:"begin_createUpdaters-end_createUpdaters","c-d,e-f,g-h,i-j" }}
~~~~~~
{:.cpp}

As with the other updaters, we are hard-coding the prior for tree topology and edge lengths. We will add options in Step 19 so that the user can change these values, but for now we are keeping things simple and baking in the priors. In this case, the tree length prior is an Exponential distribution with mean 10, and the prior on edge length proportions is a flat Dirichlet distribution. The prior on tree topology is Discrete Uniform over the space of all possible tree topologies (and tree topologies are assumed to be unrooted and binary). 

I mentioned (when creating the `TreeLengthUpdater` class) that we would need to be careful to avoid counting the tree length prior and edge length proportions prior twice. We will now take care of that by simply adding a line to the `calcLogJointPrior` function that causes the `TreeLengthUpdater` to be skipped when computing the joint prior.
~~~~~~
{{ "steps/step-17/src/chain.hpp" | polcodesnippet:"begin_calcLogJointPrior-end_calcLogJointPrior","k" }}
~~~~~~
{:.cpp}
