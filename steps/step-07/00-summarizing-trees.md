---
layout: steps
title: Summarizing tree topologies
partnum: 7
subpart: 0
description: In this part, you will learn how to summarize trees in terms of their component splits. By the end of this section, your program will read a tree file and report all unique tree topologies and the number of trees having each topology. You will also install the Nexus Class Library in order to read NEXUS-formatted tree files.
---
{{ page.description | markdownify }}

A {% indexbold split %} is a partition that divides the leaves of a tree into two subsets. There is one split associated with every edge in a tree: if that edge is cut, two subtrees are produced and the leaves in each of these subtrees represent the two subsets composing the split. In practice, we will think of a split as the set of leaves in the subtree distal from the root. Thus, splits are only useful if they are all defined using the same root position.

A `Split` class {% indexhide Split %}will store the set of leaves that descend from a given node. A tree topology can be identified by the set of splits it contains. A {% indexbold tree ID %} is thus the set of splits in a tree. Because trees with identical tree IDs have identical topologies, we can use tree IDs to determine how many unique topologies there are for any set of trees having the same leaf set.

{% include subparts.html %}
