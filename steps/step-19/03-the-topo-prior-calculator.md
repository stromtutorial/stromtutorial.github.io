---
layout: steps
title: The TopoPriorCalculator class
partnum: 19
subpart: 3
description: Create the TopoPriorCalculator class for computing the topology prior.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

With the introduction of polytomous trees, the computation of the tree topology prior becomes more interesting but also much more complicated. We now need to not only know the number of binary tree topologies (the highest resolution class), we need to know the number of tree topologies in all the other resolution classes.

There are also at least two interesting variations on the concept of a flat prior with respect to tree topology. The prior could be flat across all tree topologies, or it could be flat across resolution classes. In the first case, the star tree and any particular fully-resolved tree topology have the same prior probability. In the second case, the star tree (which is the only representative of its resolution class), gets much greater weight than any particular fully-resolved tree topology. For example, with just 10 taxa, the star tree would have (in the flat resolution class prior) a prior probabilility 2,027,025 times that of any resolved tree!

To help us compute the topology prior, create the following class `TopoPriorCalculator` in a header file named {% indexfile topo_prior_calculator.hpp %} as follows:
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"start-end_class_declaration,end","h" }}
~~~~~~
{:.cpp}

## The TopoPriorCalculator constructor and destructor

The constructor sets `_is_rooted` to false, `_is_resolution_class_prior` to true, `_C` to 1.0, `_ntax` to 4, and `_topo_priors_dirty` to true. The destructor clears the vectors `_counts`, `_nfactors` and `_topology_prior`.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The getLogNormalizedTopologyPrior member function

This is the function that delivers what you need: the log of the prior probability of a tree topology having `m` internal nodes. This represents the _resolution class prior_ if
`_is_resolution_class_prior` is true, otherwise it represents the _polytomy prior_. 

A flat **resolution class prior** places equal prior probability mass on every possible value of `m`. For example, for `N=7` taxa, the resolution class variable `m` can range from 1 (star tree) to `N-2 = 5` (fully-resolved tree) for the unrooted tree case. The prior probability of each resolution class is thus 0.2 and a particular tree in each class equals 0.2 divided by the number of distinct labeled tree topologies in that class:

|   m   |  ntopo |         ptree          | ntopo*ptree |
| :---: | :----: | :--------------------: | :---------: |
|   1   |     1  | 0.2/1    = 0.200000000 |     0.2     |
|   2   |    56  | 0.2/56   = 0.003571429 |     0.2     |
|   3   |   490  | 0.2/490  = 0.000408163 |     0.2     |
|   4   |  1260  | 0.2/1260 = 0.00015873  |     0.2     |
|   5   |   945  | 0.2/945  = 0.00021164  |     0.2     |
| total |  2752  |                        |     1.0     |

where `m` is the resolution class (number of internal nodes), `ntopo` is the number of tree topologies in resolution class `m` (computed in the `recalcCountsAndPriorsImpl` function), and `ptree` is the probability of any given labeled tree topology in resolution class `m`. 

The flat **polytomy prior** is simpler: it assumes the same prior probability for each tree, regardless of the resolution class. Because the total number of tree topologies over all resolution class is `1+56+490+1260+945 = 2752` in the 7-taxon example above, the prior for each tree would be `1/2752 = 0.000363372` under the flat polytomy prior.

The priors described above were flat priors. It is possible to make some resolution classes more probable in a resolution class prior, and trees with more or fewer internal nodes more probable in a polytomy prior. For example, suppose you wished resolution class `m = 1` (the star tree) to be twice as probable as the class of trees having 2 internal nodes (`m = 2`), class `m = 2` to be twice as probable as class `m = 3`, and so on. The data member `_C` determines the relative probability of resolution class `m` over resolution class `m+1`. Setting `_C = 2` yields this resolution class prior distribution:

|   m   |  ntopo |            ptree             | ntopo*ptree |
| :---: | :----: | :--------------------------: | :---------: |
|   1   |     1  | (16/31)/1    = 0.516129032   | 0.516129032 |
|   2   |    56  |  (8/31)/56   = 0.004608295   | 0.258064516 |
|   3   |   490  |  (4/31)/490  = 0.000263331   | 0.129032258 |
|   4   |  1260  |  (2/31)/1260 = 0.000051203   | 0.064516129 |
|   5   |   945  |  (1/31)/945  = 0.000034136   | 0.032258065 |
| total |  2752  |                              | 1.0         |

The data member `_C` in the case of a _polytomy_ prior ensures that the probability of any particular tree in resolution class `m` is `_C` times more probable than any particular tree in resolution class `m+1`. To see how this prior is calculated, note that have these 5 constraints (again for the 7-taxon unrooted case):
~~~~~~
n_1 p_1 + n_2 p_2 + n_3 p_3 + n_4 p_4 + n_5 p_5 = 1.0
p_1 = C p_2
p_2 = C p_3
p_3 = C p_4
p_4 = C p_5
~~~~~~
{:.bash-output}

These constraints leave only p_5 to be determined:
~~~~~~
p_1 = C^4 p_5
p_2 = C^3 p_5
p_3 = C^2 p_5
p_4 = C^1 p_5
(n_1 C^4 + n_2 C^3 + n_3 C^2 + n_4 C^1 + n_5 C^0) p_5 = 1.0
p_5 = 1/[n_1 C^4 + n_2 C^3 + n_3 C^2 + n_4 C^1 + n_5 C^0]
~~~~~~
{:.bash-output}

For` _C = 2` and a polytomy prior, we have

|   m   |  ntopo |             qtree           |           ptree         |          ntopo*ptree        |
| :---: | :----: | :-------------------------: | :---------------------: | :-------------------------: |
|   1   |     1  |    1*2^4 = 16               | 2^4/5889 = 0.00271693   |    1*2^4/5889 = 0.00271693  |
|   2   |    56  |   56*2^3 = 448              | 2^3/5889 = 0.001358465  |   56*2^3/5889 = 0.076074036 |
|   3   |   490  |  490*2^2 = 1960             | 2^2/5889 = 0.000679232  |  490*2^2/5889 = 0.332823909 |
|   4   |  1260  | 1260*2^1 = 2520             | 2^1/5889 = 0.000339616  | 1260*2^1/5889 = 0.427916454 |
|   5   |   945  |  945*2^0 = 945              | 2^0/5889 = 0.000169808  |  945*2^0/5889 = 0.16046867  |
| total |  2752  | 16+448+1960+2520+945 = 5889 |                         |                 1.0         |

where `qtree` is the unnormalized probability of a particular tree topology (i.e. ptree is qtree divided by the sum of qtree values).

With this introduction, here is the deceptively simple `getLogNormalizedTopologyPrior` function. The hard part is computing the `_topology_prior` vector, which is described below in the explanation of the `recalcPriorsImpl` function. The log of the normalized topology prior is obtained as `_topology_prior[m]` minus `_topology_prior[0]` (the 0th element of `_topology_prior` holds the log of the normalization constant).
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_getLogNormalizedTopologyPrior-end_getLogNormalizedTopologyPrior","" }}
~~~~~~
{:.cpp}

## The recalcPriorsImpl member function

This function is responsible for recomputing the `_topology_prior` vector. If `_is_resolution_class_prior` is true, this function requires knowledge of the number of tree topologies in each resolution class, so it is called inside the `recalcCountsAndPriorsImpl` after the counts are calculated.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_recalcPriorsImpl-end_recalcPriorsImpl","" }}
~~~~~~
{:.cpp}

## The recalcCountsAndPriorsImpl member function

This function recomputes the `_counts` vector for the supplied number of internal nodes (`n`) using the method outlined by Joe Felsenstein in his 2004 book and also in Felsenstein (1978). Below I've reproduced the table from Felsenstein (2004) showing the number of possible rooted tree topologies for any number of internal nodes (rows) and up to 8 taxa (columns):

| nodes |   2   |   3   |   4   |   5    |   6    |   7    |    8    | nfactors | 
| ----- | ----- | ----- | ----- | ------ | ------ | ------ | ------- | -------- | 
|   1   |   1   |   1   |   1   |   1    |    1   |    1   |    1    |     0    | 
|   2   |       |   3   |   10  |   25   |   56   |   119  |   246   |     0    | 
|   3   |       |       |   15  |  105   |   490  |   1918 |   6825  |     0    | 
|   4   |       |       |       | 0.0105 | 0.1260 |  0.945 |  5.6980 |     1    | 
|   5   |       |       |       |        | 0.0945 | 1.7325 | 19.0575 |     1    | 
|   6   |       |       |       |        |        | 1.0395 | 27.0270 |     1    | 
|   7   |       |       |       |        |        |        | 13.5135 |     1    | 

The `recalcCountsAndPriorsImpl` function works from left to right, calculating each column in turn. Within a column, it works down. Each cell except the first in a given column requires knowledge of two cells from the column to its left: the cell to its immediate left as well as the cell above the cell to its immediate left. This is because in order to know how many tree topologies there are for `N` taxa, one needs to know how many places a taxon could be added to a tree with `N-1` taxa. The `N-1` taxon tree could have the same number of internal nodes as the new tree (the new taxon was added to an existing node, either creating a new polytomy or enlarging an existing one), or the `N-1` taxon tree could have one fewer internal nodes (in which case the new taxon inserts a new node).

Note that this function calls `recalcPriorsImpl` before returning.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_recalcCountsAndPriorsImpl-end_recalcCountsAndPriorsImpl","" }}
~~~~~~
{:.cpp}

## The reset member function

This function forces recalculation of `polytomy_prior` if `_is_resolution_class_prior` is false, and both `_counts` and `polytomy_prior` if `_is_resolution_class_prior` is true (or if `_counts_dirty` is true).
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_reset-end_reset","" }}
~~~~~~
{:.cpp}

## The getLogCount member function

This function returns the count of the number of distinct labeled tree topologies for `n` taxa and `m` internal nodes. It depends on the _counts vector being accurate, so it calls `recalcCountsAndPriors` function if `n` is not equal to `_ntax`. If `_is_rooted` is true, assumes `m` is less than `_ntax`. If `_is_rooted` is false, assumes `m` less than `_ntax` - 1.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_getLogCount-end_getLogCount","" }}
~~~~~~
{:.cpp}

## The getLogSaturatedCount member function

Returns the number of saturated (i.e. fully-resolved and thus having as many internal nodes as possible) trees of `n` taxa. Calls `recalcCountsAndPriors` function if `n` is not equal to `_ntax` because that means that the `_counts` vector is no longer accurate and must be recalculated.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_getLogSaturatedCount-end_getLogSaturatedCount","" }}
~~~~~~
{:.cpp}

## The getLogCounts member function

This function constructs a vector in which the element having index `m` (i = 0, 1, ..., maximum number of internal nodes) represents the natural logarithm of the number of tree topologies having `m` internal nodes. If `_counts_dirty` is true, it recomputes the `_counts` vectors first. The `0`th element of the returned vector holds the natural log of the total number of tree topologies (log of the sum of all other elements).
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_getLogCounts-end_getLogCounts","" }}
~~~~~~
{:.cpp}

## The getLogTotalCount member function

This function returns the natural log of the total number of trees for `n` taxa, including all resolution classes from the star tree to fully resolved (saturated) trees. Calls `recalcCountsAndPriors` function if `n` is not equal to `_ntax` or if not using the resolution class prior (in which case `_counts` have not been calculated).
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_getLogTotalCount-end_getLogTotalCount","" }}
~~~~~~
{:.cpp}

## The getRealizedResClassPriorsVect member function

Constructs a vector of realized resolution class priors from the values in the `_topology_prior` vector. If `_topo_priors_dirty` is true, it recomputes the `_topology_prior` vectors first. The `m`th element of the returned vector is set to `T_{n,m}*_topology_prior[m] `for` m > 0`. The `0`th element of the returned vector holds the normalization constant (sum of all other elements). This function is not efficient because it is intended only to be used for providing information to the user on request. Table 2, p. 248, in the Lewis, Holder, and Holsinger (2005) presented (normalized) values from this vector, as do the columns labeled `ntopo*ptree` in the tables presented above in the documentation for the `getLogNormalizedTopologyPrior` member function.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_getRealizedResClassPriorsVect-end_getRealizedResClassPriorsVect","" }}
~~~~~~
{:.cpp}

## The sample member function

Samples a resolution class (i.e. number of internal nodes) from the realized resolution class distribution. This function is not particularly efficient because it calls `PolytomyTopoPriorCalculator::getRealizedResClassPriorsVect`, resulting in an unnecessary vector copy operation.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_sample-end_sample","" }}
~~~~~~
{:.cpp}

## The setNTax setter and getNTax accessor

The `setNTax` function returns immediately if `_ntax` equals `new_ntax`; however, if `new_ntax` differs from `_ntax`, `setNTax` sets `_ntax` to `new_ntax` and sets `_topo_priors_dirty` to true so that subsequent requests for prior probabilities or counts will trigger recalculation. The `getNTax` function returns the value of the data member `_ntax`. 
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_setNTax-end_getNTax","" }}
~~~~~~
{:.cpp}

## The chooseRooted and chooseUnrooted member functions

The `chooseRooted` function sets the `_is_rooted` data member to `true`. There are more rooted than unrooted trees for the same value of `_ntax`, so this setting is important when asking questions that require knowledge of the numbers of possible trees. The `chooseUnrooted` function sets the `_is_rooted` data member to `false`.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_chooseRooted-end_chooseUnrooted","" }}
~~~~~~
{:.cpp}

## The chooseResolutionClassPrior and choosePolytomyPrior member functions

These two functions can be used to choose between a resolution class prior or the alternative (the polytomy prior).
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_chooseResolutionClassPrior-end_choosePolytomyPrior","" }}
~~~~~~
{:.cpp}

## The setC and getC member functions

These two functions either set or return the value of the data member `_C`, which determines the nature of the resolution class or polytomy prior. For the resolution class prior, each resolution class `m` has probability `_C` times greater than resolution class `m+1`. For the polytomy prior, a particular tree with `m` internal nodes has probability  `_C` times greater than a particular tree with `m+1` internal nodes.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_setC-end_getC","" }}
~~~~~~
{:.cpp}

## The setLogScalingFactor and getLogScalingFactor member functions

These two functions set or return the value of the data member `_log_scaling_factor` data member.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_setLogScalingFactor-end_getLogScalingFactor","" }}
~~~~~~
{:.cpp}

## The getCountsVect member function

Returns copy of the `_counts` vector, which contains in its `(m-1)`th element the number of tree topologies having exactly `m` internal nodes. Note that you will need to also call `getNFactorsVect` if there is any chance that some of the `_counts` are larger than `exp(_log_scaling_factor)`. In such cases, the actual log count equals `_nfactors[m]*_log_scaling_factor + log(count[m - 1]`.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_getCountsVect-end_getCountsVect","" }}
~~~~~~
{:.cpp}

## The getNFactorsVect member function

Returns copy of the `_nfactors` vector, which contains in its `(m-1)`th element the number of times `_counts[m]` has been rescaled by dividing by the scaling factor (the log of which is `_log_scaling_factor`).
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_getNFactorsVect-end_getNFactorsVect","" }}
~~~~~~
{:.cpp}

## The getTopoPriorVect member function

Returns copy of the `_topology_prior` vector, which contains in its `m`th element the unnormalized prior for tree topologies having exactly `m` internal nodes. The `0`th element of `_topology_prior` holds the normalizing constant.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_getTopoPriorVect-end_getTopoPriorVect","" }}
~~~~~~
{:.cpp}

## The getLogTopoProb member function

Returns result of call to `getLogTopologyPrior(m)`, where `m` is the number of internal nodes in the supplied tree `t`.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_getLogTopoProb-end_getLogTopoProb","" }}
~~~~~~
{:.cpp}

## The getLogTopologyPrior member function

Returns the natural logarithm of the unnormalized topology prior. This represents the resolution class prior if `_is_resolution_class_prior` is true, otherwise it represents the polytomy prior. 
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_getLogTopologyPrior-end_getLogTopologyPrior","" }}
~~~~~~
{:.cpp}

## The getLogNormConstant member function

Returns the natural logarithm of the normalizing constant for the topology prior. This value is stored in `_topology_prior[0]`.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_getLogNormConstant-end_getLogNormConstant","" }}
~~~~~~
{:.cpp}

## The isResolutionClassPrior and isPolytomyPrior member functions

The `isResolutionClassPrior` function returns the current value of the data member `_is_resolution_class_prior` and `isPolytomyPrior` returns `!isResolutionClassPrior()`.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_isResolutionClassPrior-end_isPolytomyPrior","" }}
~~~~~~
{:.cpp}

## The isRooted and isUnrooted member functions

The `isRooted` function returns the current value of the data member `_is_rooted`, and `isUnrooted` returns `!isRooted()`.
~~~~~~
{{ "steps/step-19/src/topo_prior_calculator.hpp" | polcodesnippet:"begin_isRooted-end_isUnrooted","" }}
~~~~~~
{:.cpp}

## Literature Cited

Felsenstein, J. 1978. The number of evolutionary trees. Systematic Zoology 27:27-33 (see also the 1981 correction Systematic Zoology 30:122). [https://doi.org/10.2307/2412810/](https://doi.org/10.2307/2412810)

Felsenstein, J. 2004. Inferring Phylogeny. Sinauer, Sunderland, Massachusetts. [ISBN: 9780878931774](https://global.oup.com/ushe/product/inferring-phylogenies-9780878931774?q=Felsenstein&cc=us&lang=en)

Lewis, P. O., Holder, M. T., and Holsinger, K. E. 2005. Polytomies and bayesian phylogenetic inference. Systematic Biology 54(2):241–253. [https://doi.org/10.1080/10635150590924208](https://doi.org/10.1080/10635150590924208)
