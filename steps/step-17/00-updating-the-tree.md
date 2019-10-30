---
layout: steps
title: Updating the Tree
partnum: 17
subpart: 0
description: Create an updater that modifies the tree topology and edge lengths. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

The only remaining obstacle on the way to creating a fully-functional MCMC sampler for phylogenetics is to derive a `TreeUpdater` class from `Updater` that can simultaneously update both the tree topology and edge lengths. The `TreeUpdater` created in this tutorial implements the "LOCAL move without a molecular clock" for unrooted trees described by Larget and Simon (1999). There are many other tree proposal methods that have been invented since 1999, so you should feel free to implement others. See Clemens et al. (2008) for some examples of unrooted tree proposals and Höhna et al. (2008) for similar rooted tree examples.

Markov chains used in phylogenetic MCMC mix much better if proposals are added that rescale the tree periodically. If a tree needs to get generally bigger or smaller, it can take a long time to achieve the rescaling if we are depending on the LOCAL move, which only modifies 3 edges in a single update, and then only if the proposed move is accepted. Thus, we will simultaneously add a `TreeLengthUpdater` class that proposes rescaling the entire tree when its update method is called.

{% include subparts.html %}

## Literature Cited

Höhna, S., Defoin-Platel, M., and Drummond, A. J. 2008. Clock-constrained tree proposal operators in Bayesian phylogenetic inference (pp. 1–7). Presented at the 2008 8th IEEE International Conference on Bioinformatics and BioEngineering (BIBE), IEEE. [DOI: 10.1109/BIBE.2008.4696663](https://doi.org/10.1109/BIBE.2008.4696663)

Lakner, C., Van Der Mark, P., Huelsenbeck, J. P., Larget, B., and Ronquist, F. 2008. Efficiency of Markov chain Monte Carlo tree proposals in Bayesian phylogenetics. Systematic Biology 57:86–103. [DOI: 10.1080/10635150801886156](https://doi.org/10.1080/10635150801886156)

Larget, B., and Simon, D. 1999. Markov Chain Monte Carlo Algorithms for the Bayesian Analysis of Phylogenetic Trees. Molecular Biology and Evolution 16:750–759. [DOI: 10.1093/oxfordjournals.molbev.a026160](https://doi.org/10.1093/oxfordjournals.molbev.a026160)
