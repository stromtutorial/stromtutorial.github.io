---
layout: steps
title: The SubsetRelRateUpdater Class
partnum: 16
subpart: 4
description: Create the derived class SubsetRelRateUpdater from the base class DirichletUpdater. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

Creating a `SubsetRelRateUpdater` is slightly more complicated than creating the `StateFreqUpdater` and `ExchangeabilityUpdater` classes. That's because subset relative rates do not themselves have a Dirichlet prior distribution. Dirichlet distributions are defined on a simplex, which means that a Dirichlet random variable is a vector whose elements sum to 1.0. Subset relative rates do not sum to 1; instead, their _mean_ is 1. What does sum to 1.0 are the components of the mean. To see this, consider how the mean relative substitution rate across subsets is computed (in pseudocode):
~~~~~~
mean relative rate = r_1 p_1 + r_2 p_2 + ... + r_k p_k
~~~~~~
{:.bash-output}
Here, `r_i` is the relative rate at which subset `i` is evolving and `p_i` is the probability that a site is in subset `i` (and thus has relative rate `r_i`). The probability `p_i` is simply the proportion of sites in subset `i`. The mean relative rate equals 1.0, by design, because subset relative rates affect the _heterogeneity_ in substitution rate across subsets, not the absolute rate. Writing the same equation in this way:
~~~~~~
mean relative rate = y_1 + y_2 + ... + y_k = 1.0
~~~~~~
{:.bash-output}
Shows that the values `y_i = r_i p_i` form a simplex and can thus have a Dirichlet prior distribution. Because of the involvement of the `p_i`, this means that the `r_i` do not themselves have a Dirichlet prior. The distribution of the subset relative rates can be easily obtained, however, and the prior density for a vector of `K` subset relative rates is identical to a Dirichlet density multiplied by a constant factor equal to the product of the first `K-1` proportions (i.e. `p_1 p_2 ... p_(K-1)`). 

In order to use the `DirichletUpdater` base class, we must provide it with `y_i` values rather than `r_i` values, and then multiply the prior computed by `DirichletUpdater` by the constant factor described above. We override the virtual function `DirichletUpdater::calcLogPrior` in order to make the needed modification.

Create a new header file named {% indexfile subset_relrate_updater.hpp %} and fill it with the following.
~~~~~~
{{ "steps/step-16/src/subset_relrate_updater.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## Constructor and destructor

The constructor first calls the `clear` function defined by the `DirichletUpdater` base class. It then sets its `_name` to “Subset Relative Rates” and the data member `_model` to the argument supplied as the `model` parameter. The destructor is merely a placeholder, as usual.
~~~~~~
{{ "steps/step-16/src/subset_relrate_updater.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The calcLogPrior member function

As explained above, this function first calls the `calcLogPrior` member function of the base class `DirichletUpdater` in order to do the majority of the work. The only remaining work is to add the log of the first `K-1` subset proportions (where `K` is the number of subsets) to the log prior value returned by `DirichletUpdater::calcLogPrior`. Each of these subset proportions equals the number of sites in the subset divided by the total number of sites. On log scale, this turns into the log of the number of sites in the subset minus the log of the total number of sites.
~~~~~~
{{ "steps/step-16/src/subset_relrate_updater.hpp" | polcodesnippet:"begin_calcLogPrior-end_calcLogPrior","" }}
~~~~~~
{:.cpp}

## The pullFromModel and pushToModel member functions

The `pullFromModel` function cannot simply copy the subset relative rates from the `_subset_relrates` data member of model into `_curr_point`. Instead, each element of `_curr_point` is the product of a subset relative rate and the proportion of sites in that subset.

Similarly, the reverse process is used in `pushToModel`. Each element of `_curr_point` is divided by the proportion of sites in the subset in order to extract just the subset relative rate. These relative rates are stored in a temporary vector `tmp` and then uploaded to the model via the `setSubsetRelRates` function. Note that we can assume that the second parameter (`fixed`) of the `setSubsetRelRates` function is `false` because, if this updater is in use, then subset relative rates cannot have been fixed by the user.
~~~~~~
{{ "steps/step-16/src/subset_relrate_updater.hpp" | polcodesnippet:"begin_pullFromModel-end_pushToModel","" }}
~~~~~~
{:.cpp}
