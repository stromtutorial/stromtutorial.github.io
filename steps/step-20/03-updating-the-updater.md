---
layout: steps
title: Updating the Updater
partnum: 20
subpart: 3
description: Here we add a data member and member function to inform Updater that only the likelihood should be heated
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

To finish up, we need to add some functionality to the `Updater` class. First, add a data member and a member function to the class declaration to inform an `Updater` object that the heating power should only be applied to the likelihood, not both prior and likelihood.
~~~~~~
{{ "steps/step-20/src/updater.hpp" | polcodesnippet:"begin_updater_class_declaration-end_updater_class_declaration","declare_setHeatLikelihood,declare_heat_likelihood_only_data" }}
~~~~~~
{:.cpp}

Next, initialize the data member in the `clear` function.
~~~~~~
{{ "steps/step-20/src/updater.hpp" | polcodesnippet:"begin_clear-end_clear","clear_heat_likelihood_only" }}
~~~~~~
{:.cpp}

We use the value stored in the `_heat_likelihood_only` data member inside the `update` function:
~~~~~~
{{ "steps/step-20/src/updater.hpp" | polcodesnippet:"begin_update-end_update","use_heating_power_only_start-use_heating_power_only_stop" }}
~~~~~~
{:.cpp}

Finally, add a setting function for the `_heat_likelihood_only` data member.
~~~~~~
{{ "steps/step-20/src/updater.hpp" | polcodesnippet:"begin_setHeatLikelihoodOnly-end_setHeatLikelihoodOnly","" }}
~~~~~~
{:.cpp}
