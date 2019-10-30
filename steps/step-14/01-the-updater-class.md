---
layout: steps
title: The Updater Class
partnum: 14
subpart: 1
description: Create the Updater class that will serve as the base class for classes that update model parameters
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

The `Updater` class is responsible for updating one or more unknown parts of the model. This class will be a base class for other classes that update particular parts of the model. A base class has virtual functions that may be replaced by functions of the same name and parameter signature in classes that are derived from this base class. The member function `update` is an example of a virtual function. The version of `update` created in `Updater` will work for most things, but it is designated `virtual` so that derived classes can replace it with something different if the general function does not work for the kind of update they are performing.

Some virtual functions are not provided with a function body in this class. These may be spotted because they end with `= 0` in the class declaration. The presence of these {% indexbold pure virtual functions %} make this base class an {% indexbold abstract base class %}: you cannot create an object (variable) of type `Updater` because some of its functions are not defined. (You have seen abstract base classes before in this tutorial: e.g. {% indexcode QMatrix %}.) Why create an abstract class? These pure virtual functions are so specific to the update being performed that they must necessarily be different in every class derived from this base class. Making them pure virtual functions ensures that these specific replacements will be written (otherwise the program will not compile). Abstract base classes provide functionality that is common to a collection of derived classes, making the job of writing the derived classes easier because you need to focus only on the differences, not the similarities.

Create a new file {% indexfile updater.hpp %} and replace the default contents with the following class declaration.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## Constructor and destructor

The constructor delegates all its initialization work to the `clear` function, and the destructor does nothing.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The clear member function

This function is used by the constructor (or called on its own) to reset an `Updater` object to the same state it had when first constructed. Note that this function is designated virtual in the class declaration so that a derived class can substitute a version that specifically clears data members it has defined that are out of reach of the base class clear function. It is not a good idea to call this function often, because clear will erase potentially important information (e.g. the `_prior_parameters` vector) that is important for updating a parameter value. Instead, you will normally call the `reset` function to get the updater ready for the next parameter update.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_clear-end_clear","" }}
~~~~~~
{:.cpp}

## The reset member function

The reset (virtual) function is used to provide a way to initialize values that need to be set to zero (for example) before a new parameter update is attempted.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_reset-end_reset","" }}
~~~~~~
{:.cpp}

## The setLikelihood member function

This function provides a way to equip this updater with a Likelihood object that can be used to compute the likelihood for proposed parameter values.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_setLikelihood-end_setLikelihood","" }}
~~~~~~
{:.cpp}

## The setTreeManip member function

This function allows you to give this updater a `TreeManip` object that can be used to change the current tree. This is not important for updating many parameters (e.g. base frequencies), but would be needed for an updater whose job is to update the tree or edge lengths.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_setTreeManip-end_setTreeManip","" }}
~~~~~~
{:.cpp}

## The getTreeManip member function

This accessor function merely returns a pointer to the `TreeManip` object currently assigned to this object.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_getTreeManip-end_getTreeManip","" }}
~~~~~~
{:.cpp}

## The setLot member function

Every parameter update will involve choices that are mediated by random numbers, so every updater will need a pseudorandom number generator. This function provides a way to assign a `Lot` object to this updater object. Generally, a single pseudorandom number generating `Lot` object will be used for all updaters to ensure that an analysis is repeatable, so each updater object will receive a pointer to the same Lot object.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_setLot-end_setLot","" }}
~~~~~~
{:.cpp}

## The setHeatingPower member function

For MCMC analyses involving multiple chains, only one chain explores the actual posterior distribution, while all others explore a posterior density surface that is heated by raising the density to a power between 0.0 and 1.0. This function allows the heating power used by this updater to be set. Ordinarily, all updaters assigned to a single MCMC Chain object will be assigned the same heating power (i.e. a chain object will call the `setHeatingPower` function for every `Updater` and provide the same value of `p` for all).
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_setHeatingPower-end_setHeatingPower","" }}
~~~~~~
{:.cpp}

## The setLambda member function

Every updater needs a way to determine the boldness of its proposed changes, and this function is used to set this single parameter `_lambda`. Larger values of `_lambda` cause proposed new values to be, on average, farther away from the current value than smaller values of `_lambda`. Taking proposed steps that are too large leads to low acceptance rates, and taking proposed steps that are too small leads to really high acceptance rates (which is not necessarily desirable because such "baby steps" do not explore the posterior very efficiently). The function `tune` below automatically adjusts `_lambda` so that proposals are accepted at the desired rate, but this function may be used to set it explicitly.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_setLambda-end_setLambda","" }}
~~~~~~
{:.cpp}

## The setTuning member function

If `true` is supplied to this function, then the function `tune` will be called after each update to modify the boldness of the proposal distribution for this updater. Supplying `false` causes `tune` to not be called during an update. Tuning should only be done during burn-in, so this function is needed to provide a way to turn tuning on or off.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_setTuning-end_setTuning","" }}
~~~~~~
{:.cpp}

## The tune member function

This is the function that adjusts (tunes) `_lambda` so that proposals are just bold enough to be accepted the desired proportion of the time. The only parameter is boolean: supply `true` if the most recent update was accepted and `false` otherwise. Note that the data member `_nattempts` is updated by this function, even if tuning is currently turned off. The theory behind this function was provided by Prokaj, V. (2009). (Thanks to Dave Swofford for pointing out this simple method to me.)
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_tune-end_tune","" }}
~~~~~~
{:.cpp}

## The setTargetAcceptanceRate member function

This function provides a way for you to tell the updater the desired acceptance rate (the fraction of updates that should result in a successful change to the parameter value). This value is used by the Prokaj algorithm in the `tune` function.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_setTargetAcceptanceRate-end_setTargetAcceptanceRate","" }}
~~~~~~
{:.cpp}

## The setPriorParameters member function

Every updater is responsible for modifying a single parameter or jointly updating a group of related parameters, and in order to calculate the posterior kernel, it needs to know how to calculate the prior for any particular value of its parameter (or parameter vector). This function provides a generic way to supply prior distribution parameter values to be used in calculating the prior probability (density). For example, if the supplied vector `c` had length 2, and the parameter being updated has been assigned a Gamma prior distribution, then `c[0]` would be the shape parameter and `c[1]` the scale parameter of the Gamma prior. If, on the other hand, if the parameter being updated is a vector of 4 state frequencies, then `c` might have length 4 and provide the four parameters of a Dirichlet prior distribution for state frequencies. Each Updater interprets what is passed to this function in a potentially different way depending on the prior distribution assigned to the parameter it is in charge of updating.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_setPriorParameters-end_setPriorParameters","" }}
~~~~~~
{:.cpp}

## The setWeight member function

This function sets the `_weight` data member to the supplied value. The `_weight` represents the unnormalized probability that the updater will be called in any given generation.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_setWeight-end_setWeight","" }}
~~~~~~
{:.cpp}

## The calcProb member function

This function computes a new value for the data member `_prob` by dividing the `_weight` by the sum of all updater weights, which is the value passed into the function.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_calcProb-end_calcProb","" }}
~~~~~~
{:.cpp}

## The getLambda member function

This function simply returns the current value of `_lambda`, which is the data member that determines the boldness of proposals that update the parameter or parameters managed by this `Updater` object.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_getLambda-end_getLambda","" }}
~~~~~~
{:.cpp}

## The getProb member function

This function returns the current value of `_prob`.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_getProb-end_getProb","" }}
~~~~~~
{:.cpp}

## The getWeight member function

This function returns the current value of `_weight`.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_getWeight-end_getWeight","" }}
~~~~~~
{:.cpp}

## The getAcceptPct member function

This function returns the percentage of attempted updates that were accepted, unless no updates have been attempted since the last call to the `clear` or `setTuning` functions, both of which reset `_nattempts` and `_naccepts` to 0.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_getAcceptPct-end_getAcceptPct","" }}
~~~~~~
{:.cpp}

## The getNumUpdates member function

This function returns the value of the data member `_nattempts`, which is incremented each time this updater's `update` function is called.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_getNumUpdates-end_getNumUpdates","" }}
~~~~~~
{:.cpp}

## The getUpdaterName member function

This accessor function returns the name given to this `Updater` object, which is stored in the `_name` data member.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_getUpdaterName-end_getUpdaterName","" }}
~~~~~~
{:.cpp}

## The calcLogLikelihood member function

This function returns the log likelihood of the current model. The log likelihood is needed after proposing a new parameter value as it determines (together with the log joint prior) whether the proposed value can be accepted.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_calcLogLikelihood-end_calcLogLikelihood","" }}
~~~~~~
{:.cpp}

## The update member function

This is the function called when a parameter update is desired. This function calls the pure virtual function `proposeNewState` to propose a new parameter, decides whether to accept the proposed value, and calls `revert` if the proposed value is not accepted. The `update` function returns the current log likelihood, which will only differ from `prev_lnL` if the proposed value is accepted.

### Selecting and flipping partials and transition matrices

The `TreeManip` functions `deselectAllPartials` and `deselectAllTMatrices` are called before `proposeNewState`. This cleans the slate, so to speak. During {% indexfile proposeNewState %}, the `selectPartial` function will be called for nodes whose partials need to be recalculated due to a change in one of its descendant's edge lengths (or for all nodes if a global model parameter changes). Likewise, the `selectTMatrix` function will be called for any node whose edge length changes (or all nodes if a global model parameter changes).

Before the likelihood is calculated, the `TreeManip::flipPartialsAndTMatrices` function is called. This causes the partials and transition matrices that are recalculated to use an alternate memory location. If it turns out that the proposed change is not accepted, we need only call `flipPartialsAndTMatrices` again to return all those expensive-to-calculate partials and transition matrices to their previous state. This requires us to tell BeagleLib to allocate twice as much memory as we actually need, but the speed improvement is well worth the expense (remember that rejection of proposed moves is generally more common than acceptance).

### The log of zero

The quantity `_log_zero` requires some explanation. You can see by looking at the class declaration above that this is a static data member of the `Updater` class. Static data members exist even if no object of the class has been created, and thus static data members must be initialized outside of the constructor for the class (the constructor is only called if you are creating an object of type `Updater`). The value of `_log_zero` is initialized in the {% indexfile main.cpp %} source code file (as you will see when we create {% indexfile main.cpp %} for this step).

But, you might ask, why do we need `_log_zero` anyway? We will initialize this data member to have a value equal to the most negative number that can be stored. We need this if a proposed new state is impossible under the model. If the `log_prior` is minus infinity, it means that the proposed state has zero prior probability and should be rejected because, presumably, the current state has probability density > 0. Ideally, the code in `proposeNewState` should be written in such a way that only valid states are proposed, so (ideally) this check is not necessary.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_update-end_update","" }}
~~~~~~
{:.cpp}

## The calcEdgeLengthPrior member function

This program uses the Gamma-Dirichlet prior proposed by Rannala, Zhu, and Yang (2012), and this function calculates that prior. This approach specifies a Gamma prior distribution for tree length (TL) and the first two values in the `_prior_parameters` vector are expected to be the shape and scale of that Gamma distribution. The Dirichlet part specifies the prior distribution for the edge length proportions (not edge lengths). The third element in the `_prior_parameters` vector specifies the parameter of this symmetric Dirichlet prior distribution (normally this value is 1 so that the prior is flat and edge lengths are allowed to do whatever they please so long as they add up to TL).

We are adding this function to `Updater` because edge lengths will be proposed by more than one updater. For example, there will be an updater responsible for changing just the TL (rescaling the entire tree) to improve MCMC mixing, and this updater will be separate from an updater that modifies the topology and some edge lengths. Having this function reside in `Updater` means that we won’t have to implement this function multiple times in different derived classes.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_calcEdgeLengthPrior-end_calcEdgeLengthPrior","" }}
~~~~~~
{:.cpp}

## The getLogZero static member function

This is a static member function that returns the value stored in the static data member `_log_zero`. The fact that it is declared static means that this function can be called even if no `Updater` object exists. The value returned is the most negative floating point value that can be stored in the computer. This value stands in for the log of zero, which is really negative infinity, which is not a number and therefore cannot be stored in a computer. This value is used primarily when a parameter value is proposed outside of the support of its prior probability distribution. The prior probability is in this case zero, and the log prior is -infinity. Because we cannot represent -infinity, we instead use `_log_zero`, which is at least guaranteed to be as small or smaller (in log scale) as any valid prior probability.
~~~~~~
{{ "steps/step-14/src/updater.hpp" | polcodesnippet:"begin_getLogZero-end_getLogZero","" }}
~~~~~~
{:.cpp}

## Literature Cited

V Prokaj. 2009. Proposal selection for MCMC simulation. pp. 61–65 in: Applied Stochastic Models and Data Analysis. XIII international conference on applied stochastic models and data analysis. Vilnius, Lithuania.

B Rannala, T Zhu, and Z Yang. 2012. Tail paradox, partial identifiability, and influential priors in Bayesian branch length inference. Molecular Biology and Evolution. 29:325–335. [DOI: 10.1093/molbev/msr210](https://doi.org/10.1093/molbev/msr210)
