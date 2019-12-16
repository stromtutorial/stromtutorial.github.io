---
layout: steps
title: Adding the PolytomyUpdater class
partnum: 19
subpart: 2
description: Create a PolytomyUpdater class to jump between resolution classes.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

We need a new updater that either adds and edge to an existing polytomy or deletes an edge to create a new polytomy (or expand an existing one). Create the `PolytomyUpdater` class declaration below in the file {% indexfile polytomy_updater.hpp %}.
~~~~~~
{{ "steps/step-19/src/polytomy_updater.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## The constructor and destructor

This class uses the base class (`Updater`) version of the `clear` function in the constructor and, as usual, the destructor has no work to do.
~~~~~~
{{ "steps/step-19/src/polytomy_updater.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The reset member function

The `reset` function is called at the end of the `Updater::update` function to prepare the updater for the next update. Any temporary variables needed by an updater during the update process need to be re-initialized in this function, and this `PolytomyUpdater` has several of these temporary variables, most of which are needed to keep track of how to revert if the proposed tree is rejected.
~~~~~~
{{ "steps/step-19/src/polytomy_updater.hpp" | polcodesnippet:"begin_reset-end_reset","" }}
~~~~~~
{:.cpp}

## The calcLogPrior member function

The `calcLogPrior` function calls two functions in the base class (`Updater`) and returns the sum of the values they calculate. This function is an abstract function in the base class (along with `proposeNewState` and `revert`) and so must be defined in this derived class.
~~~~~~
{{ "steps/step-19/src/polytomy_updater.hpp" | polcodesnippet:"begin_calcLogPrior-end_calcLogPrior","" }}
~~~~~~
{:.cpp}

## The proposeNewState member function

The proposeNewState function, which is an abstract base class member function that must be defined in any derived class, proposes either an add-edge or a delete-edge move. 

The first part of this function is involved in determining whether the current tree is the star tree (in which case add-edge is the only move possible) or a fully-resolved, binary tree (in which case delete-edge is the only option). If both add-edge and delete-edge are possible, then a coin flip is used to decide which of the two will be attempted.

The member functions `proposeAddEdgeMove` and `proposeDeleteEdgeMove` handle most of the work, once the decision is made which move to attempt. The calculation of the Hastings ratio and Jacobian determinant will be discussed when these functions are introduced.
~~~~~~
{{ "steps/step-19/src/polytomy_updater.hpp" | polcodesnippet:"begin_proposeNewState-end_proposeNewState","" }}
~~~~~~
{:.cpp}

## The proposeAddEdgeMove member function

The add-edge move is illustrated below. This move involves choosing a polytomy uniformly at random (1/2 in the example because there are 2 polytomies to choose from), drawing a new edge proportion by multiplying the tuning parameter (`_phi`) by a uniform(0,1) random deviate, squeezing the previous edge proportions down to make room for the new edge, and, finally, deciding how to partition the spokes of the original polytomy between the two nodes at the ends of the newly created edge (there are 3 possible choices for a polytomy with 4 spokes: 9,10 was chosen in the example, but 8,10 and 7,10 were also possibilities).

![The add-edge move]({{ "/assets/img/addedge.png" | absolute_url }}){:.center-math-image}

The full acceptance ratio comprises 5 terms:
![The add-edge acceptance ratio]({{ "/assets/img/Raddedge.png" | absolute_url }}){:.center-math-image}

The probability of accepting the proposed add-edge move is either 1.0 or R, whichever is smaller.

The first term (left-to-right) is the likelihood ratio, which is the likelihood of the tree after the add-edge move divided by the likelihood of the tree before the move. This term is straightforward (although, as you will see shortly, we will have to modify our `Likelihood` class to handle computing likelihoods for polytomous trees). 

The second term is the ratio of the prior density after the move divided by the prior density before the move. The prior in both cases is a product of the (discrete) topology prior probability and the edge proportions prior density. The topology prior is new, but will be discussed when we modify the `Updater` base class to add the member function `calcLogTopologyPrior`. The `PolytomyUpdater::calcLogPrior` function (see above) computes the joint prior, and the `Updater::update` function is where this function is called (before and after the proposed move).

The third term from the left is the ratio of the probability of proposing the reverse move (a delete-edge move that exactly reverses the proposed add-edge move) divided by the probability of proposing the add-edge move itself. If the current tree (before the add-edge move) is the star tree, then this ratio equals `0.5/1 = 0.5`. If the current tree is a fully-resolved tree, this ratio equals `1/0.5 = 2`. Otherwise, it equals 1.0.

The fourth term is the probability of the reverse move divided by the probability of the forward (add-edge) move. The reverse move involves only a choice of which edge to delete, the probability of which is just 1 divided by the number of edges in the tree (one more than the number of edges `n_e` in the starting tree for the add-edge move). 

The forward move is more complex. First, we must choose one of `n_p` polytomies to break up (probability `1/n_p`). Next, we must decide which of the `s` spokes (edges radiating from this polytomous node) to move to the newly created node. This probability is 1 divided by the number of ways of partitioning `s` spokes into 2 groups, ensuring that each group contains at least 2 spokes. This number of possible partitionings is quantity `2^(s-1)-s-1`. Finally, we must draw a Uniform(0,1) random deviate, `u`, to help us choose an edge proportion to go along with the newly created node. The probability density of this is the `f(u)` term, which equals 1.

The final term is the absolute value of the determinant of the Jacobian matrix for the transformation of `u` and the `n_e` edge proportions before the add-edge move into the `n_e+1` edge proportions in the tree after the new edge has been inserted. The calculation of the Jacobian determinant for the simplest case (4-taxon star tree) is illustrated below:

![The Jacobian for the 4-taxon star tree add-edge case]({{ "/assets/img/jacobian.png" | absolute_url }}){:.center-math-image}

Note that the Jacobian matrix is 4x4, not 5x5, as one edge proportion can be determined from the others. I've arbitrarily chosen edge proportion 1 as the one that is determined by the others.

Hopefully, I've provided enough background for you to now understand the `proposeAddEdgeMove` function (and the section of `proposeNewState` in which it appears).
~~~~~~
{{ "steps/step-19/src/polytomy_updater.hpp" | polcodesnippet:"begin_proposeAddEdgeMove-end_proposeAddEdgeMove","" }}
~~~~~~
{:.cpp}

## The proposeDeleteEdgeMove member function

The delete-edge move is illustrated below.

![The delete-edge move]({{ "/assets/img/deleteedge.png" | absolute_url }}){:.center-math-image}

The acceptance ratio for the delete-edge move is:
![The delete-edge acceptance ratio]({{ "/assets/img/Rdeledge.png" | absolute_url }}){:.center-math-image}

The explanation of terms is largely the same as above for the add-edge move except:
*`p*` now refers to the parameters after the delete-edge move and `p` to the parameters before the delete-edge move
* the third term equals 2 if the tree resulting from the delete-edge move is the star tree, 0.5 if the tree before the delete-edge move is fully resolved, and 1 otherwise. (Note that in the 4-taxon case the third term is always 1).
* n_p is the number of polytomies in the tree _resulting) from the delete-edge move
* s is the number of spokes in the polytomy created (or expanded) by deleting an edge
* n_e is the number of edges in the tree before the delete-edge move
* the Jacobian determinant equals 1 divided by `phi (1 - v)^(n_e-2)`, where `v` is the proportion of the tree length corresponding to the edge that was deleted

~~~~~~
{{ "steps/step-19/src/polytomy_updater.hpp" | polcodesnippet:"begin_proposeDeleteEdgeMove-end_proposeDeleteEdgeMove","" }}
~~~~~~
{:.cpp}

## The computePolytomyDistribution member function

The add-edge move splits the edges of a polytomy across a new edge. This function computes the probability distribution of possible partitionings and stores it in the map member variable `_poly_prob` under a key equal to the number of "spokes" in the polytomy where a spoke is an edge radiating from the polytomous node. The value of `_poly_prob[6]`, as an example, is the following vector of length 2:
~~~~~~
                     6!
_poly_prob[6][0] = ----- C   = 3/5
                   2! 4!

                      6!
_poly_prob[6][1] = ------- C = 2/5
                   3! 3! 2
~~~~~~
{:.bash-output}
where `C` is the normalizing constant, which equals `1/(2^5 - 6 - 1) = 1/25`. Note that the final probability must be divided by 2 in the case of an even number of spokes. This means that 2 of 6 spokes will be moved to the new node with probability 0.6 and 3 spokes will be moved with probability 0.4.
~~~~~~
{{ "steps/step-19/src/polytomy_updater.hpp" | polcodesnippet:"begin_computePolytomyDistribution-end_computePolytomyDistribution","" }}
~~~~~~
{:.cpp}

## The revert member function

This function simply reverses the add-edge or delete-edge move performed. It is called by `Updater::update` if the proposed move is not accepted.
~~~~~~
{{ "steps/step-19/src/polytomy_updater.hpp" | polcodesnippet:"begin_revert-end_revert","" }}
~~~~~~
{:.cpp}

## The refreshPolytomies member function

This function creates a vector (stored in `_polytomies`) of nodes that are polytomous. This is used by the add-edge move to choose a focal polytomy to split up.
~~~~~~
{{ "steps/step-19/src/polytomy_updater.hpp" | polcodesnippet:"begin_refreshPolytomies-end_refreshPolytomies","" }}
~~~~~~
{:.cpp}

