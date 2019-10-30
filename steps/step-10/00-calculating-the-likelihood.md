---
layout: steps
title: Calculating the likelihood
partnum: 10
subpart: 0
description: Now that we can load sequence data and a tree into memory, it is time to calculate the likelihood of the tree, which is the probability of the observed data given that tree and a model of evolution.
---
{{ page.description | markdownify }}

[BeagleLib](https://github.com/beagle-dev/beagle-lib) (Ayres et al. 2012, 2019) is a library that provides the ability to compute phylogenetic likelihoods for any model that can be specified by an instantaneous rate matrix of a continuous-time, finite-state Markov chain. Furthermore, it allows likelihoods to be computed using Graphics Processing Units (GPUs) if they are available, which can substantially speed up likelihood evaluation for models involving many states (such as amino acid or codon models). 

In this step of the tutorial you will use BeagleLib to compute the likelihood of a tree read from a file using the simplest evolutionary model, the Jukes-Cantor (1969) model with equal rates across sites (JC69). In later steps you will increase the complexity of the models used, ultimately being able to process both GTR+I+G models and a basic codon model (and link parameters of these models across data subsets in arbitrary ways).

{% include subparts.html %}

## Literature Cited

DL Ayres, A Darling, DJ Zwickl, P Beerli, MT Holder, PO Lewis, JP Huelsenbeck, F Ronquist, DL Swofford, MP Cummings, A Rambaut, MA Suchard. 2012. BEAGLE: An application programming interface and high-performance computing library for statistical phylogenetics. Systematic Biology 61:170–173. [DOI: 10.1093/sysbio/syr100](https://doi.org/10.1093/sysbio/syr100)

DL Ayres, MP Cummings, G Baele, AE Darling, PO Lewis, DL Swofford, JP Huelsenbeck, P Lemey, A Rambaut, and MA Suchard. 2019. BEAGLE 3: improved performance, scaling, and usability for a high-performance computing library for statistical phylogenetics. Systematic Biology syz020, published April 23, 2019. [DOI: 10.1093/sysbio/syz020](https://doi.org/10.1093/sysbio/syz020)

TH Jukes and CR Cantor. 1969. Evolution of protein molecules. Pages 21-132 in HN Munro (ed.), Mammalian Protein Metabolism. Academic Press, New York.