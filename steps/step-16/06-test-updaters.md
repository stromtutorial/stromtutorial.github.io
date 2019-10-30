---
layout: steps
title: Test the New Updaters
partnum: 16
subpart: 6
description: Perform MCMC analyses that test all of the new Updater-derived classes. 
---
{% assign OS = site.operatingsystem %}
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

## Add the new updaters to Chain::createUpdaters

In order to use the new Updater-derived classes, they must be added to the Chain class `_updaters` vector. This is done in `Chain::createUpdaters`.
~~~~~~
{{ "steps/step-16/src/chain.hpp" | polcodesnippet:"begin_createUpdaters-end_createUpdaters","begin_StateFreqUpdater-end_StateFreqUpdater,begin_ExchangeabilityUpdater-end_ExchangeabilityUpdater,begin_PinvarUpdater-end_PinvarUpdater,begin_OmegaUpdater-end_OmegaUpdater,begin_SubsetRelRateUpdater-end_SubsetRelRateUpdater" }}
~~~~~~
{:.cpp}

Be sure to add the appropriate headers to the top of the {% indexfile chain.hpp %} file:
~~~~~~
{{ "steps/step-16/src/chain.hpp" | polcodesnippet:"start-begin_namespace","a-b" }}
~~~~~~
{:.cpp}

## Run strom

### Nucleotide model analysis

Copy the following into your {% indexfile strom.conf %} file. This tests all updaters except for `OmegaUpdater` (we will test that one separately below).
~~~~~~
{{ "steps/step-16/test/strom-nucleotide.conf" | polcodesnippet:"start-end","" }}
~~~~~~
{:.cpp}

Run strom and verify that all model parameters are being updated (except tree topology and edge lengths) by viewing the {% indexfile params.txt %} file in the program [Tracer](https://beast.community/tracer). You should find that rAC-0, rAC-1, and rAC-2 all have identical values at every iteration. This is because we specified only one set of starting values for `rmatrix` and assigned these to `default`, which means that one set of exchangeabilities applies to all subsets. In other words, we linked exchangeabilities across subsets.

On the other hand, we allowed the third codon position subset to have its own ratevar and pinvar parameters. so you should find that pinvar-0 and pinvar-1 are always identical but pinvar-2 takes a different trajectory (and ratevar-0 and ratevar-1 are identical while ratevar-2 is different).

Our program treats subset relative rates as a single multivariate parameter, so you can either fix these or allow them to vary, but if you allow them to vary, they all will vary. Note that the rate of 3rd codon position sites is more than 7 times higher than the rate of 1st position sites and more than 8 times higher than 2nd position sites, which makes sense given that rbcL is a strongly conserved protein coding gene that allows few nonsynonynous substitutions.

### Codon model analysis

To estimate the nonsynonymous/synonymous rate ratio directly, specifying a single codon subset rather than 3 nucleotide subsets in your `strom.conf` file as shown below:
~~~~~~
{{ "steps/step-16/test/strom-codon.conf" | polcodesnippet:"start-end","" }}
~~~~~~
{:.cpp}

The codon model will take much longer than the nucleotide analysis - this is typical unless your BeagleLib is able to take advantage of a GPU resource, which speeds up codon models proportionately more than nucleotide models due to the difference in the number of states (61 for the codon model vs. 4 for the nucleotide models). 

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################
Because the codon model is so slow, you may wish to create a {% indexbold release version %} of the program that is optimized for speed and use that executable to run the codon example. To do this, add `buildtype=release` to the default options in your {% indexfile meson.build %} file, as shown below:
~~~~~~
{{ "steps/step-16/src/meson.build" | polcodesnippet:"","a" }}
~~~~~~
{:.meson}

The options possible for the default_options list can be found at the [built-in options section of the Meson web site](https://mesonbuild.com/Builtin-options.html).

{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################
Because the codon model is so slow, you may wish to create a {% indexbold release version %} version of the program that is optimized for speed and use that executable to run the codon example. To do this in Xcode, choose _Archive_ from the _Product_ menu. (You should have set up a post-action script in step 1 of the tutorial so that your release executable will be automatically saved to a directory in your PATH.)

{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################

{% endif %}
[//]: ################################# ENDIF ######################################

You should find that omega has a posterior mean of about 0.02 to 0.03. This small nonsynonymous/synonymous rate ratio implies that this gene is under strong stabilizing selection and mutations that change the amino acid in the corresponding protein are rarely preserved compared to mutations that have no effect on the protein primary structure.


