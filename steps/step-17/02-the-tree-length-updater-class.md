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

The `TreeLengthUpdater` class encapsulates an `Updater` that can modify the length of all edges of the tree simultaneously, effectively rescaling the tree to make it larger or smaller. This updater is needed for good mixing because the only other updater that changes edge lengths (`TreeUpdater`) only changes 3 edges at a time.

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

This overrides the base class version and calls the base class function `calcEdgeLengthPrior` to do the work. Edge lengths are the only parameters changed when this updater is invoked, so the only component of the prior that needs to be calculated is the joint edge length prior.
~~~~~~
{{ "steps/step-17/src/tree_length_updater.hpp" | polcodesnippet:"begin_calcLogPrior-end_calcLogPrior","" }}
~~~~~~
{:.cpp}

## The pullFromModel and pushToModel member functions

The `pullFromModel` function stores the current tree length in its `_curr_point` data member. The value of `_curr_point` changes when `proposeNewState` is called (after a copy is stored in `_prev_point`), and the function `pushToModel` rescales the tree based on the ratio of the new tree length (`_curr_point`) and the previous tree length (`_prev_point`).
~~~~~~
{{ "steps/step-17/src/tree_length_updater.hpp" | polcodesnippet:"begin_pullFromModel-end_pushToModel","" }}
~~~~~~
{:.cpp}

## The proposeNewState member function

This function proposes a new tree length by multiplying the old tree length by a factor that is either slightly greater than, or slightly less than, 1.0. with the magnitude of the factor determined by the `Updater` data member `_lambda`. This is the same technique we used in previous steps tp update omega and the rate variance.
~~~~~~
{{ "steps/step-17/src/tree_length_updater.hpp" | polcodesnippet:"begin_proposeNewState-end_proposeNewState","" }}
~~~~~~
{:.cpp}

## The revert member function

This function simply copies the old tree length (stored in `_prev_point`) into `_curr_point` so that when `pushToModel` is called no change will take place to edge lengths because the proposal was rejected.
~~~~~~
{{ "steps/step-17/src/tree_length_updater.hpp" | polcodesnippet:"begin_revert-end_revert","" }}
~~~~~~
{:.cpp}
