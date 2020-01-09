---
layout: steps
title: The ASRV class
partnum: 11
subpart: 4
description: Create the ASRV class
---
The `ASRV` class handles everything associated with among-site rate heterogeneity for the `Model`. The `Model` has only to provide the variance among rates and the `ASRV` object recalculates the rate category boundaries and discrete gamma rate category means. Hiding the details of rate heterogeneity in this way makes it easy to later swap in a different version of `ASRV` that does not use the discrete gamma distribution to model among-site rate heterogeneity.

Begin by creating a new header file named {% indexfile asrv.hpp %} to contain the `ASRV` class declaration and member function bodies:
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## The constructor, destructor, and clear function
The constructor and destructor are routine, and the `clear` function simply sets the number of rate categories to 1 (i.e. rate homogeneity is the default).
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_constructor-end_clear","" }}
~~~~~~
{:.cpp}

## The getRateVarSharedPtr member function
Returns the shared pointer `_ratevar`, which points to the double value storing the variance of rates across sites.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_getRateVarSharedPtr-end_getRateVarSharedPtr","" }}
~~~~~~
{:.cpp}

## The getRateVar member function
Returns the double value stored at the shared pointer `_ratevar`, which represents the variance of rates across sites.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_getRateVar-end_getRateVar","" }}
~~~~~~
{:.cpp}

## The getPinvarSharedPtr member function
Returns the shared pointer `_pinvar`, which points to the double value storing the proportion of invariable sites.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_getPinvarSharedPtr-end_getPinvarSharedPtr","" }}
~~~~~~
{:.cpp}

## The getPinvar member function
Returns the double value at the shared pointer `_pinvar`, which represents the proportion of invariable sites.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_getPinvar-end_getPinvar","" }}
~~~~~~
{:.cpp}

## The getRates member function
Returns a pointer to the array of rates stored in the vector `_rates`.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_getRates-end_getRates","" }}
~~~~~~
{:.cpp}

## The getProbs member function
Returns a pointer to the array of rate probabilities stored in the vector `_probs`.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_getProbs-end_getProbs","" }}
~~~~~~
{:.cpp}

## The getIsInvarModel member function
Returns the value of  `_invar_model`. which determines whether this is an invariable sites model.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_getIsInvarModel-end_getIsInvarModel","" }}
~~~~~~
{:.cpp}

## The getNumCateg member function
Returns the number of rate categories, `_num_categ`.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_getNumCateg-end_getNumCateg","" }}
~~~~~~
{:.cpp}

## The setNumCateg member function
This function sets the number of rate categories (`_num_categ`) and calls `recalcASRV` to recompute rate category boundaries, rate category probabilities, and category mean rates.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_setNumCateg-end_setNumCateg","" }}
~~~~~~
{:.cpp}

## The setRateVarSharedPtr member function
This function assigns a shared pointer to the variance of rates across sites to the data member `_ratevar`.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_setRateVarSharedPtr-end_setRateVarSharedPtr","" }}
~~~~~~
{:.cpp}

## The setRateVar member function
This function sets the value of the variance of rates across sites, pointed to by the data member `_ratevar`.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_setRateVar-end_setRateVar","" }}
~~~~~~
{:.cpp}

## The setPinvarSharedPtr member function
This function assigns a shared pointer to the proportion of invariable sites to the data member `_pinvar`.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_setPinvarSharedPtr-end_setPinvarSharedPtr","" }}
~~~~~~
{:.cpp}

## The setPinvar member function
This function sets the value of the proportion of invariable sites, pointed to by the  data member `_pinvar`.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_setPinvar-end_setPinvar","" }}
~~~~~~
{:.cpp}

## The setIsInvarModel member function
This function sets the `_invar_model` data member, a bool value that determines whether this `ASRV` object will represent the invariable sites model.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_setIsInvarModel-end_setIsInvarModel","" }}
~~~~~~
{:.cpp}

## The fixRateVar, fixPinvar, isFixedRateVar, and isFixedPinvar member functions
The first 2 of these functions set the `_ratevar_fixed` and `_pinvar_fixed` data members, respectively. Each stores a bool value that determines whether their particular model parameter will be fixed or allowed to vary in, say, an MCMC analysis. The second set of 2 functions serve as accessors for these bool values.
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_fixRateVar-end_isFixedPinvar","" }}
~~~~~~
{:.cpp}

## The recalcASRV member function
### Discrete Gamma Rate Heterogeneity (+G model)
This function recomputes several vectors related to modeling discrete-gamma among-site rate variation. The relative rate of a nucleotide site is the ratio of its rate of substitution to the average rate. A relative rate equal to 1.0 means that the site is evolving at exactly the average rate, while a relative rate of 2.0 means that site is evolving at twice the average rate. Relative rates are assumed to follow a Gamma distribution with mean 1.0 and shape `1/_ratevar`. 

A Gamma(&alpha;,&beta;) distribution having shape &alpha; and scale &beta; has mean &alpha;&beta;. Because the mean relative rate should be 1.0, this implies that &beta; = 1/&alpha;, so the scale parameter &alpha; completely determines the Gamma distribution. Because the variance of a Gamma(&alpha;,&beta;) distribution is &alpha; &beta;<sup>2</sup> = &alpha;(1/&alpha;)(1/&alpha;) = 1/&alpha;, it makes sense to specify the variance of rates across sites (`_ratevar`) and determine the shape parameter &alpha; as `1/_ratevar`. Rate heterogeneity is thus proportional to `_ratevar`, and `_ratevar` equal to zero corresponds to no rate heterogeneity.

The Gamma distribution is partitioned into `_num_categ` subsets having equal area under the probability density curve (and thus equal probability), and the mean of each category serves as the relative rate representing that category. Rate heterogeneity is thus modeled as a discrete mixture distribution with each component of the mixture having equal probability (i.e. representing the same proportion of sites).

The mean of one category is computed as follows:

![Q matrix for the GTR model]({{ "/assets/img/rateMean.png" | absolute_url }}){:.center-math-image}

This involves a ratio of integrals of two Gamma distributions. The Gamma distribution in the numerator has shape &alpha; + 1, while the Gamma distribution in the denominator has shape &alpha;, and both have scale &beta; = 1/&alpha;. The values _u_ and _v_ define the boundaries of the category for which the mean is being calculated. Calcluation of these two integrals is done using the quantile and cdf functions for the Gamma distribution provided by the [Boost Math library](https://www.boost.org/doc/libs/1_65_1/libs/math/doc/html/index.html).

### Invariable Sites (+I model)
The `ASRV` class also allows a rate heterogeneity model to allow a fraction of sites (determined by the proportion of invariable sites `_pinvar` parameter) to be invariable (rate of substitution equal to zero). Adding invariable sites capability involves adding an additional rate (equal to 0.0) and and additional rate category (having probability pinvar). The remaining `_ncateg` categories no longer have probability equal to `1/_ncateg` because their sum must now equal 1 - pinvar. Making the probability of each non-zero rate category equal to (1-pinvar)/ncateg accomplishes this.

Here is the code for handling calculation of the rates and probabilities under the +G or +I+G models:
~~~~~~
{{ "steps/step-11/src/asrv.hpp" | polcodesnippet:"begin_recalcASRV-end_recalcASRV","" }}
~~~~~~
{:.cpp}


