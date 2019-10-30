---
layout: steps
title: The StateFreqUpdater Class
partnum: 16
subpart: 2
description: Create the derived class StateFreqUpdater from the base class DirichletUpdater. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

This class is derived from `DirichletUpdater`, which is, in turn, derived from `Updater`. It thus inherits everything that both `Updater` and `DirichletUpdater` can do. Remember that `DirichletUpdater` filled in some of the functionality that `Updater` specified but didn’t implement, namely the overrides of the pure virtual functions `proposeNewState` and `revert`. All we must do in this class is fill in blanks specific to the parameter being updated: `DirichletUpdater` does everything that is generic to any parameter with a Dirichlet prior distribution.

Create a new header file named {% indexfile statefreq_updater.hpp %} and fill it with the following.
~~~~~~
{{ "steps/step-16/src/statefreq_updater.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## Constructor and destructor

The constructor first calls the `clear` function defined by the `DirichletUpdater` base class. It then sets its `_name` to “State Frequencies” and its `_qmatrix` data member to the shared pointer `qmatrix` supplied as an argument. This points to the `QMatrix` object that manages the state frequencies that this class will update. The destructor is merely a placeholder, as usual.
~~~~~~
{{ "steps/step-16/src/statefreq_updater.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The pullFromModel and pushToModel member functions

The `pullFromModel` function first obtains a shared pointer to the state frequencies vector of the `QMatrix` object pointed to by `_qmatrix`. It then copies those state frequencies into the `_curr_point` vector. The `pushToModel` function copies the values from `_curr_point` to the model via the `setStateFreqs` member function of `QMatrix`, which handles recalculation of the instantaneous rate matrix and the corresponding eigen decomposition using the new state frequencies.
~~~~~~
{{ "steps/step-16/src/statefreq_updater.hpp" | polcodesnippet:"begin_pullFromModel-end_pushToModel","" }}
~~~~~~
{:.cpp}

## That's it!

The `DirichletUpdater` base class does almost all the work, so only a small amount of work in the constructor and the `pullFromModel` and `pushToModel` functions was needed to implement a state frequency updater.
