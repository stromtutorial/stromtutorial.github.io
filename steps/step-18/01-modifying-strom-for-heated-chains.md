---
layout: steps
title: Modify the Strom Class
partnum: 18
subpart: 1
description: Modifying the Strom class to implement multichain MCMC. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

All of the modifications needed to switch to a multichain version of our program are confined to the `Strom` class. The changes needed are relatively minor, but numerous, because each chain needs to have its own `Likelihood` and `Model` objects (otherwise all chains would update the exact same model parameter values).

## The Strom class declaration

Begin by making the indicated changes and additions in the `Strom` class declaration (in the file {% indexfile strom.hpp %}). Note that some highlighted functions are already in your file, but have been changed, so look carefully to find what's different!
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"start-end_class_declaration","a-b,c-d,e-f,g-h" }}
~~~~~~
{:.cpp}
The `processAssignmentString` and `handleAssignmentStrings` member functions now have an additional `Model::SharedPtr` parameter. The `_likelihood` data member has been replaced by a vector of `Likelihood` shared pointers named `_likelihoods`, and `_model` has been deleted because the model specific to a particular `Likelihood` object can always be obtained from that `Likelihood` object using the `getModel` member function. The other changes are all additions of member functions or data members.

## Modify the clear member function 

Initialize the new additions in the `clear` member function.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_clear-end_clear","i,j,k-l" }}
~~~~~~
{:.cpp}

## Add new program options

Add new program options for the number of chains to run simultaneously (`nchains`), the factor that determines how hot each heated chain is (`heatfactor`), the number of iterations to use for burn-in (`burnin`), and whether to explore the posterior (`usedata=yes`) or the prior (`usedata=no`).
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_processCommandLineOptions-end_processCommandLineOptions","m-n,o-p,q-r,s-t,begin_alloc_models-end_alloc_models" }}
~~~~~~
{:.cpp}
At the end of the `processCommandLineOptions` function, note that the section below
~~~~~~
{{ "steps/step-17/src/strom.hpp" | polcodesnippet:"begin_alloc_models-end_alloc_models","" }}
~~~~~~
{:.cpp}
has been replaced by 
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"!begin_alloc_models-!end_alloc_models","" }}
~~~~~~
{:.cpp}
We now must create a separate `Likelihood` object for every chain and set up the model contained by each of those `Likelihood` objects.

## Modify the handleAssignmentStrings member function

In addition to adding the `Model::SharedPtr m` parameter to the function, change the two lines indicated so that the model `m` is passed as the first parameter to the `processAssignmentString` function.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"!begin_handleAssignmentStrings-end_handleAssignmentStrings","begin_handleAssignmentStrings,u,v" }}
~~~~~~
{:.cpp}

## Modify the processAssignmentString member function

In addition to adding the `Model::SharedPtr m` parameter to the function, change all instances of `_model` to `m`, as indicated, because there is no longer a single model pointed to by `_model`.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"!begin_processAssignmentString-end_processAssignmentString","begin_processAssignmentString,w,x,y,z,aa,bb,cc,dd,ee,ff,gg,hh,ii,jj,kk,ll" }}
~~~~~~
{:.cpp}

## Replace the run member function 

Replace the entire `run` function body with the version below. A lot has changed in this function, so it makes more sense to just replace it rather than show individual modifications in highlighted text. The biggest change is that the creation of model and likelihood objects has been moved into the initChains function (because a `Model` object and its `Likelihood` wrapper must be created for each `Chain` separately). I've also created several small functions to handle showing various kinds of output so that `run` becomes more of an outline than a narrative.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_run-end_run","" }}
~~~~~~
{:.cpp}

## Add the readData member function

This function takes care of reading the data file whose name is stored in the data member `_data_file_name` (the value of this data member was set by the user via program options. The code in this function was previously in the body of the `run` function.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_readData-end_readData","" }}
~~~~~~
{:.cpp}

## Add the readTrees member function

This function takes care of reading the tree file whose name is stored in the data member `_tree_file_name` (the value of this data member was set by the user via program options). This function also checks to ensure that the tree has the same number of taxa as the data file and throws an exception the number of taxa differ. The code in this function was previously in the body of the `run` function.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_readTrees-end_readTrees","" }}
~~~~~~
{:.cpp}

## Add the showPartitionInfo member function
This function summarizes the data partition information. The code in this function was previously in the body of the `run` function.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_showPartitionInfo-end_showPartitionInfo","" }}
~~~~~~
{:.cpp}

## Add the showBeagleInfo member function
This function shows what BeagleLib resources are available. The code in this function was previously in the body of the `run` function.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_showBeagleInfo-end_showBeagleInfo","" }}
~~~~~~
{:.cpp}

## Add the calcHeatingPowers member function

This is a new function called within another new function, `initChains` (described below). Its job is to compute the heating power of each chain using the chain index and the `heatfactor` (data member `_heating_lambda`) specified by the user on the command line. The chain with index 0 is the cold chain and has power 1.0 (i.e. it explores the posterior distribution), while chains with indices greater than 0 explore distributions that are more gentle (valleys higher, peaks lower) due to the fact that the posterior density is raised to a power between 0.0 and 1.0 (determined by this function).
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_calcHeatingPowers-end_calcHeatingPowers","" }}
~~~~~~
{:.cpp}

## Add the initChains member function

This new function equips each chain with an index, starting tree, random number generator, model, likelihood calculator, and heating power.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_initChains-end_initChains","" }}
~~~~~~
{:.cpp}

## Add the showMCMCInfo member function
This function spits out information about the MCMC analysis. It is called just before the burnin iteration loop begins. If data is being ignored, it says "Exploring prior"; otherwise, it computes and displays the starting likelihood, arbitrarily using the first `Likelihood` object in the `_likelihoods` vector (all `Likelihood` objects are identical at this point except for the BeagleLib instance assigned to them).
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_showMCMCInfo-end_showMCMCInfo","" }}
~~~~~~
{:.cpp}

## Add the startTuningChains and stopTuningChains member functions

These functions tell each chain to start and stop autotuning, respectively. The `startTuningChains` function should be called prior to burn-in, and the `stopTuningChains` should be called after burn-in so that tuning parameters are not modified during the sampling run.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_startTuningChains-end_stopTuningChains","" }}
~~~~~~
{:.cpp}

## Add the stepChains member function

This function loops through chains, telling each one to take one step. Each step involves calling the `update` method of each updater. The `sample` function is also called if sampling is true. Note that just because `sample` is called does not necessarily mean that a line is added to the tree and parameter file; that depends on the iteration and degree of thinning specified by the user. Also, sample does nothing if the chain `c` passed to it does not currently represent the cold chain. Remember that chain 0 started out as the cold chain but cannot any longer be assumed to be the cold chain because the cold chain may have swapped with one of the heated chains in the interim.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_stepChains-end_stepChains","" }}
~~~~~~
{:.cpp}

## Add the swapChains member function

The `swapChains` function selects two chains at random and asks them to attempt a swap. The swap is successful if a uniform random deviate is less than or equal to the coupled acceptance probability. Although it is not quite this simple, essentially both chains have to be able to move to where the other chain is located in order for the proposed swap to be accepted. If a swap is accepted, then the chain objects swap indices, tuning parameters, and heating power. It is much simpler to swap these three simple quantities than it would be to swap the entire state (tree topology, all edge lengths, and the current value of all substitution model parameters). The tuning parameters need to be swapped because the chains are exploring different landscapes, and a chain tuned for one landscape would not mix well if it suddenly found itself using a tuning parameter adjusted for a different landscape.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_swapChains-end_swapChains","" }}
~~~~~~
{:.cpp}

## Add the showChainTuningInfo member function

This function displays the tuning parameter values for each defined updater. Because autotuning is performed during burn-in, these will differ from the starting values defined in `Chain::createUpdaters` after burn-in is completed. This function also dispays the acceptance percentage for each updater, so it makes sense to call this function after the sampling run is finished in order to see the acceptance rates that were achieved during sampling.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_showChainTuningInfo-end_showChainTuningInfo","" }}
~~~~~~
{:.cpp}

## Add the stopChains member function

This function loops through all chains and calls the `stop` function for each.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_stopChains-end_stopChains","" }}
~~~~~~
{:.cpp}

## Add the swapSummary member function

Each time two chains attempt a swap, the event is recorded in the `_swaps` data member, which is a vector that is used to represent a 2-dimensional matrix in which the upper triangle is used to store the number of attempted swaps between each pair of chains and the bottom triangle is used to store the number of successful swaps between each pair. This function simply displays the `_swaps` vector in the form of a table.
~~~~~~
{{ "steps/step-18/src/strom.hpp" | polcodesnippet:"begin_swapSummary-end_swapSummary","" }}
~~~~~~
{:.cpp}

