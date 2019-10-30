---
layout: steps
title: The Model class
partnum: 11
subpart: 2
description: Create the Model class
---
By way of a data member (`_model`) of the `Likelihood` class, the `Model` class will supply BeagleLib with the eigenvectors, eigenvalues, and relative rates it needs in order to compute the likelihood of a tree. The [Eigen](http://eigen.tuxfamily.org/) library is used to compute the eigenvectors and eigenvalues for a given combination of exchangeabilities and nucleotide (or codon) frequencies.

The model class will manage BeagleLib instances: if subsets differ in the number of states or number of rate categories, different BeagleLib instances must be created for each combination. The `Model` class will thus be our BeagleLib manager.

Begin by creating a new header file named {% indexfile model.hpp %} to contain the {% indexcode Model %} class declaration and member function bodies:
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## Constructor, destructor, and clear functions

The constructor and destructor are very simple, and are similar to most of the other constructors and destructors you have created thus far. 
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_constructor-end_clear","" }}
~~~~~~
{:.cpp}

## The describeModel member function

This function reveals the current state of the model. It constructs and returns a string, which can be output to either `std::cout` or to a file. 

This function will probably appear overly long and complicated to you! It could be made much shorted if it simply regurgitated the model specification provided by the user, but this function does a lot of sanity checking, reporting features of the model as it actually exists in memory. If this description differs from the model specified in the {% indexfile strom.conf %} file, then it is a clear sign that there is a bug. Later this function will be used to enumerate free model parameters that need updating during an MCMC analysis, so it could be argued that the name `describeModel` falls short of fully describing its significance.

Some explanation of how this function works follows the code.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_describeModel-end_describeModel","" }}
~~~~~~
{:.cpp}
Each row of output is stored as one element of the map `ss`, which has keys that are descriptive of the information in that particular row.

The table labeled "Partition information" is fairly straightforward. The number of sites, patterns, number of states, and number of (discrete gamma) rate categories for each subset are appended to the appropriate element of `ss` using `boost::format` to ensure they are all right-justified.

The second table shows how many unique subset relative rates, state frequency vectors, exchangeability vectors, omegas, rate variances, and proportions of invariable sites are defined in the model. This is somewhat more tricky to determine, and rather than just use the information supplied by the user in the configuration file, this function _determines_ actual linkage of parameters across subsets as a sanity check to make sure that the model is set up as the user intended.

To do this, 6 {% indexcode std::set %} variables are created (`freqset`, `xchgset`, `omegaset`, `ratevarset`, `pinvarset`, and `relrateset`). I'll use exchangeabilities to illustrate, as they are one of the more interesting parameters defined in this example, being shared across 2 subsets and distinct for the 3rd. Here is the relevant code. This is part of a loop over subsets, where `i` is the current subset index:
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_uniq_xchg-end_uniq_xchg","" }}
~~~~~~
{:.cpp}
First, the reference `xchg` is assigned to the exchangeabilities for subset `i`. In the example model specification used for this step, the configuration file contains these lines pertaining to the assignment of exchangeabilities across subsets:
~~~~~~
{{ "steps/step-11/test/strom.conf" | polcodesnippet:"begin_xchg-end_xchg","" }}
~~~~~~
{:.bash-output}
This specifies that the exact same exchangeabilities should be used for both subsets `first` and `second`, while a distinct set of exchangeabilities should be used for subset `third`. This means that the shared pointers to exchangeabilities stored in `_qmatrix[0]` and `_qmatrix[1]` should point to the same array in memory, and the shared pointer to the exchangeabilities stored in `_qmatrix[2]` should point to a different memory location. When visiting subset `i`, we find the address of the exchangeability array used for that subset (`xchg_addr`) and insert that address into the set `xchgset`. The return value of the `std::set::insert` function is a `std::pair`, the first element of which is an iterator positioned at the inserted element and the second of which is true if an insertion took place, false otherwise. If an insertion took place, it means that the memory address `xchg_addr` is distinct, and we thus push `xchg_addr` onto the end of the vector `unique_xchg`, which holds the distinct memory addresses of each unique exchangeability vector in the order in which it was discovered when iterating through subsets. If an insertion did not take place, it means that the memory address `xchg_addr` was not distinct. In this case, the index of that address in the `unique_xchg` vector is found. In either case, the index of the element in the `unique_xchg` vector (plus 1) serves as the value stored for subset `i` in the row corresponding to exchangeabilities.

Note that this approach will not attribute the same index to two different subsets if only the exchangeability _values_ are identical; it is the _memory address_ of the first element of the exchangeability vector that matters. It is possible for two subsets to have _potentially_ different exchangeability vectors but not be distinct at the moment this function was called (for example, it is quite possible that exchangeability vectors for every subset are allowed to differ but were all initialized to `1,1,1,1,1,1` at the start of the program).

Don't fret that you don't know about the `ASRV` and `QMatrix` classes, those will be introduced in coming sections. For now just think of them as keepers of parameters and information related to Among-Site Rate Variation (`ASRV`) and the instantaneous rate matrix (`QMatrix`).

## Accessors

These 8 functions (`getNumPatterns`, `getSubsetNumSites`, `getNumSites`, `getNumSubsets`, `getSubsetNumCateg`, `getSubsetIsInvarModel`, `getQMatrix`, and `getASRV`) simply provide a way to peek at the values of private data members.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_getNumPatterns-end_getASRV","" }}
~~~~~~
{:.cpp}

## The calcNormalizingConstantForSubsetRelRates member function

This function calculates and returns the sum of the products of subset proportions and subset relative rates. The subset relative rates must be normalized so that they don't affect the mean rate, which is determined by the edge lengths of the tree. To compute the normalization constant, we compute the sum of the products of subset proportions and subset relative rates. For example, for 3 subsets,
~~~~~~
normalizing_constant = p1*r1 + p2*r2 +p3*r3
~~~~~~
{:.bash-output}
Dividing each relative rate by this sum of products ensures that the sum of products, if computed again, would be equal to 1.0 (which is the goal).
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_calcNormalizingConstantForSubsetRelRates-end_calcNormalizingConstantForSubsetRelRates","" }}
~~~~~~
{:.cpp}

## The getSubsetSizes member function
This function returns a reference to the vector of subset sizes, `_subset_sizes`.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_getSubsetSizes-end_getSubsetSizes","" }}
~~~~~~
{:.cpp}

## The setSubsetSizes member function

This function uses {% indexcode std::copy %} to copy the subset sizes (i.e. the number of sites in each subset, supplied via the `nsites_vect` parameter) into the `_subset_sizes` data member. It then uses {% indexcode std::accumulate %} to sum all subset sizes and store the result in the data member `_num_sites`.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setSubsetSizes-end_setSubsetSizes","" }}
~~~~~~
{:.cpp}

## The setSubsetNumPatterns member function

This function copies the supplied vector of pattern counts (`npatterns_vect`) to the data member `_subset_npatterns` using {% indexcode std::copy %}. The subset pattern counts are not needed by model; they are only supplied to the model to allow the model to print them out in the `describeModel` member function.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setSubsetNumPatterns-end_setSubsetNumPatterns","" }}
~~~~~~
{:.cpp}

## The setSubsetDataTypes member function

This function should be called as soon as the data types associated with each partition subset are known. The function sets the `_num_subsets` data member to the number of partition subsets and resizes the `_asrv`, `_qmatrix`, and `_subset_datatypes` vectors to have length `_num_subsets`. The `datatype_vect` parameter is copied (using {% indexcode std::copy %}) to the `_subset_datatypes` vector and each of the `_num_subsets` elements of `_asrv` and `_qmatrix` are initialized with new `ASRV` or `QMatrixNucleotide`/`QMatrixCodon` elements, respectively.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setSubsetDataTypes-end_setSubsetDataTypes","" }}
~~~~~~
{:.cpp}

## The setSubsetNumCateg member function

This function sets the number of among-site rate categories. Everything about rate heterogeneity is handled by the `ASRV` data member, so, after performing a couple of sanity checks, the `setNumCateg` member function of the `_asrv` data member is called upon to store this value for the `Model`.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setSubsetNumCateg-end_setSubsetNumCateg","" }}
~~~~~~
{:.cpp}

## The setSubsetRateVar member function

This function sets the variance of among-site rates. Everything about among-site rate heterogeneity is handled by the `ASRV` data member, so, after performing a couple of sanity checks, the `setRateVarSharedPtr` member function of the `_asrv` data member is called upon to store this value for the `Model`. Note that `ratevar` is a `shared_ptr` rather than a `double` value, so it must be dereferenced using an asterisk in order to check whether the user has supplied a valid value. The `fixed` parameter determines whether this rate variance parameter will be updated during a subsequent analysis (e.g. MCMC).
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setSubsetRateVar-end_setSubsetRateVar","" }}
~~~~~~
{:.cpp}

## The setSubsetPinvar member function

This function sets the proportion of invariable sites. Everything about among-site rate heterogeneity is handled by the `ASRV` data member, so, after performing sanity checks, the `setPinvarSharedPtr` member function of the `_asrv` data member is called upon to store this value for the `Model`. Note that `pinvar` is a `shared_ptr` rather than a `double` value, so it must be dereferenced using an asterisk in order to check whether the user has supplied a valid value. The `fixed` parameter determines whether this proportion of invariable site parameter will be updated during a subsequent analysis (e.g. MCMC).
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setSubsetPinvar-end_setSubsetPinvar","" }}
~~~~~~
{:.cpp}

## The setSubsetIsInvarModel member function

The `is_invar` parameter of this function determines whether this model will be an invariable sites model for the partition subset indicated.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setSubsetIsInvarModel-end_setSubsetIsInvarModel","" }}
~~~~~~
{:.cpp}

## The setSubsetExchangeabilities member function

This function sets the exchangeabilities for a particular subset. The exchangeabilities are normalized to make sure they sum to 1 and sent to the `QMatrix` object pointed to by the data member `_qmatrix`, which is responsible for all data related to calculating the instantaneous rate matrix and its eigenvalues/eigenvectors.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setSubsetExchangeabilities-end_setSubsetExchangeabilities","" }}
~~~~~~
{:.cpp}

## The setSubsetStateFreqs member function

This function sets the state frequencies for a particular subset. The frequencies are sent to the `QMatrix` object pointed to by the data member `_qmatrix`, which is responsible for all data related to calculating the instantaneous rate matrix and its eigenvalues/eigenvectors. It is possible that the user has specified the keyword `equal` for the state frequencies in the config file, in which case the first element of the parameter `state_frequencies` will be equal to -1. If that is the case, the `QMatrix::setEqualStateFreqs` member function is called instead of the `QMatrix::setStateFreqsSharedPtr` member function.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setSubsetStateFreqs-end_setSubsetStateFreqs","" }}
~~~~~~
{:.cpp}

## The setSubsetOmega member function

This function sets the omega parameter for a particular subset. The `omega` parameter represents the nonsynonymous/synonymous rate ratio used in codon models. The parameter `omega` is a shared pointer, hence the need for indirection using the asterisk in order to assert that its value is valid.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setSubsetOmega-end_setSubsetOmega","" }}
~~~~~~
{:.cpp}

## The setTreeIndex, getTreeIndex, and isFixedTree member functions

The `setTreeIndex` function allows for telling the model which tree is to be used to compute the likelihood (or starting likelihood in the case of an MCMC analysis). The `getTreeIndex` can be used to determine which tree index is currently held by the model, and `isFixedTree` returns whether the tree is considered fixed (which only makes sense in the context of an MCMC analysis).
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setTreeIndex-end_isFixedTree","" }}
~~~~~~
{:.cpp}

## The setSubsetRelRates member function

This function sets the elements of the `_subset_relrates` vector according to the values specified by the user. If the user specified "equal" then the first element of the `relrates` vector will equal -1. In this case, the relative rates for all subsets are set to the value 1.0. If the user specified that the subset relative rates should be fixed at the specified values, then the data member `_subset_relrates_fixed` will be set to true.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setSubsetRelRates-end_setSubsetRelRates","" }}
~~~~~~
{:.cpp}

## The getSubsetRelRates member function

This accessor returns a reference to the `_subset_relrates` vector.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_getSubsetRelRates-end_getSubsetRelRates","" }}
~~~~~~
{:.cpp}

## The isFixedSubsetRelRates member function
This accessor returns the boolean value of the data member `_subset_relrates_fixed`.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_isFixedSubsetRelRates-end_isFixedSubsetRelRates","" }}
~~~~~~
{:.cpp}

## The activate and inactivate member functions

This function is useful during model initiation: it is unnecessary to recalculate the eigensystem for the rate matrix when only part of the rate matrix has been specified. Hence the model should begin life in an inactivated state and the `activate` function can be called when it is set up and ready to go.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_activate-end_inactivate","" }}
~~~~~~
{:.cpp}

## The setBeagleEigenDecomposition member function

This function provides eigenvalues, eigenvectors, and inverse eigenvectors to BeagleLib for use in computing transition probabilities.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setBeagleEigenDecomposition-end_setBeagleEigenDecomposition","" }}
~~~~~~
{:.cpp}

## The setBeagleStateFrequencies member function

This function provides state frequencies to BeagleLib for use in computing transition probabilities and the log-likelihood.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setBeagleStateFrequencies-end_setBeagleStateFrequencies","" }}
~~~~~~
{:.cpp}

## The setBeagleAmongSiteRateVariationRates member function

This function provides the rates component of the among-site rate heterogeneity submodel to BeagleLib.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setBeagleAmongSiteRateVariationRates-end_setBeagleAmongSiteRateVariationRates","" }}
~~~~~~
{:.cpp}

## The setBeagleAmongSiteRateVariationProbs member function

This function sends the vector of category weights to BeagleLib. The category weights are the probabilities of each category in the among-site rate heterogeneity model.
~~~~~~
{{ "steps/step-11/src/model.hpp" | polcodesnippet:"begin_setBeagleAmongSiteRateVariationProbs-end_setBeagleAmongSiteRateVariationProbs","" }}
~~~~~~
{:.cpp}
