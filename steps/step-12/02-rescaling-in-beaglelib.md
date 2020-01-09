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

In the `Likelihood::newInstance` function, add `BEAGLE_FLAG_SCALING_MANUAL` and `BEAGLE_FLAG_SCALERS_LOG` to the preference flags and tell BeagleLib to create `num_internals + 1` scaling buffers (i.e. arrays). Each scaling buffer has one element for each pattern in the data, plus an extra element to store the cumulative scaling factors. If scaling is done at each internal node, there needs to be at least the same number of scaling buffers as there are internal nodes. The items to add or change are highlighted in blue.
~~~~~~
{{ "steps/step-12/src/likelihood.hpp" | polcodesnippet:"begin_newInstance-end_newInstance","c-cc,d,dd" }}
~~~~~~
{:.cpp}

## Modify getScalerIndex

Modify the `Likelihood::getScalerIndex` function to tell BeagleLib which scaling buffer to use when computing the partials for a given internal node. We will reserve the first scaling buffer element to hold the cumulative sum of log scalers for each pattern, and because internal nodes are numbered starting with `_ntaxa`, we need to subtract `_ntaxa` from the internal node number and add 1 to get the index of the scaler buffer to use.
~~~~~~
{{ "steps/step-12/src/likelihood.hpp" | polcodesnippet:"begin_getScalerIndex-end_getScalerIndex","e-ee" }}
~~~~~~
{:.cpp}

## Accumulate scalers in calcInstanceLogLikelihood

We've now provided BeagleLib with the appropriate number of scaling buffers, and, when operations are added to recalculate partials in `addOperation`, scaling will be done because `getScalerIndex` will return the appropriate scaling buffer index rather than BEAGLE_OP_NONE. All that is left is to harvest the scalers at each internal node and accumulate them for the final log-likelihood calculation.

The large section in blue in the code below first creates a list (`internal_node_scaler_indices`) of all scaling buffer indices used (which is just the scaler index for every internal node). 

If the instance is managing just one data subset (unpartitioned case), `beagleResetScaleFactors` is used to zero out all elements of the `cumulative_scale_index`, which is the first (0th) scaler buffer and `beagleAccumulateScaleFactors` is used to sum up the log scalers (using scaler indices supplied via `internal_node_scaler_indices`).

If the instance is managing more than one data subset (partitioned case), `beagleResetScaleFactorsByPartition` is used to zero out all elements of the `cumulative_scale_index`, which is the first (0th) scaler buffer and `beagleAccumulateScaleFactorsByPartition` is used to sum up the log scalers (using scaler indices supplied via `internal_node_scaler_indices`). 

Because we are using just one cumulative scaling buffer for all site patterns regardless of their subset of origin, it is really not necessary to use the ByPartition versions of these functions, so you might save a tiny bit of computational effort by using the simpler versions for the partitioned case. The ByPartition versions simply limit their activity (in the cumulative scaler buffer) to patterns from subset `s`, whereas `beagleResetScaleFactors` and `beagleResetScaleFactors` affect the entire cumulative scaler buffer.

The other highlighted lines simply specify that the scaler buffer with index 0 is where the cumulative scaling factors are stored when underflow scaling is being done.
~~~~~~
{{ "steps/step-12/src/likelihood.hpp" | polcodesnippet:"begin_calcInstanceLogLikelihood-dots","g,h-hh,i" }}
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


