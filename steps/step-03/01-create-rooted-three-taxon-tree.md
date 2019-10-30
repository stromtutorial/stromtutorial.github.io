---
layout: steps
title: Create a rooted, 3-taxon tree
partnum: 3
subpart: 1
description: Add a function that creates a 3-taxon rooted tree.
---
The next step is to add a function that creates a 3-taxon rooted tree. We will call this function from the `Tree` constructor in order to have a way to generate a test tree.

This way of generating a tree is admittedly tedious, and we will soon abandon it for a more general method that automatically creates trees from newick tree descriptions

## Additions to *tree.hpp*
Most of the code below is already in your {% indexfile tree.hpp %} file. Just add the bold, blue text. Note that the `clear()` function call has been commented out in the Tree constructor.

~~~~~~
{{ "steps/step-03/src/tree.hpp" | polcodesnippet:"start-end","a,b-c,e-f"}}
~~~~~~
{:.cpp}

## Explanation of `createtesttree()`

We’ve added a member function to the `Tree` class named `createTestTree()`, and this function is now called, instead of `clear()`, in the `Tree` constructor. Let’s go through this function to see how it creates a tree.
~~~~~~
{{ "steps/step-03/src/tree.hpp" | polcodesnippet:"g",""}}
~~~~~~
{:.cpp}
This line sets the size of the `_nodes` vector to 6. The `Node` constructor will be called 6 times when this line is executed because 6 `Node` objects are created, one for each of the first 6 elements of the `_node`s vector.
~~~~~~
{{ "steps/step-03/src/tree.hpp" | polcodesnippet:"h-i",""}}
~~~~~~
{:.cpp}
These 6 lines create pointers to each of the 6 node objects stored in the `_nodes` vector. This will make it easier (and less error-prone) to hook up each node to its parent and children. Note the ampersand symbol (`&`): in your mind, replace the `&` symbol with the words *memory address of*. The asterisk symbol (`*`) means *pointer*, and `Node *` means *pointer to an object of type Node*. A pointer stores the memory address of an object. Thus, the pointer named `root_node` stores the memory address of the first element (element 0) of the `_nodes` vector. {% indexhide ampersand (`&`) %} {% indexhide asterisk (`*`) %}
~~~~~~
{{ "steps/step-03/src/tree.hpp" | polcodesnippet:"j-k",""}}
~~~~~~
{:.cpp}
The lines above provide values for all the data members of the `Node` object pointed to by the `root_node` pointer. The {% indexhide arrow symbol (`->`) %} arrow symbol (`->`) is used to specify a data member or method of an object when a pointer is used to refer to the object, while a {% indexhide dot (`.`) %} dot (`.`) would be used if attributes are being set for the object directly. The root node has no parent and no right sibling, so we set these pointers to the value 0. The left child of the root node should point to the first internal node, and `first_internal` is a pointer to that node.

The other nodes are set up similarly according to the tree diagram given in the comments.
~~~~~~
{{ "steps/step-03/src/tree.hpp" | polcodesnippet:"l-m",""}}
~~~~~~
{:.cpp}
The above 3 lines set data members of the `Tree` class. We specify `true` for `_is_rooted` because we want this tree to be interpreted as a rooted tree (setting this to false would imply that the root node is really a leaf and observed data is associated with the root node).
~~~~~~
{{ "steps/step-03/src/tree.hpp" | polcodesnippet:"n-o",""}}
~~~~~~
{:.cpp}
The 5 lines above build up the `_preorder` vector one element at a time. Each element is a pointer to a `Node` object already stored in the `_nodes` vector. The order in which we push pointers onto `_preorder` is important. It is called `_preorder` because it stores nodes in {% indexbold preorder sequence %} (ancestral nodes visited before their children). One could walk through all the nodes of the tree in preorder sequence by simply iterating through this vector. Iterating through `_preorder` in reverse would walk the tree in {% indexbold postorder sequence %} (child nodes visited before their parents).

Important: note that the root node is not included in the `_preorder` vector. There are two reasons for this. First, we already have a pointer (`_root`) to the root node. Second, in most situations the root node either is ignored (rooted trees) or is treated specially (unrooted trees), so you will soon see that we almost always start iterating through nodes with the first (and only) child of the root node anyway: starting at the root node would only cause us to add extra lines to skip the root node.
~~~~~~
{{ "steps/step-03/src/tree.hpp" | polcodesnippet:"p-q",""}}
~~~~~~
{:.cpp}
The `_levelorder` vector also stores a list of node pointers, but this time in {% indexbold level-order sequence %} rather than preorder sequence. Level-order sequence involves visiting all children of the current level before moving closer to the root in the tree. The reason why this sequence needs to be stored will be explained later when likelihoods are calculated.

