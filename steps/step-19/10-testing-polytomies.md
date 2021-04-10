---
layout: steps
title: Testing reversible-jump MCMC
partnum: 19
subpart: 10
description: Perform an MCMC analysis of the prior to demonstrate that the new polytomy-aware MCMC is working.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

Replace your `strom.conf` file with the following version. 
~~~~~~
datafile        = rbcl10.nex
treefile        = rbcl10.tre
tree            = default:1
subset          = default[nucleotide]:1-1314
statefreq       = default:[0.296038, 0.190571, 0.202137, 0.311254]
ratevar         = default:[4.08241581]
rmatrix         = default:[1.21774, 5.72420, 1.25796, 1.24770, 9.51219, 1.0]
ncateg          = default:4
pinvar          = default:[0.0]
nchains         = 1
burnin          = 10000
niter           = 10000000
samplefreq      = 100
printfreq       = 1000000
seed            = 171395
expectedLnL     = -6736.701
usedata         = no
gpu             = no
ambigmissing    = yes
allowpolytomies = yes
resclassprior   = yes
topopriorC      = 1.0
~~~~~~
{:.bash-output}

## Expectations

This version is set up to perform an MCMC analysis of the 10-taxon rbcL data set, but note that `usedata = no` so the prior, not the posterior, will be analyzed. Setting `allowpolytomies = yes`, `resclassprior = yes`, and `topopriorC = 1.0` means that the star tree should appear as often in the sample as all fully-resolved trees combined. This is not easy to verify by hand, but you could easily verify that this is true by inspection if you substitute rbcl10.nex with a 4-taxon example where the length of the tree description in the resulting {% indexfile trees.tre %} file provides a means of counting star trees vs. fully-resolved trees.

## A challenge

You can also think about modifying your `TreeSummary` class to summarize the trees in `trees.tre`. One thing you will need to do is be sure that the final boolean argument in calls to the `TreeManip::buildFromNewick` function is set to `true` so that polytomies are allowed. You can revisit Step 7 (Summarizing tree topologies) for hints about how to go about reading the tree file and outputting a summary. 

Once you get the basic `TreeSummary::showSummary()` working, consider adding a `showResolutionClassSummary` function to `TreeSummary` that tallies up the number of sampled trees in each resolution class.

## Up next

In the next step, we'll add an option that will allow estimation of the log marginal likelihood of a model. Marginal likelihoods are used in Bayesian model selection to compare models, and the current program is preadapted for carrying out the steppingstone method for marginal likelihood estimation due to the fact that it can run multiple chains with different heating powers.



