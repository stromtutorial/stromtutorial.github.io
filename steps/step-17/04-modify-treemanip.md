---
layout: steps
title: Modify the TreeManip Class
partnum: 17
subpart: 4
description: Add the LargetSimonSwap and randomInternalEdge functions to TreeManip. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

Two helper functions need to be added to the `TreeManip` class. The `randomInternalEdge` function returns a pointer to the `Node` object managing a randomly selected internal edge. The `LargetSimonSwap` function effects a swap of nodes `a` and `b`, where `a` and `b` are the ends of the 3-edge segment modified during a Larget-Simon move, with `a` the higher (furthest from the root) of the two in the tree. If `b` is not the bottom-most node in the path, node `a` and `b` are both removed from the tree, then `a` is placed back into the tree where `b` once was and vice versa. If `b` is bottom-most, then the other nodes along the path are swapped (which accomplishes the same NNI swap but is less trouble). This function works even if the nodes in the path are polytomies.

Here are the two lines that must be added to the class declaration in {% indexfile tree_manip.hpp %}.
~~~~~~
{{ "steps/step-17/src/tree_manip.hpp" | polcodesnippet:"start-end_class_declaration","b,c"}}
~~~~~~
{:.cpp}

Here is the function body for the `LargetSimonSwap` member function.
~~~~~~
{{ "steps/step-17/src/tree_manip.hpp" | polcodesnippet:"begin_LargetSimonSwap-end_LargetSimonSwap",""}}
~~~~~~
{:.cpp}

Here is the function body for the `randomInternalEdge` member function.
~~~~~~
{{ "steps/step-17/src/tree_manip.hpp" | polcodesnippet:"begin_randomInternalEdge-end_randomInternalEdge",""}}
~~~~~~
{:.cpp}
