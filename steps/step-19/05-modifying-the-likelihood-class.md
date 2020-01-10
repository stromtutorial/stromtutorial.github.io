---
layout: steps
title: Modifying the Likelihood class
partnum: 19
subpart: 5
description: Calculation of the likelihood for polytomous trees.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

As you know, the `Likelihood` class uses BeagleLib to compute the likelihood of a tree. BeagleLib computes the likelihood by working down the tree in reverse level order, combining the conditional likelihood arrays for each pair of child lineages to compute the conditional likelihood vector for the parent node.

If trees contain polytomies, then a wrench is thrown in the works because a polytomous node has more than two children. We will take advantage of the fact that
1. there are always enough nodes in the `_nodes` array of the `Tree` object to represent a fully resolved tree, and 
2. the likelihood of a polytomous tree is identical to the likelihood of a fully resolved tree representing an arbitrary resolution of all polytomies so long as the edges added have an edge length equal to zero.

Begin by adding the highlighted lines below to the `Likelihood` class declaration in {% indexfile likelihood.hpp %}:
~~~~~~
{{ "steps/step-19/src/likelihood.hpp" | polcodesnippet:"start-end_class_declaration","a,b-bb,c" }}
~~~~~~
{:.cpp}

## Modifying the clear function

Add the indicated line to the clear function.
~~~~~~
{{ "steps/step-19/src/likelihood.hpp" | polcodesnippet:"begin_clear-end_clear","f" }}
~~~~~~
{:.cpp}

## Modifying the newInstance function

Initialize the `_identity_matrix` data member in the first few lines of `newInstance`:
~~~~~~
{{ "steps/step-19/src/likelihood.hpp" | polcodesnippet:"begin_newInstance-after_identity_matrix_init,end_newInstance","g-h" }}
~~~~~~
{:.cpp}

## Modify the queuePartialsRecalculation function

Add the highlighted lines in the `queuePartialsRecalculation` member function:
~~~~~~
{{ "steps/step-19/src/likelihood.hpp" | polcodesnippet:"!begin_queuePartialsRecalculation-end_queuePartialsRecalculation","begin_queuePartialsRecalculation,d-dd" }}
~~~~~~
{:.cpp}

## Modify the defineOperations function

This is the main modification made to the `Likelihood` class to allow for polytomies in trees. The vector `_polytomy_stack` starts out empty. Currently unused nodes are assigned to each polytomy until the polytomy is resolved (the number of children is just 2). Each fake internal node is assigned a transition matrix equal to the identity matrix because its edge length is necessarily zero (so no calculation of the transition matrix is needed). Operations are added to compute partials for each fake internal node and also the actual (polytomous) node. Each unused node that is put into service is stored in `_polytomy_stack` so that, after the likelihood is computed, all these fake internal nodes can be returned to their former unused state.

Add or change the highlighted lines in the `defineOperations` member function:
~~~~~~
{{ "steps/step-19/src/likelihood.hpp" | polcodesnippet:"begin_defineOperations-end_defineOperations","i,j-k,l-m,n-o,p-q,r-s" }}
~~~~~~
{:.cpp}

## Modify the calcLogLikelihood function

All we need to do to the `calcLogLikelihood` function is to return the nodes we pulled out of storage back to their previous, unused state.
~~~~~~
{{ "steps/step-19/src/likelihood.hpp" | polcodesnippet:"begin_calcLogLikelihood-end_calcLogLikelihood","t-tt" }}
~~~~~~
{:.cpp}
