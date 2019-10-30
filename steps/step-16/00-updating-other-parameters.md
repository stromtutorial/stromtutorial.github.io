---
layout: steps
title: Updating Other Parameters
partnum: 16
subpart: 0
description: Create updater classes for state frequencies, exchangeabilities, omega, pinvar, and subset relative rates. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

State frequencies (e.g. nucleotide equilibrium relative frequencies), exchangeabilities (the relative rates for different types of substitution in the GTR model), and subset relative rates differ from other parameters in being both multivariate and constrained (one cannot update them individually because a change to any one of them necessarily forces at least one other one to change).

This part of the tutorial introduces a focussed Dirichlet approach to updating these kinds of model parameters. We will first create a base class (`DirichletUpdater`) and then create 3 derived classes (`StateFrequencyUpdater`, `GTRExchangeabilityUpdater`, and `SubsetRelRateUpdater`) that modify only the parts of the base class that are specific to the particular parameter that they manage.

We also create `PinvarUpdater` and `OmegaUpdater` to handle univariate parameter updates of the proportion of invariable sites and the nonsynonymous-synonymous rate ratio, respectively.

At the conclusion of this part, you will have a program that updates state frequencies, exchangeabilities, omega, pinvar, the gamma rate variance parameter, and subset relative rates as it carries out its Markov chain Monte Carlo simulation. The only remaining updaters needed will modify tree topology and edge lengths.

{% include subparts.html %}
