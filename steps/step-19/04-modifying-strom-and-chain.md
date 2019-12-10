---
layout: steps
title: Modifying the Strom and Chain classes
partnum: 19
subpart: 4
description: Deploy PolytomyUpdater and add prior options to Strom.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

** NOT YET WRITTEN **

Run the program using the following `strom.conf` file:
~~~~~~
{{ "steps/step-19/test/strom.conf" | polcodesnippet:"","" }}
~~~~~~
{:.cpp}

Note that no data will be used for this run, so the MCMC analysis will explore the prior. You'll remember that we did this in the previous step, but now there is a convenient program option to allow easy switching between using data and ignoring data. 

After the run has finished, load the resulting {% indexfile params.txt %} file into the program Tracer to summarize the results. You should find that the means and variances of all parameters are the same as in the previous step: adding multiple chains does not change the distribution being explored by the cold chain, it simply makes moving around that landscape more efficient.

Note that chain summaries are provided for all four chains.
~~~~~~
Chain 0 (power 1.00000)
                       Updater   Tuning Param.        Accept %
             State Frequencies        1000.000           100.0
             Exchangeabilities        1000.000           100.0
           Gamma Rate Variance          13.365            30.4
Proportion of Invariable Sites           1.000           100.0
         Tree and Edge Lengths           7.088            29.8
                   Tree Length          11.087            30.0

Chain 1 (power 0.66667)
                       Updater   Tuning Param.        Accept %
             State Frequencies        1000.000           100.0
             Exchangeabilities        1000.000           100.0
           Gamma Rate Variance          11.197            30.9
Proportion of Invariable Sites           1.000           100.0
         Tree and Edge Lengths           6.510            29.2
                   Tree Length          12.093            30.4

Chain 2 (power 0.50000)
                       Updater   Tuning Param.        Accept %
             State Frequencies        1000.000           100.0
             Exchangeabilities        1000.000           100.0
           Gamma Rate Variance          12.910            29.6
Proportion of Invariable Sites           1.000           100.0
         Tree and Edge Lengths           7.487            29.2
                   Tree Length          11.702            31.0

Chain 3 (power 0.40000)
                       Updater   Tuning Param.        Accept %
             State Frequencies        1000.000           100.0
             Exchangeabilities        1000.000           100.0
           Gamma Rate Variance          13.306            30.8
Proportion of Invariable Sites           1.000           100.0
         Tree and Edge Lengths           7.332            29.3
                   Tree Length          11.956            31.4
~~~~~~
{:.bash-output}

The acceptance fractions are all near 30% (which is what we specified as the target acceptance percentage in the `Chain::createUpdaters` function) with the exception of State Frequencies, Exchangeabilities, and Pinvar. For the first two of these, the tuning parameter has maxed out at a boldness of 1000 and the acceptance percentages are both 100%. For Pinvar, the acceptance percentage is also 100% but the tuning parameter maxed out at 1.0 rather than 1000. Why is this? These three parameters are the only parameters with a truly flat prior, and under a flat prior, all possible combinations have equal probability density. Because there are no downhill moves when updating State Frequencies, Exchangeabilities, or Pinvar, all proposals are accepted. It is impossible to tune the samplers in this case because the proposal will be accepted regardless of how far away the proposed point is from the current point. You should not see this behavior when analyzing real data because sequence data contains information about these parameters that results in a posterior that is not perfectly flat. The fact that the Pinvar tuning parameter maxes out at 1 rather than 1000 is due to the highlighted lines at the beginning of the `PinvarUpdater::proposeNewState` function:
~~~~~~
{{ "steps/step-18/src/pinvar_updater.hpp" | polcodesnippet:"begin_proposeNewState-end_proposeNewState","a-b" }}
~~~~~~
{:.cpp}
Because the proposal method used for pinvar is a window of width `_lambda`, it is prudent to limit `_lambda` to the maximum possible window width, namely 1.0. Allowing `_lambda` to get larger than 1.0 risks having to reflect multiple times if the proposed new state lies outside the valid inteval 0.0 to 1.0.

Feel free to run the program again, this time setting `usedata=yes` (or leaving out the `usedata` program altogether, which will default to using the data).

## The End!

This is the end of the current version of the tutorial. You now have a program that can carry out a Bayesian phylogenetic analysis using either a GTR+I+G or a codon model, allowing data to be partitioned, and can use multiple chains during MCMC. With slight modification, your program can make use of the GPU version of BeagleLib when computing likelihoods, which greatly speeds up the codon model if GPUs are available.

I plan to extend the tutorial in the next version to allow polytomous and/or rooted trees, and estimation of marginal likelihoods.
