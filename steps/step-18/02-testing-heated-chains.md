---
layout: steps
title: Testing Heated Chains
partnum: 18
subpart: 2
description: Perform an MCMC analysis using one cold and three heated chains.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

Run the program using the following `strom.conf` file:
~~~~~~
{{ "steps/step-18/test/strom.conf" | polcodesnippet:"start-end","" }}
~~~~~~
{:.cpp}

Note that no data will be used for this run, so the MCMC analysis will explore the prior. You'll remember that we did this in the previous step, but now there is a convenient program option to allow easy switching between using data and ignoring data. 

After the run has finished, load the resulting {% indexfile params.txt %} file into the program Tracer to summarize the results. You should find that the means and variances of all parameters are the same as in the previous step: adding multiple chains does not change the distribution being explored by the cold chain, it simply makes moving around that landscape more efficient.

Note that chain summaries are provided for all four chains.
~~~~~~
Chain 0 (power 1.00000)
                       Updater   Tuning Param.        Accept %     No. Updates
             State Frequencies        1000.000           100.0            4091
             Exchangeabilities        1000.000           100.0            4077
           Gamma Rate Variance          14.496            27.0            4152
Proportion of Invariable Sites           1.000           100.0            4154
         Tree and Edge Lengths           7.189            30.0           79353
                   Tree Length          14.111            32.5            4173

Chain 1 (power 0.66667)
                       Updater   Tuning Param.        Accept %     No. Updates
             State Frequencies        1000.000           100.0            4110
             Exchangeabilities        1000.000           100.0            4250
           Gamma Rate Variance          12.001            26.4            4318
Proportion of Invariable Sites           1.000           100.0            4171
         Tree and Edge Lengths           6.932            29.7           79013
                   Tree Length           9.822            33.4            4138

Chain 2 (power 0.50000)
                       Updater   Tuning Param.        Accept %     No. Updates
             State Frequencies        1000.000           100.0            4234
             Exchangeabilities        1000.000           100.0            4152
           Gamma Rate Variance          15.390            26.7            4203
Proportion of Invariable Sites           1.000           100.0            4127
         Tree and Edge Lengths           6.902            29.8           79094
                   Tree Length          10.265            33.8            4190

Chain 3 (power 0.40000)
                       Updater   Tuning Param.        Accept %     No. Updates
             State Frequencies        1000.000           100.0            4179
             Exchangeabilities        1000.000           100.0            4149
           Gamma Rate Variance          15.177            26.1            4195
Proportion of Invariable Sites           1.000           100.0            4126
         Tree and Edge Lengths           7.104            29.7           79189
                   Tree Length          10.590            32.9            4162
~~~~~~
{:.bash-output}

The acceptance fractions are all near 30% (which is what we specified as the target acceptance percentage in the `Chain::createUpdaters` function) with the exception of State Frequencies, Exchangeabilities, and Pinvar. For the first two of these, the tuning parameter has maxed out at a boldness of 1000 and the acceptance percentages are both 100%. For Pinvar, the acceptance percentage is also 100% but the tuning parameter maxed out at 1.0 rather than 1000. Why is this? These three parameters are the only parameters with a truly flat prior, and under a flat prior, all possible combinations have equal probability density. Because there are no downhill moves when updating State Frequencies, Exchangeabilities, or Pinvar, all proposals are accepted. It is impossible to tune the samplers in this case because the proposal will be accepted regardless of how far away the proposed point is from the current point. You should not see this behavior when analyzing real data because sequence data contains information about these parameters that results in a posterior that is not perfectly flat. The fact that the Pinvar tuning parameter maxes out at 1 rather than 1000 is due to the highlighted lines at the beginning of the `PinvarUpdater::proposeNewState` function:
~~~~~~
{{ "steps/step-18/src/pinvar_updater.hpp" | polcodesnippet:"begin_proposeNewState-end_proposeNewState","a-b" }}
~~~~~~
{:.cpp}
Because the proposal method used for pinvar is a window of width `_lambda`, it is prudent to limit `_lambda` to the maximum possible window width, namely 1.0. Allowing `_lambda` to get larger than 1.0 risks having to reflect multiple times if the proposed new state lies outside the valid inteval 0.0 to 1.0.

The swap summary shows that more than 16000 swaps were attempted between each pair of chains, fewer swaps were successful between chains that differed substantially in their heating power. For example, 12934/16854 = 77% of swaps were accepted between the two hottest chains whereas only 4608/16789 = 27% were accepted between the cold chain and the hottest chain. This is explained by the fact that the hottest chain is often far away from a peak while the cold chain never strays far from a peak; the probability that the cold chain will accept a jump from the peak to the lowlands is small, so, despite the fact that the heated chain can accept the swap because it is uphill, fewer swaps succeed between the coldest and hottest chains than between chains visiting more similar landscapes. The reason that even 27% of such swaps succeed has a lot to do with the fact that the landscape is determined entirely by the prior in this case, so there is not a huge difference between the altitude of the peak compared to the lowlands.
~~~~~~
Swap summary (upper triangle = no. attempted swaps; lower triangle = no. successful swaps):
                        0            1            2            3
----------------------------------------------------------------
           0          ---        16664        16427        16789
           1        10631          ---        16524        16742
           2         6831        12050          ---        16854
           3         4608         8644        12934          ---
----------------------------------------------------------------
~~~~~~
{:.bash-output}

Feel free to run the program again, this time setting `usedata=yes` (or leaving out the `usedata` program altogether, which will default to using the data).

## The End!

This is the end of the current version of the tutorial. You now have a program that can carry out a Bayesian phylogenetic analysis using either a GTR+I+G or a codon model, allowing data to be partitioned, and can use multiple chains during MCMC. With slight modification, your program can make use of the GPU version of BeagleLib when computing likelihoods, which greatly speeds up the codon model if GPUs are available.

I plan to extend the tutorial in the next version to allow polytomous and/or rooted trees, and estimation of marginal likelihoods.
