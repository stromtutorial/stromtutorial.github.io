---
layout: steps
title: Adding PolytomyUpdater to the Chain class
partnum: 19
subpart: 7
description: Deploy PolytomyUpdater.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

Now that we've created the `PolytomyUpdater` class, we need to insert it into the updater rotation. Start by including the {% indexfile polytomy_updater.hpp %} header near the top of the {% indexfile chain.hpp %}.
~~~~~~
{{ "steps/step-19/src/chain.hpp" | polcodesnippet:"start-end_includes","a" }}
~~~~~~
{:.cpp}

## Modifying the setTreeFromNewick function

In the `setTreeFromNewick` function, make the last argument to `buildFromNewick` equal to true so that polytomies are allowed when creating (for example) the starting tree.
~~~~~~
{{ "steps/step-19/src/chain.hpp" | polcodesnippet:"begin_setTreeFromNewick-end_setTreeFromNewick","b" }}
~~~~~~
{:.cpp}

## Modifying the createUpdaters function

Add the highlighted lines to the `createUpdaters` function to add `PolytomyUpdater` to the list of updaters potentially called during an MCMC iteration.
~~~~~~
{{ "steps/step-19/src/chain.hpp" | polcodesnippet:"begin_createUpdaters-aa,begin_tree_updaters-end_createUpdaters","x,c-d,e,f-g,h" }}
~~~~~~
{:.cpp}

## The calcLogJointPrior function

We must not let either the `TreeLengthUpdator` nor the `PolytomyUpdater` contribute to the calculation of the joint prior because `TreeUpdater` is already handling the tree topology and edge length components of the joint prior.
~~~~~~
{{ "steps/step-19/src/chain.hpp" | polcodesnippet:"begin_calcLogJointPrior-end_calcLogJointPrior","i" }}
~~~~~~
{:.cpp}

