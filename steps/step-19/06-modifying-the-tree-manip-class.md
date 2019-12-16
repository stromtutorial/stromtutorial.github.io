---
layout: steps
title: Modifying the TreeManip class
partnum: 19
subpart: 6
description: Add functions to TreeManip to enable likelihood calculation on polytomous trees.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

We need to add a few functions to the TreeManip class to facilitate dealing with trees containing polytomies.

Add the highlighted lines below to the TreeManip class declaration.
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"start-end_class_declaration","a,b,c-d" }}
~~~~~~
{:.cpp}

## The calcResolutionClass function

Returns the resolution class of the managed tree (the resolution class is just the number of internal nodes).
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_calcResolutionClass-end_calcResolutionClass","" }}
~~~~~~
{:.cpp}

## The countInternals function

Counts and returns the number of internal nodes in the managed tree.
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_countInternals-end_countInternals","" }}
~~~~~~
{:.cpp}

## Simplifying the refreshPreorder function

The changes to the refreshPreorder function highlighed below are not necessary, but serve to simply this function and others by making use of a new function findNextPreorder. In the next major version of the tutorial, these changes will be moved earlier in the tutorial when refreshPreorder is first introduced.
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_refreshPreorder-end_refreshPreorder","e-f" }}
~~~~~~
{:.cpp}

## Modifying the renumberInternals function

In the previous version of this function, the internal nodes that were not used because of the presence of polytomies were simply numbered. Now that there is the possibility that a tree can make use of these initially unused nodes, we will modify this function to store these currently unused nodes in the `_unused_nodes` vector of the managed Tree object. 
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_renumberInternals-end_renumberInternals","g-h" }}
~~~~~~
{:.cpp}

## Adding the findNextPreorder function

This function was introduced to simplify the refreshPreorder function. In a future version of the tutorial, this function will be introduced earlier.
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_findNextPreorder-end_begin_findNextPreorder","" }}
~~~~~~
{:.cpp}

## Adding the findLeftSib function

This function determines whether there is a sibling to the left of the supplied Node. If so, a pointer to that left sib is returned; if there is no left sib, NULL is returned.
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_findLeftSib-end_findLeftSib","" }}
~~~~~~
{:.cpp}

## Adding the findRightmostChild function

Finds the rightmost child of the supplied Node. If there are no children, returns NULL.
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_findRightmostChild-end_findRightmostChild","" }}
~~~~~~
{:.cpp}

## Adding the findLastPreorderInClade function

Walks through the entire clade rooted at the supplied node, returning the last descendant in the preorder sequence before leaving the clade.
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_findLastPreorderInClade-end_findLastPreorderInClade","" }}
~~~~~~
{:.cpp}

## Adding the insertSubtreeOnLeft function

Makes supplied Node `s` the left child of supplied Node `u`.
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_insertSubtreeOnLeft-end_insertSubtreeOnLeft","" }}
~~~~~~
{:.cpp}

## Adding the insertSubtreeOnRight function

If supplied Node `u` has children, this function makes supplied Node `s` the right sib of the rightmost child of `u`. If `u` has no children currently, `s` is made the only child of `u`.
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_insertSubtreeOnRight-end_insertSubtreeOnRight","" }}
~~~~~~
{:.cpp}

## Adding the detachSubtree function

This function detaches the supplied Node `s` from the managed tree. This function is normally used just before calling the `insertSubtreeOnLeft` or `insertSubtreeOnRight` function to reinsert `s`.
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_detachSubtree-end_detachSubtree","" }}
~~~~~~
{:.cpp}

## Adding the rectifyNumInternals function

Tree objects have an `_ninternals` data member that holds the number of internal nodes in the tree. Polytomy analyses perform modifications to trees that change the number of internal nodes, and this function is used to keep this Tree data member accurate by adding incr to the value currently stored in the managed tree's `_ninternals` data member. Note that `incr` is an int and can be negative.
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_rectifyNumInternals-end_rectifyNumInternals","" }}
~~~~~~
{:.cpp}

## Adding the refreshNavigationPointers function

This function calls `refreshPreorder` and `refreshLevelorder` to ensure that the managed tree's _preorder and _levelorder vectors accurately reflect the structure of the tree. This function should be called if, for example, a polytomy is created or expanded by a delete-edge move or is broken apart by an add-edge move.
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_refreshNavigationPointers-end_refreshNavigationPointers","" }}
~~~~~~
{:.cpp}

## Adding the getUnusedNode and putUnusedNode functions

Polytomies in trees mean that not all elements of the tree's `_nodes` vector are used. The Tree class's `_unused_nodes` stack keeps a list of nodes that are currently not being used. These unused nodes are tapped by the `Likelihood::defineOperations` function to help in computing the likelihood on polytomous trees. the `getUnusedNode` removes a node from the top of the stack and returns a pointer to the removed Node object. The `putUnusedNode` function takes a `Node` pointer and pushes it onto the `_unused_nodes` stack.
~~~~~~
{{ "steps/step-19/src/tree_manip.hpp" | polcodesnippet:"begin_getUnusedNode-end_putUnusedNode","" }}
~~~~~~
{:.cpp}

