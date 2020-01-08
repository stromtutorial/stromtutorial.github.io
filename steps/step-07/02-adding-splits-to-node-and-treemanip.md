---
layout: steps
title: Adding splits to Node and TreeManip
partnum: 7
subpart: 2
description: Before we can do anything interesting with our `Split` class, we need to add a `Split` object to the `Node` class and a `storeSplits` member function to the `TreeManip` class.
---
{{ page.description | markdownify }}

## Adding splits to the Node class

Uncomment the line that includes the {% indexfile split.hpp %} header, which is located at the top of the {% indexfile node.hpp %} file. Also uncomment the line that declares a data member of type `Split` just below `_edge_length` inside the `Node` class declaration. Finally, uncomment the `getSplit` accessor member function. The changes needed in the `Node` class are shown below.
~~~~~~
{{ "steps/step-07/src/node.hpp" | polcodesnippet:"start-end_class_declaration","a,b,c" }}
~~~~~~
{:.cpp}

## Adding the storeSplits member function to the TreeManip class

Add the following member function prototype to the `TreeManip` class declaration (just below `buildFromNewick` member function prototype).
~~~~~~
{{ "steps/step-07/src/tree_manip.hpp" | polcodesnippet:"start-end_class_declaration","a" }}
~~~~~~
{:.cpp}

Now add the member function body to the end of the {% indexfile tree_manip.hpp %} file (but before the right curly bracket that terminates the namespace scope).
~~~~~~
{{ "steps/step-07/src/tree_manip.hpp" | polcodesnippet:"begin_storeSplits-end_storeSplits","" }}
~~~~~~
{:.cpp}

The first for loop visits each node in the tree (i.e. every element of the `_nodes` vector) and resizes that node’s `_split` to ensure that all splits have just enough elements in their `_bits` vector to store any possible split defined on `_nleaves` taxa.

The second loop (which involves the use of `{% indexcode boost::adaptors::reverse %}` to reverse the order in which nodes in the `_preorder` vector are visited) conducts a {% indexshow postorder traversal %} (i.e. all descendants are visited before the node representing their most recent common ancestor is visited). Because of the `Split::resize` call we just did for every node, it is safe to assume that all splits have zeros in every bit. When visiting a leaf node (i.e. `nd->_left_child = 0`), this function sets the bit in its split corresponding to that leaf’s taxon number. If the leaf node has `_number = 0`, then the first (i.e. 0th) bit will be set in its `_split`.

Note that for any node having a `_parent` (i.e. all nodes except the root node), the bits in the node being visited are added to the bits already set in the `_parent` node. Thus, by the time all descendants of an internal node have been visited, the `_split` for that internal node is already completely specified. When an internal node is visited, all that needs to be done is to add its `_split` to the growing `splitset` and, of course, add the bits defined in its `_split` to its own parent.

The parameter `splitset` is a reference to a `{% indexcode std::set %}` of `Split` objects. A {% indexshow reference %} in C++ represents an alias to an existing variable. In this case, the `std::set<Split>` variable aliased must be created before this function is called, and this function builds up the set as it walks through the tree. Why use a `std::set` and not a `std::vector` here? Different trees may have identical splits, but they may not be in the same order depending on how each edge is rotated. If splits are added to a set, then the order in which they are encountered does not matter: trees with the same set of splits will have identical split sets. When the function exits, the external `std::set<Split>` variable will contain the same number of splits as there are internal nodes in the tree, and will serve as a {% indexbold tree ID %}: a unique bar code that identifies the tree topology and allows other trees with the same topology (but probably different edge lengths) to be identified.


