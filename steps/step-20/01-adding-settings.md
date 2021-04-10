---
layout: steps
title: Adding settings for steppingstone
partnum: 20
subpart: 1
description: Modifying the Strom class to add settings for steppingstone.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

We will need to add a function (`calcMarginalLikelihood`) and a couple of variables (`_steppingstone` and `_ss_alpha`) to {% indexfile strom.hpp %} to hold settings that allow users to specify that they would like to carry out a steppingstone analysis rather than an ordinary MCMC analysis. 
~~~~~~
{{ "steps/step-20/src/strom.hpp" | polcodesnippet:"begin_class_declaration-end_class_declaration","a,b-c" }}
~~~~~~
{:.cpp}

Initialize the new variables in the `clear` function:
~~~~~~
{{ "steps/step-20/src/strom.hpp" | polcodesnippet:"begin_clear-end_clear","clear_steppingstone-clear_ss_alpha" }}
~~~~~~
{:.cpp}

#### Adding program options

Create program options that the user can access in the `processCommandLineOptions` function:
~~~~~~
{{ "steps/step-20/src/strom.hpp" | polcodesnippet:"begin_processCommandLineOptions-end_add_options","processCommandLineOptions_steppingstone-processCommandLineOptions_ss_alpha" }}
~~~~~~
{:.cpp}

#### Modifying the sample function

Add a conditional to the sample function to account for the fact that the chains are doing something quite differently under steppingstone than under normal MCMC sampling:
~~~~~~
{{ "steps/step-20/src/strom.hpp" | polcodesnippet:"begin_sample-end_sample","sample_start-sample_end,sample_trailer" }}
~~~~~~
{:.cpp}
You will note that the steppingstone section is almost the same as the non-steppingstone section, but there are some key differences. Ordinarily (i.e. when we are not doing steppingstone), we only produce a progress report for the cold chain. That explains why we return from the function early if the heating power is less than 1. Under steppingstone, it is arbitrary which chain we use when reporting progress, so I've chosen here to report progress for the chain with the largest power, which is the final entry in the `_heating_powers` vector.

You'll notice that, in the steppingstone section, I've also left out code to save parameter values to the sampled parameters and sampled trees files. Samples from these chains have no use except to estimate the marginal likelihood.

Finally, at the beginning of the steppingstone section, a new function storeLogLikelihood is called for each chain if it is time to sample. This function, as you'll see when you create it, just stores the current log-likelihood in a vector named `_ss_loglikes`, which is a data member of the `Chain` class.

#### Modifying the calcHeatingPowers function

In the `calcHeatingPowers` function, we need to setup the chain powers differently for steppingstone than for ordinary Metropolis-coupled MCMC. In MCMCMC, it is not wise to let the chain powers get too small. For example, if the chain power were set to zero, the chain would explore the prior and would almost never serve as a useful scout for the cold chain. On the other hand, the chain powers used in steppingstone are concentrated toward the prior end of the spectrum. The reason for this is best explained graphically. Returning to our coin-flipping example, compare the power posterior distributions for evenly-spaced powers (left) to the power posterior distributions for powers concentrated toward the prior end of the spectrum (right).
![Comparison of even versus bunched steppingstone heating powers]({{ "/assets/img/steppingstone-spacing.png" | relative_url }}){:.center-math-image}
As you can see, evenly-spaced powers do not result in homogeneous ratios between adjacent distributions. It is as if the steppingstones placed to help cross a stream involved a really big first jump and then really tiny jumps thereafter. Bunching powers close to the prior results in ratios that are more even and thus more easily and accurately estimated. 

The degree to which the heating powers are pushed towards 0.0 is determined by the `_ss_alpha` setting. The code highlighted below allocates heating powers for steppingstone based on the number of chains specified and the value of `_ss_alpha`. Note that if `_ss_alpha` equals 1, then the powers will be evenly spaced between 0 and 1 (but note that power equal to 1 is never used). If `_ss_alpha` is less than 1, then the nominal (equally spaced) power is raised to the value 1/`_ss_alpha`. An example is provided in the comments for `K=5` chains and `_ss_alpha = 0.25`.
~~~~~~
{{ "steps/step-20/src/strom.hpp" | polcodesnippet:"begin_calcheatingpowers-end_calcheatingpowers","calcheatingpowers_start-calcheatingpowers_end,calcheatingpowers_trailer" }}
~~~~~~
{:.cpp}

#### Adding the calcMarginalLikelihood function

Once all heated chains have finished, the `calcMarginalLikelihood` function can be called to do the final computation.
~~~~~~
{{ "steps/step-20/src/strom.hpp" | polcodesnippet:"begin_calcMarginalLikelihood-end_calcMarginalLikelihood","" }}
~~~~~~
{:.cpp}
This function first creates a vector of pairs in which the first element is the heating power of the chain and the second element is the steppingstone ratio calculated for that chain. The next step is to sort this vector so that elements are in order of heating power. This sorting is purely cosmetic: sorting causes the output to have increasing chain powers and not a random ordering.

I mentioned in the introduction that the estimated marginal likelihood is simply the product of the ratios calculated from each steppingstone power posterior distribution. On the log scale, this amounts to adding up the log ratios computed by each chain. This simple summation is performed in the last section of this function while outputting the heating powers and the estimated log ratios from each chain.

#### Miscellany

Before moving on to making necessary modifications and additions to the `Chain` class, I want to finish up work on the `Strom` class.

Because it makes no sense to swap chains during steppingstone, we need to be sure to turn off chain swapping if steppingstone is being performed. In the swapChains function, this is as simple as changing `if (_num_chains == 1) return;` to  `if (_num_chains == 1 || _steppingstone) return;` in the first statement inside the function body:
~~~~~~
{{ "steps/step-20/src/strom.hpp" | polcodesnippet:"begin_swapChains-end_swapChains","swapChains_start-swapChains_stop" }}
~~~~~~
{:.cpp}

Make the same kind of modification to the `swapSummary` function:
~~~~~~
{{ "steps/step-20/src/strom.hpp" | polcodesnippet:"begin_swapSummary-end_swapSummary","swapSummary_start-swapSummary_stop" }}
~~~~~~
{:.cpp}

In order to calculate their particular steppingstone ratio, each chain must know not only the heating power that it is using but also the heating power of the power posterior distribution that is nested just inside it. The chains are told not only their heating power but this "next heating power" in the `initChains` function:
~~~~~~
{{ "steps/step-20/src/strom.hpp" | polcodesnippet:"begin_initChains-end_initChains","initChains_start-initChains_stop" }}
~~~~~~
{:.cpp}

The last modifications we need to make in the `Strom` class are to the `run` function: (1) add the `calcMarginalLikelihood` call and (2) avoid opening the parameter or tree output files if steppingstone is in progress. Note that `calcMarginalLikelihood` is a no-op (i.e. no-operation, nothing done) if we are not currently carrying out steppingstone.
~~~~~~
{{ "steps/step-20/src/strom.hpp" | polcodesnippet:"begin_run-end_run","open_param_tree_files_start,open_param_tree_files_stop,run_calcmarglike_start-run_calcmarglike_stop,close_param_tree_files_start,close_param_tree_files_stop" }}
~~~~~~
{:.cpp}


