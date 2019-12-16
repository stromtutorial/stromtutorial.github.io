---
layout: steps
title: Adding calcLogTopologyPrior to Updater
partnum: 19
subpart: 4
description: Move calcLogTopologyPrior from TreeUpdater to the base class Updater so that it can be shared with PolytomyUpdater.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

## Updating the Updater class declaration

The following highlighted changes need to be made to the class declaration in {% indexfile updater.hpp %}:
~~~~~~
{{ "steps/step-19/src/updater.hpp" | polcodesnippet:"start-end_class_declaration","a,b,c,d,e,f" }}
~~~~~~
{:.cpp}

## Modifying the clear function

Initialize the new data member `_topo_prior_C` in the clear function:
~~~~~~
{{ "steps/step-19/src/updater.hpp" | polcodesnippet:"begin_clear-end_clear","g" }}
~~~~~~
{:.cpp}

## Modifying the reset function

Set the new data member `_log_jacobian` to zero in the reset function:
~~~~~~
{{ "steps/step-19/src/updater.hpp" | polcodesnippet:"begin_reset-end_reset","h" }}
~~~~~~
{:.cpp}

## Add the setTopoPriorC function

This function allows the topology prior parameter C to be set.
~~~~~~
{{ "steps/step-19/src/updater.hpp" | polcodesnippet:"begin_setTopoPriorC-end_setTopoPriorC","h" }}
~~~~~~
{:.cpp}

## Modify the update function

Modify the update function to take account of the new _log_jacobian data member.
~~~~~~
{{ "steps/step-19/src/updater.hpp" | polcodesnippet:"begin_update-end_update","i-j,k-l" }}
~~~~~~
{:.cpp}

## Add the calcLogTopologyPrior function

This function uses the data member _topo_prior_calculator to compute the log of the tree topology prior.
~~~~~~
{{ "steps/step-19/src/updater.hpp" | polcodesnippet:"begin_calcLogTopologyPrior-end_calcLogTopologyPrior","" }}
~~~~~~
{:.cpp}

## Modify the calcLogEdgeLengthPrior function

The `calcLogEdgeLengthPrior` function must be modified because, if polytomies may be present, we must now count the number of edges in the tree can can no longer use a formula that only works for binary trees.
~~~~~~
{{ "steps/step-19/src/updater.hpp" | polcodesnippet:"begin_calcLogEdgeLengthPrior-end_calcLogEdgeLengthPrior","m-n" }}
~~~~~~
{:.cpp}

