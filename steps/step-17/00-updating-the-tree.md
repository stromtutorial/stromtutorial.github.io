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

The only remaining obstacle on the way to creating a fully-functional MCMC sampler for phylogenetics is to derive a `TreeUpdater` class from `Updater` that can simultaneously update both the tree topology and edge lengths. The `TreeUpdater` created in this tutorial implements a modified version of the "LOCAL move without a molecular clock" for unrooted trees described by Larget and Simon (1999). There are many other tree proposal methods that have been invented since 1999, so you should feel free to implement others. See Clemens et al. (2008) for some examples of unrooted tree proposals and Höhna et al. (2008) for similar rooted tree examples.

Modifying the tree topology introduces an issue that has not yet arisen. All updaters introduced thus far require the entire likelihood to be recalculated. The LOCAL move, on the other hand, affects only a small part of the tree, providing an opportunity to improve computational efficiency; most transition probability matrices and partial likelihood arrays are unaffected by the LOCAL proposal, so it behooves us to do a little bookkeeping to prevent these from being recalculated unnecessarily. Furthermore, if a LOCAL proposal fails to be accepted, all transition probability matrices and partial arrays need to be returned to their previous values. It thus also behooves us to use more memory to save the original values so that those can simply be swapped back in if a proposal fails: this would benefit efficiency of all MCMC proposals, not just the tree topology updater.

Markov chains used in phylogenetic MCMC mix much better if proposals are added that rescale the tree periodically. If a tree needs to get generally bigger or smaller, it can take a long time to achieve the rescaling if we are depending on the LOCAL move, which only modifies 3 edges in a single update, and then only if the proposed move is accepted. Thus, we will simultaneously add a `TreeLengthUpdater` class that proposes rescaling the entire tree when its update method is called.

Because we are introducing a `TreeLengthUpdater` to handle overall tree scaling, the modification we will make to the Larget-Simon method is to not do the expansion/contraction step that shrinks/grows a 3-contiguous-edge segment of the tree by a random amount. Our version of the Larget-Simon proposal will thus only modify edge length _proportions_, not edge _lengths_. This is more in line with the way we've parameterized edge lengths: edge length parameters in our model are proportions of the tree length. An advantage of leaving all scaling of edge lengths to the `TreeLengthUpdater` class is that `TreeUpdater` does not need to worry about the Hastings ratio because our modified version of the proposal is symmetric: the probability (density) of the forward move is identical to the probability (density) of the reverse move.

{% include subparts.html %}

## Literature Cited

Höhna, S., Defoin-Platel, M., and Drummond, A. J. 2008. Clock-constrained tree proposal operators in Bayesian phylogenetic inference (pp. 1–7). Presented at the 2008 8th IEEE International Conference on Bioinformatics and BioEngineering (BIBE), IEEE. [DOI: 10.1109/BIBE.2008.4696663](https://doi.org/10.1109/BIBE.2008.4696663)

Lakner, C., Van Der Mark, P., Huelsenbeck, J. P., Larget, B., and Ronquist, F. 2008. Efficiency of Markov chain Monte Carlo tree proposals in Bayesian phylogenetics. Systematic Biology 57:86–103. [DOI: 10.1080/10635150801886156](https://doi.org/10.1080/10635150801886156)

Larget, B., and Simon, D. 1999. Markov Chain Monte Carlo Algorithms for the Bayesian Analysis of Phylogenetic Trees. Molecular Biology and Evolution 16:750–759. [DOI: 10.1093/oxfordjournals.molbev.a026160](https://doi.org/10.1093/oxfordjournals.molbev.a026160)
