---
layout: steps
title: The GammaRateVarUpdater Class
partnum: 14
subpart: 2
description: Create a class derived from Updater that can update the rate variance parameter governing the amount of among-site rate heterogeneity
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}
Most of the work of updating the discrete gamma rate variance parameter has already been done by creating the `Updater` base class. Now all we need to do is to fill in the blanks by implementing the abstract functions in `Updater`.

Create a new header file named {% indexfile gamma_ratevar_updater.hpp %} and add the following class declaration code to it:
~~~~~~
{{ "steps/step-14/src/gamma_ratevar_updater.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## Constructor and destructor

The constructor sets `_name` to be "Gamma Rate Variance" and calls the `clear` function. Note that `_name` is not listed as a data member of `GammaRateVarUpdater` in this class’s declaration above, so why are we allowed to modify its value here? The answer is that `_name` is a member of the `Updater` base class and thus is inherited by `GammaRateVarUpdater`. So, it really _is_ a data member of `GammaRateVarUpdater`.
~~~~~~
{{ "steps/step-14/src/gamma_ratevar_updater.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The clear member function

The `clear` function first calls the parent class’s version (`Updater::clear()`) and then returns data members `_prev_point` and `_curr_point` to their just-constructed state.
~~~~~~
{{ "steps/step-14/src/gamma_ratevar_updater.hpp" | polcodesnippet:"begin_clear-end_clear","" }}
~~~~~~
{:.cpp}

## The getCurrentPoint member function

This function returns the current value of the gamma rate variance parameter.
~~~~~~
{{ "steps/step-14/src/gamma_ratevar_updater.hpp" | polcodesnippet:"begin_getCurrentPoint-end_getCurrentPoint","" }}
~~~~~~
{:.cpp}

## The calcLogPrior member function

The `calcLogPrior` function is (you’ll remember) a pure virtual function that we must provide for this class in order to compile the program; there is no generic version of this function provided by the `Updater` parent class. The function body below calculates and returns the log of the Gamma prior probability density at `_curr_point`. The shape and scale parameters of the Gamma prior distribution are extracted from the `_prior_parameters` vector. This function body shows that the program produced by this tutorial will not allow the user to apply anything other than a Gamma prior distribution for this parameter, but the user will be able to set the parameters of that Gamma prior distribution.
~~~~~~
{{ "steps/step-14/src/gamma_ratevar_updater.hpp" | polcodesnippet:"begin_calcLogPrior-end_calcLogPrior","" }}
~~~~~~
{:.cpp}

## The revert member function

In the event that the proposed value is rejected, this function can be called to copy `_prev_point` to `_curr_point`.
~~~~~~
{{ "steps/step-14/src/gamma_ratevar_updater.hpp" | polcodesnippet:"begin_revert-end_revert","" }}
~~~~~~
{:.cpp}

## The proposeNewState member function

This is another pure virtual function in the parent class `Updater` that we must provide. The proposal algorithm used here centers a proposal window of width `_lambda` over `_prev_point`, then proposes a new value uniformly from within that proposal window. If the proposed value is less than zero, the proposed value is reflected back into the valid parameter space by simply multiplying the value by -1. While counterintuitive, this proposal is symmetric: the probability density of the proposed point given `_prev_point` equals the probability density of `_prev_point` given the proposed point. Thus, the Hastings Ratio is 1, and its log is zero.
~~~~~~
{{ "steps/step-14/src/gamma_ratevar_updater.hpp" | polcodesnippet:"begin_proposeNewState-end_proposeNewState","" }}
~~~~~~
{:.cpp}

