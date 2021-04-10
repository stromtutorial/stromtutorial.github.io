---
layout: steps
title: Modifying the Chain class for steppingstone
partnum: 20
subpart: 2
description: Modifying the Chain class for steppingstone.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

We need to add some data members and member functions to the `Chain` class in order for each chain to do its part in computing the marginal likelihood. In the declaration of the class, add data members `_heat_likelihood_only`, `_next_heating_power`, and `_ss_loglikes`, and member functions `setNextHeatingPower`, `storeLogLikelihood`, and `calcLogSteppingstoneRatio`.
~~~~~~
{{ "steps/step-20/src/chain.hpp" | polcodesnippet:"begin_chain_class_declaration-end_chain_class_declaration","chain_funcs_start-chain_funcs_stop,chain_data_start-chain_data_stop" }}
~~~~~~
{:.cpp}

Initialize the new data members in the `clear` function:
~~~~~~
{{ "steps/step-20/src/chain.hpp" | polcodesnippet:"begin_clear-end_clear","clear_start-clear_stop" }}
~~~~~~
{:.cpp}

#### The setNextHeatingPower function

This function sets the `_heat_likelihood_only` data member to true and the `_next_heating_power` data member to the supplied power `p`. It also informs all updaters that only the likelihood should be raised to a power, not the prior. Steppingstone power posteriors differ from Metropolis-coupled Markov chains in that only the likelihood is raised to a power. In MCMCMC, the entire posterior kernel is heated. Because `setNextHeatingPower` is only called when doing steppingstone, this function provides a convenient place to make sure all the updaters know that we are doing steppingstone-style heating. I will explain why each chain needs to know not only its own heating power but also the heating power of the next power posterior nested within it when I discuss the `calcLogSteppingstoneRatio` function below.
~~~~~~
{{ "steps/step-20/src/chain.hpp" | polcodesnippet:"begin_setNextHeatingPower-end_setNextHeatingPower","" }}
~~~~~~
{:.cpp}

#### The storeLogLikelihood function

This is a simple function that simply adds the current log likelihood to the end of the `_ss_loglikes` vector.
~~~~~~
{{ "steps/step-20/src/chain.hpp" | polcodesnippet:"begin_storeLogLikelihood-end_storeLogLikelihood","" }}
~~~~~~
{:.cpp}

#### The calcLogSteppingstoneRatio function

This function is neither long nor complicated, but I need to provide some explanation if you are to understand how it works. I have described the steppingstone method as basically throwing darts randomly and uniformly within the area defined by one power posterior and then counting how many of those darts fall underneath the next power posterior nested within. That metaphor is very useful for gaining an intuitive understanding, but, in practice, the steppingstone uses a modification that is more efficient. Instead of choosing both an x-coordinate (i.e. a point in parameter space) as well as a y-coordinate for each dart (by choosing a point uniformly between 0 and the outer power posterior density), then looking to see whether the y-coordinate is also beneath the inner power posterior, we will instead just choose a point in parameter space and compute the expected fraction of darts that would fall under the inner power posterior were we to actually throw them! It's as if we are throwing an infinite number of darts for each parameter value sampled, and effectively gives us an infinitely large sample size for free. First, I'll show you the function body and then I'll explain.
~~~~~~
{{ "steps/step-20/src/chain.hpp" | polcodesnippet:"begin_calcLogSteppingstoneRatio-end_calcLogSteppingstoneRatio","" }}
~~~~~~
{:.cpp}

At the time this function is called (within `Strom::calcMarginalLikelihood`), the `_ss_loglikes` vector is full of sampled log likelihoods. Each of these values represents the log likelihood of a point sampled from the particular power posterior distribution that the chain has implemented. That log likelihood is associated with a particular point. Here are a couple of power posterior distributions showing some darts thrown at the larger (enclosing) distribution:

![Na&iuml;ve approach to estimating the area under the posterior kernel]({{ "/assets/img/steppingstone-slice.png" | relative_url }}){:.center-math-image}

The x-coordinates of the orange dots represent sampled points from that _enclosing_ power posterior distribution. Thus, we can obtain the x-coordinates of the darts just from recording the points visited by the chain that is exploring that _enclosing_ distribution. The y-coordinate of the sampled orange dot could be obtained by sampling uniformly from the interval represented by the vertical dotted line, which extends from zero up to the height of the enclosing power posterior density at the sampled point. Imagine sampling not just one point along that dotted line, but instead sampling an infinite number of points uniformly along that line. The fraction of points sampled with that same x-coordinate that are underneath the _enclosed_ power posterior density is just the height of the _enclosed_ power posterior density (labeled _x_) divided by the height of the _enclosing_ power posterior density (labeled _y_). So, we can simulate throwing an infinite number of darts at the larger power posterior and counting the fraction that lie beneath the smaller, nested power posterior by sampling x-coordinates and, for each, computing the ratio of power posterior densities! 

The ratio of power posteriors is the ratio of likelihoods multiplied by the ratio of priors. Because the prior is the same for both (remember, the prior is not heated), the ratio of priors equals 1. So, to compute the ratio of the enclosed to the enclosing power posterior density, we need only compute the likelihood and raise it to a power equal to the difference in heating powers:
![Ratio of power posteriors]({{ "/assets/img/likelihood-ratio.png" | relative_url }}){:.center-math-image}

Thus, an estimate of the ratio ("steppingstone') needed for this particular chain can be obtained by raising each sampled likelihood by the difference in chain heating powers, `_next_heating_power - _heating_power` and computing the simple average. 

A perusal of the code above for the `calcLogSteppingstoneRatio` function shows, however, that something more complicated is being done. The problem is that the average computed the simple way can be numerically unstable. We must avoid leaving the log scale for likelihoods because, for all but the smallest of data sets, underflow will turn these tiny likelihoods into 0. How do you compute a sum when the elements you are summing together must all stay on the log scale?

The way to solve this problem is to factor out the largest term (assume that the largest term is _c_ in the example below):
![Factoring out the largest value in a sum]({{ "/assets/img/factor-out-term-c.png" | relative_url }}){:.center-math-image}

That doesn't solve our problem, however, because _a_, _b_, and _c_ are not on the log scale. Here's how we can fix that:
![Factoring out largest term on log scale]({{ "/assets/img/exponentiate-terms-in-sum.png" | relative_url }}){:.center-math-image}
Now we have the log of the sum on the left, and on the right side _a_, _b_, and _c_ are all present only on log scale. Now it doesn't matter if some of the terms of the sum on the right underflow because those terms represent likelihoods that were so much smaller than the largest value that they would not have made any measurable difference to the sum anyway.

Now you should be in a position to understand the body of the `calcLogSteppingstoneRatio` function. The function first finds the largest log likelihood stored in the `_ss_loglikes` vector (this is analogous to finding the value _c_ in the example above). The value returned by `std::max_element` is an iterator, not a value, so the initial asterisk is necessary to dereference (i.e. obtain the value pointed to by) the iterator).

The next section computes the sum of the exponentiated terms. The only difference between this section and our example is the fact that the ratio of log likelihoods is being raised to the power `_next_heating_power - _heating_power`, which is an essential part of the steppingstone method.

The final section finishes the computation of the log ratio by adding the log of the largest term (also raised to the power specified by the steppingstone method and converting the sum to a mean by dividing by the sample size (which is done by subtracting the log of the sample size since we are on the log scale).



