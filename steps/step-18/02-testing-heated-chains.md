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
                  State Frequencies        1000.000           100.0            4203
                  Exchangeabilities        1000.000           100.0            4255
                Gamma Rate Variance          10.568            29.6            4250
     Proportion of Invariable Sites           1.000           100.0            4209
         Tree Topol. and Edge Prop.        1000.000           100.0           78939
                        Tree Length          13.137            29.7            4144

Chain 1 (power 0.66667)
                            Updater   Tuning Param.        Accept %     No. Updates
                  State Frequencies        1000.000           100.0            4173
                  Exchangeabilities        1000.000           100.0            4222
                Gamma Rate Variance          13.741            30.3            4229
     Proportion of Invariable Sites           1.000           100.0            4122
         Tree Topol. and Edge Prop.        1000.000           100.0           79047
                        Tree Length          13.716            28.1            4207

Chain 2 (power 0.50000)
                            Updater   Tuning Param.        Accept %     No. Updates
                  State Frequencies        1000.000           100.0            4165
                  Exchangeabilities        1000.000           100.0            4154
                Gamma Rate Variance          10.863            30.6            4159
     Proportion of Invariable Sites           1.000           100.0            4279
         Tree Topol. and Edge Prop.        1000.000           100.0           79040
                        Tree Length          11.490            29.4            4203

Chain 3 (power 0.40000)
                            Updater   Tuning Param.        Accept %     No. Updates
                  State Frequencies        1000.000           100.0            4204
                  Exchangeabilities        1000.000           100.0            4217
                Gamma Rate Variance          13.100            30.6            4224
     Proportion of Invariable Sites           1.000           100.0            4204
         Tree Topol. and Edge Prop.        1000.000           100.0           79095
                        Tree Length          12.140            29.4            4056
~~~~~~
{:.bash-output}

The acceptance fractions are near 30% (which is what we specified as the target acceptance percentage in the `Chain::createUpdaters` function) for _Gamma Rate Variance_ and _Tree Length_. For _State Frequencies_, _Exchangeabilities_, and _Tree Topology and Edge Proportions_, the tuning parameter has maxed out at a boldness of 1000 and the acceptance percentages are both 100%. For _Proportion of Invariable Sites_, the acceptance percentage is also 100% but the tuning parameter maxed out at 1.0 rather than 1000. Why is this? The parameters modified by these four updaters are the only parameters with truly flat priors, and, under a flat prior, all possible combinations have equal probability density. Because there are no downhill moves when updating state frequencies, exchangeabilities, tree topology, edge length proportions, or the proportion of invariable sites, all proposals are accepted. It is impossible to tune the samplers in this case because the proposal will be accepted regardless of how far away the proposed point is from the current point. You should not see this behavior when analyzing real data because sequence data contains information about these parameters that results in a posterior that is not perfectly flat. The fact that the _Proportion of Invariable Sites_ tuning parameter maxes out at 1 rather than 1000 is due to the highlighted lines at the beginning of the `PinvarUpdater::proposeNewState` function:
~~~~~~
{{ "steps/step-18/src/pinvar_updater.hpp" | polcodesnippet:"begin_proposeNewState-end_proposeNewState","a-b" }}
~~~~~~
{:.cpp}
Because the proposal method used for pinvar is a window of width `_lambda`, it is prudent to limit `_lambda` to the maximum possible window width, namely 1.0. Allowing `_lambda` to get larger than 1.0 risks having to reflect multiple times if the proposed new state lies outside the valid inteval 0.0 to 1.0.

The swap summary shows that more than 16000 swaps were attempted between each pair of chains, fewer swaps were successful between chains that differed substantially in their heating power. For example, 100*12381/16377 = 76% of swaps were accepted between the two hottest chains whereas only 100*4118/16876 = 24% were accepted between the cold chain and the hottest chain. This is explained by the fact that the hottest chain is often far away from a peak while the cold chain never strays far from a peak; the probability that the cold chain will accept a jump from the peak to the lowlands is small, so, despite the fact that the heated chain can accept the swap because it is uphill, fewer swaps succeed between the coldest and hottest chains than between chains visiting more similar landscapes. The reason that even 24% of such swaps succeed has a lot to do with the fact that the landscape is determined entirely by the prior in this case, so there is not a huge difference between the altitude of the peak compared to the lowlands.
~~~~~~
Swap summary (upper triangle = no. attempted swaps; lower triangle = no. successful swaps):
                        0            1            2            3
----------------------------------------------------------------
           0          ---        16520        16589        16824
           1        11719          ---        16561        16908
           2         8699        13130          ---        16598
           3         6898        10922        13951          ---
----------------------------------------------------------------
~~~~~~
{:.bash-output}

Feel free to run the program again, this time setting `usedata=yes` (or leaving out the `usedata` program altogether, which will default to using the data).

## Almost there!

You now have a program that can carry out a Bayesian phylogenetic analysis using either a GTR+I+G or a codon model, allowing data to be partitioned, and can use multiple chains during MCMC. With slight modification, your program can make use of the GPU version of BeagleLib when computing likelihoods, which greatly speeds up the codon model if GPUs are available.

In the next step in the tutorial, we will implement reversible-jump MCMC to make it possible to explore trees containing polytomies. 

