---
layout: steps
title: Create a Newick tree description
partnum: 5
subpart: 1
description: In this section you will write the `makeNewick` function, which creates a string that records both the topology and edge lengths of the tree.
---
Our next goal is to create a {% indexshow newick %} string for the tree currently stored in a `TreeManip` object. Add the `makeNewick` function declaration in the public area of the `TreeManip` class declaration, and add the `makeNewick` function body just above the terminating curly bracket of the namespace (i.e. just before the end of the file):
~~~~~~
{{ "steps/step-05/src/tree_manip.hpp" | polcodesnippet:"start-end_declaration,dots-end","x,y,a,b-c" }}
~~~~~~
{:.cpp}
Note the convention that `//...` indicates that possibly many lines have been omitted from the code snippet.

## Include the necessary header files
In the code shown above, you may have noticed that I've also indicated in blue, bold text two new headers that have been included near the top of the file. The `<stack>` header allows us to use the standard `stack` {% indexhide std::stack %} and the `<boost/format.hpp>` header allows us to use the Boost `format` function {% indexhide boost::format %}, which simplifies producing formatted strings. 

## Explanation of the `makeNewick` function
![Four-taxon rooted tree showing corresponding newick string]({{ "/assets/img/newick-rooted.png" | absolute_url }}){: .right-image}
The goal of this function is to turn a tree in memory into a {% indexshow newick %}
 string representation using nested parentheses to indicate clades and subclades. Each left parenthesis encountered in the newick description signifies the start of a new clade (we are moving to the left child of the current node), each comma means we're moving laterally to a sibling node, and a right parenthesis means we have reached the upper right node in a clade and are dropping back down to the parent of that node. The figure at right shows a newick description for 4-taxon rooted tree. The numbers at the nodes are the index of the node in the `_preorder` vector. Note that the root node is not included in `_preorder`, so there is no number shown in the figure for the root node.

The main loop in this function visits all nodes in the tree in {% indexshow preorder sequence %}:
~~~~~~
{{ "steps/step-05/src/tree_manip.hpp" | polcodesnippet:"begin_mainloop-dots_mainloop,end_mainloop","" }}
~~~~~~
{:.cpp}
The `auto nd : _tree->_preorder` code sets `nd` to each element of `_tree->_preorder`, in turn. The {% indexshow C++11 keyword `auto` %} makes our life easy by determining the type of `nd` automatically.

Internal nodes, when visited, result in the addition of a left parenthesis to the growing newick string. We will not add information about the internal node until we are finished with the clade defined by the node and have added the matching right parenthesis. Internal nodes are stored in the `node_stack`. A {% indexbold stack %} is a container in which items can only be removed in the reverse order in which they were added to the stack, like a stack of documents (the last one added to the top is also the first one removed). It is necessary to store each internal node in the stack so that we can determine how many right parentheses to add to the newick string.
~~~~~~
{{ "steps/step-05/src/tree_manip.hpp" | polcodesnippet:"begin_stack-end_stack","" }}
~~~~~~
{:.cpp}
When a tip node is visited, the `tip_node_format` string is used to add a number (one greater than the node's `_number`) followed by a colon and then the edge length for that tip node.
~~~~~~
{{ "steps/step-05/src/tree_manip.hpp" | polcodesnippet:"tip_format","" }}
~~~~~~
{:.cpp}
The Boost library's format class is used to format the string. Here we are creating an object of type `{% indexcode boost::format %}` (the `boost` part is the Boost namespace, while `format` is the class). The variable's name is `tip_node_format`. This works much like Python or sprintf in C: placeholders (single percent symbols followed by a letter such as d, f, or s) are substituted for the supplied integer, floating point value, or string, respectively. Each doubled percent (`%%`) ends up being a single literal `%` in the string after replacements, so the above statement will be equal to the following after replacement of the `%d` by the supplied precision (assume that precision = 5 for this example):
~~~~~~
const boost::format tip_node_format("%d:%.5f");
~~~~~~
{:.bash-output}
You might wonder why the `boost::str(...)` is necessary. The `boost::format` constructor takes a string as its sole argument, not a `boost::format` object, and the Boost format library does not provide an implicit conversion of format objects to string objects, so the `boost::str` function provides an explicit way to do this conversion. This is done intentionally in order to make it easier to report errors accurately when compiling formats. You could also call {% indexcode boost::format %}'s `str` function to accomplish the conversion:
~~~~~~
const boost::format tip_node_format( (boost::format("%%d:%%.%df") % precision).str() );
~~~~~~
{:.bash-output}
The [Boost format documentation](https://www.boost.org/doc/libs/1_64_0/libs/format/doc/format.html#examples) provides some examples of using `boost::format`.

If the tip node has a sibling to its right, a comma is output to the growing newick string. If the tip node does not have a right sibling, then things get interesting! Take a look at the figure above of a rooted tree and its associated newick description. Note that after the tip node C is output, two right parentheses (each followed by edge lengths) are output before the next node in the preorder sequence (tip node D) is handled. Why is this? It is because we must close up the definitions of two clades before moving on to tip node D. The first clade is the "cherry" comprising tip nodes B and C along with the internal node labeled 3. The second clade that must be finished comprises nodes labeled 1, 2, 3, 4 and 5. How do we know which edge lengths to spit out? The answer lies in our `node_stack`. The last two nodes added to `node_stack` are the internal nodes labeled 1 and 3 in the figure. When we reach the far corner of a clade (we know we are at the far corner when a node has no children and no siblings), we must pop nodes off the `node_stack` (outputting a right parenthesis and edge length for each) until we reach an internal node with a right sibling: this sibling is necessarily the next node in the preorder sequence (in this case, tip node D, labeled 6 in the figure). That is what is happening in the large `else` clause paired with `if (nd->_right_sib)` in the code for the `makeNewick` function. This `else` clause is further complicated by the need to check whether we are done, which happens when we pop the last node off the `node_stack`.

The stack function `top()` returns the next object on the stack without actually removing it. The function `pop()` deletes the object that is at the top of the stack. Note that we must always call `top()` to save the node at the top of the stack before we call `pop()` to delete it: it would not do to lose track of a node by `pop()`ing it with out first saving a pointer to the node `pop()`ed!

Finally, note that we add a comma to the newick string if the last internal node popped off the stack has a right sibling. The comma always indicates a sibling relationship for a node whether the node is a tip node or an internal node.

## The special case of the root node in unrooted trees
![five-taxon unrooted tree showing corresponding newick string]({{ "/assets/img/newick-unrooted.png" | absolute_url }}){: .right-image}

The figure of a 5-taxon unrooted tree on the right is subtly different from the figure above of a 4-taxon rooted tree. The root node in this case represents a tip (A), and this particular tip along with its edge length (which is really the `_edge_length` belonging to its only child (the node labeled 0 in the figure) is saved to the newick string as if it were the leftmost child of the first node in the `_preorder` vector. The part of the `makeNewick` function that accomplishes this is contained in the `if (root_tip) {...}` block of code. The variable `root_tip` is 0 if the tree is rooted, but points to the root node if the tree is unrooted, so the code inside the `if (root_node) {...}` conditional is only executed if `root_tip` actually points to the root node. This code adds the number of the root node and a comma immediately after the opening left parenthesis. It also sets root_tip to 0 so that this block of code is not executed again.

While it is not essential to store unrooted trees by "rooting" them at a tip node, that is the convention that is followed throughout this tutorial. This points out a basic ambiguity inherent in newick tree descriptions. If you saw the newick string
~~~~~~
(A:0.13,(B:0.2,(C:0.1,D:0.1):0.11):0.15,E:0.3)
~~~~~~
{:.bash-output}
how would you know this represents an unrooted binary tree and not a rooted tree with a polytomy (multifurcation) at the base? The answer is you could not know this unless the user who supplied you with the newick string told you.






