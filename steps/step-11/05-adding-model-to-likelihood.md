---
layout: steps
title: Adding the Model to the Likelihood class
partnum: 11
subpart: 5
description: Connect the Model class to the Likelihood class.
---
Before we can test the new `Model` class, we must integrate it into the `Likelihood` class.

Start by adding this include at the top of the {% indexfile likelihood.hpp %} file:
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"start-end_headers","a" }}
~~~~~~
{:.cpp}

## Add getModel and setModel member functions

Add these two declarations to the public section of the `Likelihood` class declaration:
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"getModel-setModel","" }}
~~~~~~
{:.cpp}

Here are the bodies of these two functions. It is assumed that the `setModel` function will only be called before BeagleLib is instantiated, hence the assertion that the length of the `_instances` vector is zero.
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"begin_getModel-end_setModel","" }}
~~~~~~
{:.cpp}

## Add an invarmodel data member to the InstanceInfo class

A data member `invarmodel` should be added to the declaration of the {% indexcode InstanceInfo %} struct in the private section of the `Likelihood` class declaration.
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"begin_InstanceInfo-end_InstanceInfo","b,c" }}
~~~~~~
{:.cpp}

This `bool` will keep track of whether the model for a particular instance has an extra zero-rate category for invariable sites.

Be sure to also initialize the new variable in the `InstanceInfo` constructor.

## New data member _model

Add a new data member `_model` to the private part of the `Likelihood` class declaration.
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"model_data_member","" }}
~~~~~~
{:.cpp}

Initialize `_model` in the `clear` function:
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"model_clear","" }}
~~~~~~
{:.cpp}

## Modify the initBeagleLib function

Add the `assert` highlighted in blue to ensure that `_model` has been assigned prior to  `initBeagleLib` being called.

Instead of assuming 1 rate category (i.e. equal rates), make the changes and additions shown in blue. Note that invariable sites models add an additional rate category, so different instances of BeagleLib would be needed to handle a 4-category GTR+G model versus a 4-category GTR+I+G model. We need some way to distinguish an I+G model from a G model having the same number of rate categories. The way this is done here is to make the number of rate categories negative for I+G models and positive for G models.  

The last change in blue simply adds information about invariable sites status when reporting on newly-created instances.
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"begin_initBeagleLib-end_initBeagleLib","d,e-f,g" }}
~~~~~~
{:.cpp}

## Modifications to the newInstance member function

If the incoming `nrates` is a negative number, it means that this is an invariable sites model, and the actual number of rate categories handled by BeagleLib should be the positive version of this, i.e. `-nrates`. The changes in blue below set the new `invarmodel` data member of the `InstanceInfo` struct accordingly and make the number of categories positive if the incoming `nrates` was negative.
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"begin_newInstance-end_newInstance","h-i,j" }}
~~~~~~
{:.cpp}

## Unconstrain the setTipPartials function
Remove the 2 lines below from `setTipPartials` because we now allow models to have different numbers of states. 
~~~~~~
{{ "steps/step-10/src/likelihood.hpp" | polcodesnippet:"begin_unconstrain-end_unconstrain","" }}
~~~~~~
{:.cpp}

## Simplify the setAmongSiteRateHeterogenetity function

The `setAmongSiteRateHeterogenetity` member function can be simplified now because most of the work can be done by member functions of the `Model` class. Replace the current version of `setAmongSiteRateHeterogenetity` with this version.
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"begin_setAmongSiteRateHeterogenetity-end_setAmongSiteRateHeterogenetity","" }}
~~~~~~
{:.cpp}

## Simplify setModelRateMatrix function

We can also rewrite `setModelRateMatrix` now using member functions of `Model`.
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"begin_setModelRateMatrix-end_setModelRateMatrix","" }}
~~~~~~
{:.cpp}

## Modify the defineOperations function to accommodate subset relative rates

It is possible to specify different relative rates for each partition subset. This allows the model to, for example, allow 3rd codon position sites to evolve at a faster rate than 1st or 2nd position sites. Subset-specific rates modify edge lengths. If a subset has a subset relative rate of 2, it is as if, for purposes of calculating the likelihood, the tree was twice as large as the tree read in from the file (i.e. every edge in the tree is twice as long as the corresponding edge in the tree file version). The `defineOperations` function is where edge lengths are used, and the following code snippet highlights the lines that need to change in order to use the subset relative rates stored by the model.
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"begin_defineOperations-ellipsis","k-kk,l" }}
~~~~~~
{:.cpp}

## Modify calcInstanceLogLikelihood to accommodate invariable sites models

Adding the capability to accommodate invariable sites in models adds some complexity to the calculation of the log likelihood. We cannot simply rely on BeagleLib to do all the work if an invariable sites model is in effect. (That's not exactly true, we could use BeagleLib for this, but doing so would be very inefficient as it would require full site likelihood calculations for the zero-rate case, which is a trivial calculation if done outside of BeagleLib). In the case of an I or I+G model, BeagleLib will handle everything except the zero-rate category. The code highlighted in blue below takes site log likelihoods calculated by BeagleLib and modifies them according to the proportion of invariable sites if an invariable sites model is being used.
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"begin_calcInstanceLogLikelihood-end_calcInstanceLogLikelihood","n-o" }}
~~~~~~
{:.cpp}

If the substitution rate is assumed to equal zero, then the likelihood is non-zero only if a site is constant because a variable site implies that at least one substitution occurred, which would be impossible if the rate were 0.0. Furthermore, we  can assume that the likelihood of the entire tree given the root state is simply 1.0 because all leaves will have the root state with probability 1.0 if the rate is 0.0. The likelihood of the tree under this zero-rate category is thus just the probability of the root state, which is just the equilibrium frequency of the root state. Remember that the `Data` class has a vector data member `_monomorphic` that stores the state present at every monomorphic pattern (and stores 0 for sites that are not even potentially constant). This allows us to calculate the invariable site component of the likelihood,` invar_like`, using this simple loop over states:
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"begin_states_loop-end_states_loop","" }}
~~~~~~
{:.cpp}

The overall site likelihood is calculated (using pseudocode) as follows:
~~~~~~
L = pinvar*invar_like + (1 - pinvar)*site_like
~~~~~~
{:.bash-output}

The site log likelihood is then:
~~~~~~
log(L) = log{pinvar*invar_like + (1 - pinvar)*site_like}
~~~~~~
{:.bash-output}

This creates a small predicament because BeagleLib provides log(`site_like`) for us, yet the formula above involves `site_like`, not log(`site_like`). If we remove the log by exponentiating log(`site_like`), we run afoul of the underflow problem. The trick is to factor out `(1 - pinvar)*site_like` before changing to log scale:
~~~~~~
L = pinvar*invar_like + (1 - pinvar)*site_like
  = (1 - pinvar)*site_like*(pinvar*invar_like/((1 - pinvar)*site_like) + 1)
~~~~~~
{:.bash-output}

Taking the log of both sides leads to the formula used in the code:
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"begin_sitelnL-end_sitelnL","" }}
~~~~~~
{:.cpp}

## Asserting that the model has been specified in calcLogLikelihood

In addition to asserting that `_data` exists, we should now also assert that `_model` exists before attempting to calculate the likelihood.
~~~~~~
{{ "steps/step-11/src/likelihood.hpp" | polcodesnippet:"begin_calcLogLikelihood-end_calcLogLikelihood","p" }}
~~~~~~
{:.cpp}

