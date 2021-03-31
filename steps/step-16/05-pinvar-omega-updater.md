---
layout: steps
title: The PinvarUpdater and OmegaUpdater Classes
partnum: 16
subpart: 5
description: Create the derived classes PinvarUpdater and OmegaUpdater from the base class Updater. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

These classes are so similar to `GammaRateVarUpdater` that I will simply provide the entire source code for each of these classes and make a few comments about the few differences that do exist.

## The PinvarUpdater class

Other than trivial differences (e.g. the name of the class is necessarily different), this class differs from `GammaRateVarUpdater` primarily in the fact that a Beta prior is used for this parameter (because its support is limited to the unit interval) and the proposal is symmetric. The `calcLogPrior` function computes a Beta(a,b) prior density for the current point, where `a` and `b` are the two values supplied to it via `Updater::setPriorParameters`. A new value is proposed by drawing a value from a `Uniform(curr_point - lambda/2, curr_point + lambda/2)`. If the proposed point is less than 0.0 or greater than 1.0, it is reflected back into the support interval. An assert at the beginning of `proposeNewState` ensures that `_lambda` is not large enough that multiple reflections are ever needed. Like the similar proposal used in the `GammaRateVarUpdater`, this proposal is symmetric and thus the Hastings ratio is 1 (0 on log scale).

Enter the following text into a new header file named {% indexfile pinvar_updater.hpp %}.
~~~~~~
{{ "steps/step-16/src/pinvar_updater.hpp" | polcodesnippet:"start-end","" }}
~~~~~~
{:.cpp}

## The OmegaUpdater class

This class is essentially identical to `GammaRateVarUpdater` except for the fact that a shared pointer to a `QMatrix` object is passed into the constructor and stored instead of a shared ponter to an `ASRV` object. Another difference is that a {% indexshow multiplicative proposal %} (rather than a centered window proposal) is used to propose new parameter values in `OmegaUpdater`. The proposal algorithm used here generates a proposed value close to the old value (`_prev_point`) by multiplying `_prev_point` by a factor that is either slightly larger than or slightly smaller than 1.0. What is meant by "slightly" depends on `_lambda`, the boldness factor, with larger values of `_lambda` yielding bigger distances, on average, between `_prev_point` and the proposed point. This proposal is not symmetric: the probability density of proposing the new point given `_prev_point` is not the same as the probability density of proposing `_prev_point` given the new point. This proposal asymmetry means that we have to put the (log of the) ratio of these two probabilities into the `_log_hastings_ratio` data member so that the asymmetry of the move can be countered by a modified acceptance probability. That is, if proposals tend to be to the right twice as often as to the left, we accept half as many to the right to compensate for the proposal bias.

Enter the following text into a new header file named {% indexfile omega_updater.hpp %}.
~~~~~~
{{ "steps/step-16/src/omega_updater.hpp" | polcodesnippet:"start-end","" }}
~~~~~~
{:.cpp}
