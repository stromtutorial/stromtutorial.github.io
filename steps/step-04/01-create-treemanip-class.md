---
layout: steps
title: Create the `TreeManip` class
partnum: 4
subpart: 1
description: There are benefits to keeping the `Tree` and `Node` classes simple, so in this step we will create a `TreeManip` class that will own, manage and manipulate a `Tree` object.
---
{{ page.description | markdownify }}

Rather than call the `createTestTree()` function of `Tree`, we will move that
function to `TreeManip`. `TreeManip` will also eventually have functions to create a
standard Newick description string, build a `Tree` from a Newick description, and
perform modifications to a `Tree` needed during Metropolis (MCMC) proposals.

First, create the `TreeManip` class {% indexhide TreeManip class %} by creating a new header file named
{% indexfile tree_manip.hpp %} and populating it with the following code:
~~~~~~
{{ "steps/step-04/src/tree_manip.hpp" | polcodesnippet:"start-bodies,end","" }}
~~~~~~
{:.cpp}
Class declarations like this should look familiar because there are many similarities of this class declaration to those we created for `Tree` and `Node`. Inside the `strom` namespace, we have created a class named `TreeManip` that has two constructors, a destructor, four public member functions, a private data member named `_tree`, and a type definition that will allow us to create shared pointers to `TreeManip` objects.

## Two constructors and a destructor
We still need to define bodies for each of the functions in the class declaration above. Here are the bodies for the two constructors and the destructor. These functions currently do little more than announce that a `TreeManip` object is being created or destroyed, except that the constructor that takes a tree shared pointer argument calls the member function `setTree`, which is defined below. These three member function definitions should go after the semicolon (`;`) ending the class declaration but before the curly bracket (`}`) that closes the namespace.
~~~~~~
{{ "steps/step-04/src/tree_manip.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The clear function
This function resets the `_tree` data member. Resetting a shared pointer causes it to no longer point to any object. If this shared pointer was still pointing to a `Tree` object, and if this was the last shared pointer holding onto that `Tree` object, the `Tree` object would be deleted (and the destructor function will report that it has been called).
~~~~~~
{{ "steps/step-04/src/tree_manip.hpp" | polcodesnippet:"begin_clear-end_clear","" }}
~~~~~~
{:.cpp}

## The setTree function
This function sets the data member `_tree` to point to the supplied tree `t`, and is used by the second contructor function. This second, extra constructor is not essential: we could use the default constructor (the one that takes no arguments) and then call `setTree` immediately afterwards and accomplish the same thing. The extra constructor just makes it possible to accomplish both actions (creating the `TreeManip` object and assigning a tree to it) in the same line of code.
~~~~~~
{{ "steps/step-04/src/tree_manip.hpp" | polcodesnippet:"begin_setTree-end_setTree","" }}
~~~~~~
{:.cpp}

## The getTree function
This function simply returns a shared pointer to the `Tree` object being managed by this `TreeManip` object.
~~~~~~
{{ "steps/step-04/src/tree_manip.hpp" | polcodesnippet:"begin_getTree-end_getTree","" }}
~~~~~~
{:.cpp}

## The calcTreeLength function
We will not need this function for a while, but given that it is a small, easily understood function, we might as well go ahead and add it to the class now. This function simply adds up the lengths of all edges in its tree and returns the sum (tree length). The function first sets the value of the local variable `TL` to zero. It then iterates through all nodes in the tree by setting the local variable `nd` to each element of the `_tree->_preorder` vector in turn and adding its `_edge_length` to the current value of `TL`. The {% indexbold auto %} keyword forces the compiler to figure out the type of the local variable `nd`. This makes the code easier to write and read. In this case, the type of `nd` is `Node *` (pointer to `Node`).
~~~~~~
{{ "steps/step-04/src/tree_manip.hpp" | polcodesnippet:"begin_calcTreeLength-end_calcTreeLength","" }}
~~~~~~
{:.cpp}

## The scaleAllEdgeLengths function
This function, like `calcTreeLength`, will not be needed for some time, but it is easy to add now so it will be in place later when we need to scale all edge lengths in the tree up or down. This function simply scales all edge lengths in the tree by multiplying each by the supplied value `scaler`.
~~~~~~
{{ "steps/step-04/src/tree_manip.hpp" | polcodesnippet:"begin_scaleAllEdgeLengths-end_scaleAllEdgeLengths","" }}
~~~~~~
{:.cpp}

## The createTestTree function
This function is nearly identical to the function of the same name in the `Tree` class. One difference is that we must create a `Tree` first, and data members of the `Tree` class (such as `_nodes`, `_preorder`, and `_is_rooted`) must be accessed through the `TreeManip` object's `_tree` pointer.
~~~~~~
{{ "steps/step-04/src/tree_manip.hpp" | polcodesnippet:"begin_createTestTree-end_createTestTree","" }}
~~~~~~
{:.cpp}

## Before moving on...
Edit your `Node` class (file {% indexfile node.hpp %}) and uncomment the lines making `TreeManip` a friend class of `Node`. This will just involve removing the initial `//` from the two bold, blue lines shown below in {% indexfile node.hpp %}.
~~~~~~
{{ "steps/step-04/src/node.hpp" | polcodesnippet:"start-end","a,b" }}
~~~~~~
{:.cpp}

Edit your `Tree` class (file {% indexfile tree.hpp %}) and delete all traces of `createTestTree()`. We no longer need `Tree` to have this capability because we can now ask `TreeManip` to create a test tree. The `Tree` class will also need to be modified so that `TreeManip` is a friend of `Tree`. I've indicated all lines in {% indexfile tree.hpp %} that need to be modified below in bold, blue text. I have commented out lines relating to `createTestTree` function so that I could show them to you, but you should feel free to just delete these lines entirely (they will be gone when I show you the contents of this file in future steps).
~~~~~~
{{ "steps/step-04/src/tree.hpp" | polcodesnippet:"start-end","a,b,c-d,e-f" }}
~~~~~~
{:.cpp}


