---
layout: steps
title: Rescaling in BeagleLib
partnum: 12
subpart: 2
description: Set up rescaling in BeagleLib to deal with underflow.
---
To add rescaling, we will need to modify several BeagleLib function calls inside our `Likelihood` class.

## Add a data member to control whether scaling occurs
Add new data member `_underflow_scaling` (and a new public member function (`useUnderflowScaling`) to set its value) to the class declaration .
~~~~~~
{{ "steps/step-12/src/likelihood.hpp" | polcodesnippet:"begin_classdeclaration-end_classdeclaration","a,aa" }}
~~~~~~
{:.cpp}

Set the new data member to `false` in the clear function. We will turn scaling on or off using a program option, but the default will be to not scale.
~~~~~~
{{ "steps/step-12/src/likelihood.hpp" | polcodesnippet:"begin_clear-end_clear","b" }}
~~~~~~
{:.cpp}

Add the body of the new member function somewhere after the class declaration but before the namespace-closing right curly bracket.
~~~~~~
{{ "steps/step-12/src/likelihood.hpp" | polcodesnippet:"begin_useUnderflowScaling-end_useUnderflowScaling","" }}
~~~~~~
{:.cpp}

## Tell BeagleLib to create scale buffers

In the `Likelihood::newInstance` function, add `BEAGLE_FLAG_SCALING_MANUAL` to the requirement flags and tell BeagleLib to create `num_internals + num_subsets` scaling buffers (i.e. arrays). Each scaling buffer has one element for each pattern in the data, plus an extra element for each data subset to store the cumulative scaling factors. If scaling is done at each internal node, there needs to be at least the same number of scaling buffers as there are internal nodes. The items to add or change are highlighted in blue.
~~~~~~
{{ "steps/step-12/src/likelihood.hpp" | polcodesnippet:"begin_newInstance-end_newInstance","c-cc,d" }}
~~~~~~
{:.cpp}

## Add the scale buffer index to each operation

Modify the `Likelihood::addOperation` function to tell BeagleLib which scaling buffer to use when computing the partials for a given internal node. We will reserve the first `num_subsets` scaling buffer elements to hold the cumulative sums of log scalers for each subset, and because internal nodes are numbered starting with `_ntaxa`, we need to subtract `_ntaxa` and add `num_subsets` from the internal node number to get the index of the scaler buffer to use.

Elements 8 and 9 of each operation are only present if there is more than one subset, and element 9 determines which scale buffer element to use for the cumulative scaling factor. The variable `subset_index` keeps track of the _relative_ subset index, which is what is needed here. The absolute subset index is not helpful, as it is always possible that some data subsets are being handled by other BeagleLib instances.
~~~~~~
{{ "steps/step-12/src/likelihood.hpp" | polcodesnippet:"begin_addOperation-end_addOperation","eee,e-ee,f-ff" }}
~~~~~~
{:.cpp}
In our application, we need only specify scaling buffers for writing, not reading, so the 3rd element of each operation should be left as `BEAGLE_OP_NONE`.

## Specify the cumulative scale buffer index in calculatePartials

The new code in blue below serves to initialize the scaling buffer element for each pattern to 0.0 before recalculating partial likelihood arrays. 

In the case of more than one subset, the cumulative scaling factor index is supplied as the final argument to `beagleResetScaleFactorsByPartition` (just the instance-relative subset index). The second argument is the relative subset index, which is the same numerical value. Note that `beagleUpdatePartialsByPartition` does not need to know this information because it is supplied as element 9 of each operation.

In the case of an instance containing a single subset, the cumulative scaler index is simply 0, supplied as the last argument to `beagleUpdatePartials`.
~~~~~~
{{ "steps/step-12/src/likelihood.hpp" | polcodesnippet:"begin_calculatePartials-end_calculatePartials","g-gg,h-hh,i" }}
~~~~~~
{:.cpp}

## Telling BeagleCalculateEdgeLogLikelihoods the cumulative scale buffer index

Finally, in the `calcInstanceLogLikelihood` member function, we need to provide the index of the scalar array holding cumulative sums of log scaling factors to the `beagleCalculateEdgeLogLikelihoods` (or `beagleCalculateEdgeLogLikelihoodsByPartition`) function, which is called in our `Likelihood::calcInstanceLogLikelihood` function. 

First, change `cumulativeScalingIndex` from `BEAGLE_OP_NONE` to 0 (this will be used in the single-subset case), and modify the `_scaling_indices` vector so that it provides the correct index to the cumulative scaling element for each subset (used only in the multi-subset case).
~~~~~~
{{ "steps/step-12/src/likelihood.hpp" | polcodesnippet:"begin_calcInstanceLogLikelihood-dots","j,k" }}
~~~~~~
{:.cpp}

## Modify Strom to add program option for scaling
Add a new data member to the `Strom` class named `_use_underflow_scaling`:
~~~~~~
{{ "steps/step-12/src/strom.hpp" | polcodesnippet:"begin_classdeclaration-end_classdeclaration","a" }}
~~~~~~
{:.cpp}

Initialize it to `false` in the `clear` function:
~~~~~~
{{ "steps/step-12/src/strom.hpp" | polcodesnippet:"begin_clear-end_clear","b" }}
~~~~~~
{:.cpp}

Add a program option to set it in `processCommandLineOptions`:
~~~~~~
{{ "steps/step-12/src/strom.hpp" | polcodesnippet:"begin_processCommandLineOptions-dots","c" }}
~~~~~~
{:.cpp}

Finally, call the function `useUnderflowScaling` to tell the `Likelihood` object whether the user wishes to use scaling:
~~~~~~
{{ "steps/step-12/src/strom.hpp" | polcodesnippet:"begin_run-end_run","d" }}
~~~~~~
{:.cpp}

## Run the program again
If you now run your program using the {% indexfile strom.conf %} file below (which sets `underflowscaling` to `yes` to turn on scaling), you should find that it computes the log-likelihood correctly.
~~~~~~
{{ "steps/step-12/test/strom.conf" | polcodesnippet:"","a" }}
~~~~~~
{:.bash-output}

## Tradeoffs

Adding rescaling has added an additional computational burden to our likelihood calculation. You can lessen the burden by not rescaling at every internal node. All that is needed to avoid rescaling for a particular internal node is to modify the operation association with that internal node, specifying `BEAGLE_OP_NONE` instead of the node number for the 3rd element of the operation. Skimping too much, however, runs the risk of overflow. For now, we will rescale at every internal node (if the user specifies that scaling is to be done) just to be safe.


