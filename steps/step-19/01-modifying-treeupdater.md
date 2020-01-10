---
layout: steps
title: Modifying TreeUpdater
partnum: 19
subpart: 1
description: Modify the TreeUpdater class to allow polytomies.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

We will need to make one modification to the `TreeUpdater` class. Specificaly, we need to provide a special case proposal for the case in which the current tree is the star tree. The star tree (only one internal node) is a special case because it does not contain a 3-edge segment to modify. In the case of the star tree, we will simply adjust two randomly-chosen edge proportions. Add the highlighted `starTreeMove` function prototype as well as the `_star_tree_move` data member to the class declaration
~~~~~~
{{ "steps/step-19/src/tree_updater.hpp" | polcodesnippet:"begin_class_declaration-end_class_declaration","a,b" }}
~~~~~~
{:.cpp}

Now add the following function body to {% indexfile tree_updater.hpp %}. 
~~~~~~
{{ "steps/step-19/src/tree_updater.hpp" | polcodesnippet:"begin_starTreeMove-end_starTreeMove","" }}
~~~~~~
{:.cpp}

Call the `starTreeMove` function near the beginning of the proposeNewState function if the current tree is the star tree:
~~~~~~
{{ "steps/step-19/src/tree_updater.hpp" | polcodesnippet:"begin_proposeNewState-end_star_tree_modifications","d-e,f-g" }}
~~~~~~
{:.cpp}

Finally, add a section to the `TreeUpdater::revert` function to revert a star tree move, if the current tree is the star tree.
~~~~~~
{{ "steps/step-19/src/tree_updater.hpp" | polcodesnippet:"begin_revert-end_revert","h-i,j" }}
~~~~~~
{:.cpp}
