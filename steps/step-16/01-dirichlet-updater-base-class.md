---
layout: steps
title: The Dirichlet Updater Base Class
partnum: 16
subpart: 1
description: Create a base class for StateFreqUpdater, ExchangeabilityUpdater, and SubsetRelRateUpdater. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

Create a file named {% indexfile dirichlet_updater.hpp %} and fill it with the following class declaration. Note that the `DirichletUpdater` class is derived from the class `Updater` and overrides 2 of pure virtual functions specified in `Updater` (namely `proposeNewState` and `revert`).
~~~~~~
{{ "steps/step-16/src/dirichlet_updater.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## Constructor and destructor

As usual, the constructor just calls the `clear` function to do its work and the destructor is a placeholder but does nothing currently.
~~~~~~
{{ "steps/step-16/src/dirichlet_updater.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The clear member function

This function returns the object to its just-constructed state (and is what actually does the work of the constructor). Note that this class is derived from `Updater` and first calls the `Updater::clear` function before doing additional work specific to this class.
~~~~~~
{{ "steps/step-16/src/dirichlet_updater.hpp" | polcodesnippet:"begin_clear-end_clear","" }}
~~~~~~
{:.cpp}

## The calcLogPrior member function

This function assumes that the `_prior_parameters` vector in the `Updater` base class has been filled with the appropriate number of Dirichlet prior parameters. The assumption is that if `DirichletUpdater` is being used to update a multivariate model parameter, that parameter must have a Dirichlet prior and thus the length of a vector representing the current value of the parameter (`_curr_point`) should have the same length as the vector `_prior_parameters`. Because all parameters governed by `DirichletUpdater` have a Dirichlet prior (or at least a transformed Dirichlet prior), this class can handle calculation of most or all of the log prior, thus relieving derived classes of the need to do this job.
~~~~~~
{{ "steps/step-16/src/dirichlet_updater.hpp" | polcodesnippet:"begin_calcLogPrior-end_calcLogPrior","" }}
~~~~~~
{:.cpp}

## The pullFromModel and pushToModel pure virtual member functions

From the class declaration:
~~~~~~
{{ "steps/step-16/src/dirichlet_updater.hpp" | polcodesnippet:"pullFromModel-pushToModel","" }}
~~~~~~
{:.cpp}

These are placeholders for functions that must be defined in derived classes. Each derived class must provide a way to fill the `_curr_point` vector with values that have a Dirichlet prior and are updated using the focussed Dirichlet proposal implemented in this abstract base class. For example, the `StateFreqUpdater` will simply copy the state frequencies stored in the model to `_curr_point` in its `pullFromModel` function, and copy the values in `_curr_point` to the model in its `pushToModel` function.

## The proposeNewState member function

This updater works by centering a sharp (low variance) Dirichlet distribution over the current value of the parameter (`_curr_point`), then choosing the proposed value from that Dirichlet distribution. Note that the tuning parameter `_lambda` controls the sharpness of the proposal distribution: larger values of `_lambda` (e.g. 1) mean bolder proposals that generate proposed states further away from the current state on average, while smaller `_lambda` values (e.g. 1/1000) result in a sharper proposal distribution that chooses proposed values close to the current state.

Yes, it is a little confusing that we are using distinct Dirichlet distributions for the prior and the proposal distribution, but the advantage of this is that we modify all 4 nucleotide frequencies (or 6 exchangeabilities, or 61 codon frequencies, or all subset relative rates) at once but in a way that keeps the proposed values close to the current values. This proposal approach also automatically ensures that the proposed values add to 1, thus maintaining the constraint that is required.

One additional complication is that this proposal is not symmetric, so we must calculate the Hastings ratio in this function as well as proposing a new state. The Hastings ratio is the conditional probability density of the current state given the proposed state divided by the conditional probability density of the proposed state given the current state. Said more simply, but less precisely, it is the ratio of the probability of the reverse proposal to the probability of the forward proposal.

What is the reverse proposal? It involves proposing the current state assuming that the Markov chain is currently sitting at the proposed state. Calculation of the Hastings ratio thus involves centering a sharp Dirichlet distribution over the proposed state and asking about the probability density of the current state were the current state to be drawn from that distribution.

The comments attempt to explain which part of the process is being done by each chunk of code.
~~~~~~
{{ "steps/step-16/src/dirichlet_updater.hpp" | polcodesnippet:"begin_proposeNewState-end_proposeNewState","" }}
~~~~~~
{:.cpp}

## The revert member function

This function is called if the proposal is not accepted. It simply copies the `_prev_point` vector into the `_curr_point` vector. (The `_prev_point` vector was filled with the values from `_curr_point` at the beginning of the `proposeNewState` function before `_curr_point` was modified.)
~~~~~~
{{ "steps/step-16/src/dirichlet_updater.hpp" | polcodesnippet:"begin_revert-end_revert","" }}
~~~~~~
{:.cpp}
