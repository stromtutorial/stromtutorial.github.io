---
layout: steps
title: The TreeLengthUpdater class
partnum: 17
subpart: 2
description: Create an updater that scales all edge lengths simultaneously (i.e. updates the tree length). 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

The `TreeLengthUpdater` class encapsulates an `Updater` that can modify the length of all edges of the tree simultaneously, effectively rescaling the tree to make it larger or smaller. This updater is needed for good mixing because the only other updater that changes edge lengths (`TreeUpdater`) only changes edge length proportions, not edge lengths.

Create a new file named {% indexfile tree_length_updater.hpp %} and fill it with the following class declaration.
~~~~~~
{{ "steps/step-17/src/tree_length_updater.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## The constructor and destructor

The constructor calls clear to do all the work and then sets its name to "Tree Length".
~~~~~~
{{ "steps/step-17/src/tree_length_updater.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The clear member function

This function returns this `TreeLengthUpdater` object to its just-constructed state.
~~~~~~
{{ "steps/step-17/src/tree_length_updater.hpp" | polcodesnippet:"begin_clear-end_clear","" }}
~~~~~~
{:.cpp}

## The calcLogPrior member function

This overrides the base class version and calls the base class function `calcEdgeLengthPrior` to do the work. Edge lengths are the only parameters changed when this updater is invoked, so the only component of the prior that needs to be calculated is the joint prior on tree length and edge length proportions. We will need to be careful, however, because `TreeUpdater` returns a joint prior for tree topology, tree length, and edge length proportions, and if both updaters' `calcLogPrior` function is called in the process of generating the overall prior, we will end up including the tree lengths and edge length proportions twice. We will handle this in a crude way now (by simply excluding one of these updaters when generating the prior), but will be forced to create a more sophisticated system in Step 19 when we add yet another updater that modifies tree topology and edge length proportions.
~~~~~~
{{ "steps/step-17/src/tree_length_updater.hpp" | polcodesnippet:"begin_calcLogPrior-end_calcLogPrior","" }}
~~~~~~
{:.cpp}

## The pullFromModel and pushToModel member functions

The `pullFromModel` function stores the current tree length in its `_curr_point` data member. The value of `_curr_point` changes when `proposeNewState` is called (after a copy is stored in `_prev_point`), and the function `pushToModel` rescales the tree based on the ratio of the new tree length (`_curr_point`) to the previous tree length (`_prev_point`).
~~~~~~
{{ "steps/step-17/src/tree_length_updater.hpp" | polcodesnippet:"begin_pullFromModel-end_pushToModel","" }}
~~~~~~
{:.cpp}

## The proposeNewState member function

This function proposes a new tree length by multiplying the old tree length by a factor that is either slightly greater than, or slightly less than, 1.0. with the magnitude of the factor determined by the `Updater` data member `_lambda`. This is the same technique we used in previous steps tp update omega and the rate variance. The calculation of the Hastings ratio for a proposal like this is discussed in Holder et al. (2005), but note that the Hastings ratio calculated there was m^3 because three separate edge length parameters were being modified. Here, there is only one parameter (the tree length) that is being modified, so the Hastings ratio is, in this case, simply m^1.
~~~~~~
{{ "steps/step-17/src/tree_length_updater.hpp" | polcodesnippet:"begin_proposeNewState-end_proposeNewState","" }}
~~~~~~
{:.cpp}

## The revert member function

This function swaps `_prev_point` with `_curr_point` so that, when `pushToModel` is called, the tree length will revert to the value it had before the change was proposed.
~~~~~~
{{ "steps/step-17/src/tree_length_updater.hpp" | polcodesnippet:"begin_revert-end_revert","" }}
~~~~~~
{:.cpp}
