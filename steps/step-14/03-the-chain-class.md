---
layout: steps
title: The Chain Class
partnum: 14
subpart: 3
description: Create a class that encapsulates a Markov chain that can carry out MCMC
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}
The `Chain` class encapsulates a Markov chain. In this first draft, it contains a `GammaRateVarUpdater` object that modifies only one parameter, but in future versions the `Chain` class will include additional updaters that have the ability to update the other model parameters as well as the tree topology and edge lengths.

Create a file {% indexfile chain.hpp %} that contains the following code.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## Constructor and destructor

The constructor calls the `clear` function, which returns the `Chain` object to its just-constructed state. The destructor does nothing.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The clear member function

This function return the `Chain` object to its just-constructed state. It sets the chain index to 0 and the heating power to 1 (i.e. this is the "cold" chain by default) and calls `startTuning` to tell all updaters to begin automatic tuning.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_clear-end_clear","" }}
~~~~~~
{:.cpp}

## The startTuning and stopTuning member functions

Calling the `Chain` function `startTuning` calls, in turn, the `setTuning` function of all its component updaters, passing the value `true` to each. Similarly, the `stopTuning` function passes `false` to the `setTuning` function of all its component updaters.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_startTuning-end_stopTuning","" }}
~~~~~~
{:.cpp}

## The setTreeFromNewick member function

This function provides a copy of the tree in the form of a Newick tree description string to the `Chain`. If `_tree_manipulator` does not currently exist, then a `TreeManip` object is created to hold the tree built from the supplied string.

Our only current updater `GammaRateVarUpdater` does not modify tree topology or edge lengths, so it may be surprising that we call its `setTreeFromNewick` function here. The answer is that all updaters need to calculate the log likelihood (see `Chain::calcLogLikelihood` below), and a tree is required in order to calculate the likelihood.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_setTreeFromNewick-end_setTreeFromNewick","" }}
~~~~~~
{:.cpp}

## The createUpdaters member function

This function creates an updater for each independent model parameter that has not been fixed by the user and adds the updater to the `_updaters` vector. Each updater is provided with a copy of the `model`, the pseudorandom number generator (`lot`), and the `likelihood`. The updater's `setWeight` function is called to set the updater's `_weight` data member. The weight is used to determine the probability that an updater's `update` function will be called in any given iteration. The sum of weights of all updaters is used to normalize the weights (done by calling each updater's `calcProb` function).
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_createUpdaters-end_createUpdaters","" }}
~~~~~~
{:.cpp}

## The getTreeManip member function

This is an accessor for the `_tree_manipulator` data member.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_getTreeManip-end_getTreeManip","" }}
~~~~~~
{:.cpp}

## The getModel member function

`Chain` does not need to have a data member to store the model because the `Likelihood` object already provides housing for the model (and also provides a convenient `getModel` accessor function that we can use here).
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_getModel-end_getModel","" }}
~~~~~~
{:.cpp}

## The getLogLikelihood member function

This is an accessor for the `_log_likelihood` data member.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_getLogLikelihood-end_getLogLikelihood","" }}
~~~~~~
{:.cpp}

## The getHeatingPower and setHeatingPower member functions

The `setHeatingPower` function is responsible for setting the heating power for all updaters. Right now, there is only one updater (`GammaRateVarUpdater`) but more lines will be uncommented as other updaters are created. The `getHeatingPower` accessor function may be used to query a `Chain` object to find out its current heating power. When two chains swap, it is easier to simply exchange heating powers rather than swap states (which would involve copying lots of parameter values as well as the tree topology and edge lengths). As a result, the `setHeatingPower` function gets called often during an MCMC analysis involving multiple chains.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_getHeatingPower-end_setHeatingPower","" }}
~~~~~~
{:.cpp}

## The getChainIndex and setChainIndex member functions

The `setChainIndex` function is responsible for setting the index of the chain (stored in the data member `_chain_index`). The chain index will become important when we begin to use heated chains; when two chains swap places, it is easier to simply swap their indices and chain powers rather than copy the entire state (tree topology, edge lengths, state frequencies, etc.).
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_getChainIndex-end_setChainIndex","" }}
~~~~~~
{:.cpp}

## The findUpdaterByName member function

Loops through all updaters in the `_updaters` vector, returning the first one whose `getUpdaterName` function returns `name` (or `nullptr` if an updater named `name` is not found).
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_findUpdaterByName-end_findUpdaterByName","" }}
~~~~~~
{:.cpp}

## The getUpdaterNames member function

Part of the output generated will be a list of the tuning values for all updaters, so it is important to be able to get a list of the names of all updaters. This list may be obtained by calling the `getUpdaterNames` function.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_getUpdaterNames-end_getUpdaterNames","" }}
~~~~~~
{:.cpp}

## The getAcceptPercentages member function

It is important to provide feedback to the user regarding the performance of each updater. To that end, this function returns a vector of the acceptance percentages for all updaters.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_getAcceptPercentages-end_getAcceptPercentages","" }}
~~~~~~
{:.cpp}

## The getNumUpdates member function

This function reports the number of attempts to update each parameter. Along with `getAcceptPercentages`, this provides feedback to the user regarding the performance of each updater.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_getNumUpdates-end_getNumUpdates","" }}
~~~~~~
{:.cpp}

## The getLambdas and setLambdas member functions

The `getLambdas` function returns a list of the values of the tuning parameters for all updaters. The values are returned in the same order as the names returned by `getLambdaNames`. The `setLambdas` function sets the tuning parameter values for all updaters. Right now, we have only one updater (and thus one tuning parameter value), but the `setLambdas` function accepts a vector argument so that all values may be specified at once. Currently, only the first value in the vector `v` is used.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_getLambdas-end_setLambdas","" }}
~~~~~~
{:.cpp}

## The calcLogLikelihood member function

This function calculates the current log likelihood. Any updater can be used for this, as all have the capability of computing the likelihood. Here we call upon the `GammaRateVarUpdater` to help us out because it is currently the only updater we have.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_calcLogLikelihood-end_calcLogLikelihood","" }}
~~~~~~
{:.cpp}

## The calcLogJointPrior member function

Each updater (right now there is just one) is called upon to calculate the log prior for the model parameter that it is responsible for updating. The log priors for each parameter are added together to yield the log of the joint prior, which is returned.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_calcLogJointPrior-end_calcLogJointPrior","" }}
~~~~~~
{:.cpp}

## The start and stop member functions

The `start` and `stop` functions are called before the `Chain` object begins simulating a Markov chain and after it finishes, respectively. The `stop` function is simply a placeholder for now, but the `start` function is responsible for spitting out the starting state of the chain to the output.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_start-end_stop","" }}
~~~~~~
{:.cpp}

## The nextStep member function

Finally, we come to the `nextStep` function, whose job it is to take one step in the Markov chain. It calls the `update` function for each updater, then reports (every 100 iterations) the current state and saves parameter values (currently just the gamma rate variance) in the output every `sampling_freq` iterations.
~~~~~~
{{ "steps/step-14/src/chain.hpp" | polcodesnippet:"begin_nextStep-end_nextStep","" }}
~~~~~~
{:.cpp}
