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

~~~~~~
{{ "steps/step-19/src/tree_updater.hpp" | polcodesnippet:"start-end_class_declaration","a-b,c-d,e-f,g-h" }}
~~~~~~
{:.cpp}
The `processAssignmentString` and `handleAssignmentStrings` member functions now have an additional parameter `Model::SharedPtr` parameter. The `_likelihood` data member has been replaced by a vector of `Likelihood` shared pointers named `_likelihoods`, and `_model` has been deleted because the model specific to a particular `Likelihood` object can always be obtained from that `Likelihood` object using the `getModel` member function. the other changes are all additions of member functions or data members.

