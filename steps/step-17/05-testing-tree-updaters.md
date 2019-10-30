---
layout: steps
title: Testing TreeUpdater and TreeLengthUpdater
partnum: 17
subpart: 5
description: Explore both the prior and posterior while updating tree topology and edge lengths. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

Run the program using this {% indexfile strom.conf %} file.
~~~~~~
{{ "steps/step-17/test/strom.conf" | polcodesnippet:"start-end",""}}
~~~~~~
{:.cpp}

This time take a look at the file {% indexfile trees.tre %} and note how the tree topology and edge lengths vary during the run. You may wish to load the tree file into the program [FigTree](https://beast.community/figtree) to view the trees. 

## Exploring the prior

A good way to test Bayesian MCMC software is to allow the program to explore the prior rather than the posterior. Because the prior is known, this provides a good sanity check of the MCMC machinery.

Modify the `Strom::run` function, adding just the one line highlighted in bold, blue text. The default behavior is to use data if it is available (see the `Likelihood::clear` function). The line we're adding sets `Likelihood::_using_data` to `false`, which causes the `Likelihood::calcLogLikelihood` to always return 0.0 rather than computing the log-likelihood.
~~~~~~
{{ "steps/step-17/src/strom.hpp" | polcodesnippet:"begin_run-end_run","a"}}
~~~~~~
{:.cpp}

Bump up the number of iterations (niter) in the `strom.conf` file to 100,000 in order to get an extremely good sample:
~~~~~~
niter = 100000
~~~~~~
{:.bash-output}

Run the program again and view the results in [Tracer](https://beast.community/tracer).

### Check the tree length prior

![Exponential distribution with mean 10]({{ "/assets/img/exponential_mean_10.png" | absolute_url }}){: .right-image}
In Tracer, bring the panel labeled "Marginal Density" to the front and click on "TL" in the Traces list. We specified the tree length prior to be Exponential with mean 10. To the right is a plot of this density for comparison. Because Tracer approximates the density using a generic kernel density estimator, which does not take into account the fact that an exponential 0.1 density must have height 0.1 at 0.0, the density shown in Tracer will not exactly fit the plot, but it should be close except for the gap on the left end. 

Switch to the "Estimates" panel while still selecting TL in Traces: the mean of the distribution should be close to 10 and the variance close to 100.
 
### Check the subset relative rates

The subset relative rates have a prior distribution related to Dirichlet(1,1,1). The difference lies in the fact that it is the product of subset rates and subset proportions that is distributed as Dirichlet(1,1,1), not the rates themselves. Select all three subset relative rates (m-0, m-1, and m-2) under Traces on the left. These three densities should be identical ("Marginal Density" panel), and they should all have means close to 1.0 and variances close to 0.5. 
{% comment %}
variance of Dirichlet(1,1,1) = (1/3)(2/3)/4 = (1/2)(1/9)
variance of RelRate(1,1,1) = (3^2)Var(Dir(1,1,1)) = 1/2
{% endcomment %}

### Check the state frequencies

The state frequencies have prior Dirichlet(1,1,1,1). This is a flat Dirichlet distribution. Each component of a flat Dirichlet has variance equal to
~~~~~
variance of one component = (1/n)*(1 - 1/n)/(n + 1)
~~~~~
{:.bash-output}
where n is the number of dimensions (i.e. the number of Dirichlet parameters). For nucleotide state frequencies, n = 4 so the variance of one component should equal (1/4)(3/4)/5 = 3/80 = 0.0375. Being flat, each state frequency has the same probability as any other state frequency, so the mean for each should be 1/4 = 0.25. 

Select all 4 state frequencies for any subset (e.g. piA-0, piC-0, piG-0, and piT-0) under Traces on the left. These 4 densities should be identical ("Marginal Density" panel), and they should all have means close to 1/4 and variances close to 0.0375. 
{% comment %}
variance of Dirichlet(1,1,1,1) = (1/4)(3/4)/5 = 3/80 = 0.0375
{% endcomment %}

### Check the exchangeabilities

The exchangeabilities have prior Dirichlet(1,1,1,1,1,1). Select all 6 exchangeabilities for any subset (e.g. rAC-0, rAG-0, rAT-0, rCG-0, rCT-0, and rGT-0) under Traces on the left. These 6 densities should be identical ("Marginal Density" panel), and they should all have means close to 0.167 and variances close to 0.0198. These values can be obtained using the formula given above with n = 6 rather than n = 4. 
{% comment %}
variance of Dirichlet(1,1,1,1,1,1) = (1/6)(5/6)/7 = 5/252 = 0.0198
{% endcomment %}

### Check pinvar

The pinvar parameter has prior Beta(1,1), which is just a special case of a flat Dirichlet distribution. Hence, using n = 2, we find that pinvar should have mean 0.5 and variance (1/2)(1/2)/3 = 1/12 = 0.0833.

### Check the rate variance

Fihnally, the rate variance parameter has prior Exponential(1), which has mean 1 and variance 1.

{% comment %}
## Preparing for the final step

If all of these distributions appear satisfactory, then change the line
~~~~~~
_likelihood->useStoredData(false);
~~~~~~
{:.cpp}
 in Strom::run so that data are once again used:
~~~~~~
_likelihood->useStoredData(true);
~~~~~~
{:.cpp}
{% endcomment %}

## On to the final step!

The final step of the tutorial will introduce multiple chains (Metropolis coupling) to the MCMC analysis to improve mixing. We will also add a program option (`usedata`) to make ignoring data (exploring the prior) easy.
