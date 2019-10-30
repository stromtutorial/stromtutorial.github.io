---
layout: steps
title: Testing the Chain Class
partnum: 14
subpart: 4
description: Perform an MCMC analysis to test the new Chain class
---
{% assign OS = site.operatingsystem %}
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

Now all that is needed is to make some small changes to the `Node`, `Tree`, and `TreeManip` classes, and update the `Strom::run` function to set up `Chain` and run it. For this exercise we will use another variation on the rbcL data set used before.

Download [rbcl10.nex]({{ "/assets/data/rbcl10.nex" | absolute_url }}) and [rbcl10.tre]({{ "/assets/data/rbcl10.tre" | absolute_url }}) to your working directory with the other downloaded data and tree files.

Here are maximum likelihood estimates (using PAUP* Version 4.0a, build 159) of the GTR+G model parameters using the data stored in {% indexfile rbcl10.nex  %} and the tree topology stored in {% indexfile rbcl10.tre %}. You can refer back to this table once your program begins estimating these parameters to see how the values compare. Remember, however, that your program is Bayesian and thus prior distributions will affect estimates, and the maximized log-likelihood computed by PAUP* will (almost surely) be higher than any log-likelihood you will see in your output. I have modified the output produced by PAUP* 
below by adding a second column (values in parentheses) in which I provide:

* a normalized version the GTR exchangeabilities (PAUP* sets GT to 1 and makes all other exchangeabilities relative to GT); and 

* the rate variance (1/shape) instead of the shape parameter reported by PAUP*

In short, if all is working, we should see our MCMC analysis hovering (after a few iterations) around the rate variance 3.94 and a log-likelihood a little less than -6723.144.
~~~~~~
-ln L     6723.144
Base frequencies:
  A       0.294667
  C       0.189172
  G       0.206055
  T       0.310106
Rate matrix R:
  AC       1.25798 (0.06082)
  AG       5.76823 (0.27887)
  AT       1.33643 (0.06461)
  CG       1.29145 (0.06244)
  CT      10.03035 (0.48492)
  GT       1.00000 (0.04834)
Shape     0.253596 (3.94322)
~~~~~~
{:.bash-output}

## Making Updater a friend of Node and Tree

Edit {% indexfile node.hpp %} and {% indexfile tree.hpp %} and uncomment the `class Updater;` and `friend class Updater;` lines that are currently commented out.

## Modifying the Model class
Make the following blue-highlighted additions to the `Model` class.
~~~~~~
{{ "steps/step-14/src/model.hpp" | polcodesnippet:"begin_class_declaration-end_class_declaration","a-b,c-d,e-f" }}
~~~~~~
{:.cpp}

Add these 5 function bodies somewhere before the right curly bracket that closes the `strom` namespace in {% indexfile model.hpp %}:
~~~~~~
{{ "steps/step-14/src/model.hpp" | polcodesnippet:"begin_getStateFreqParams-end_getPinvarParams","" }}
~~~~~~
{:.cpp}

Make the following additions to the `describeModel` function in {% indexfile model.hpp %}. These additions specify which parameters are variable in the model and allow the `Chain` class to determine what specific parameter updaters are needed.
~~~~~~
{{ "steps/step-14/src/model.hpp" | polcodesnippet:"begin_describeModel-end_describeModel","g-gg,h-hh,i-ii,j-jj,k-kk,l-ll" }}
~~~~~~
{:.cpp}

## Modifying the Strom class declaration

Modify {% indexfile strom.hpp %} by adding the bold, blue parts below to the `Strom` class declaration:
~~~~~~
{{ "steps/step-14/src/strom.hpp" | polcodesnippet:"start-end_class_declaration","a-b,c,d-e" }}
~~~~~~
{:.cpp}

I've added several data members: `_lot` (the pseudorandom number generator), `_random_seed` (the starting seed for the pseudorandom number generator), `_num_iter` (the number of parameter updates that our Markov chain simulator will attempt, `_sample_freq` (the frequency with which parameter values will be saved), and `_print_freq` (the frequency with which a progress report will be issued to the console). The last one (`_print_freq`) will be ignored by our program for now; we will begin using it in the next step to control how much output appears on the screen as the program is running (we will not want a report after every iteration if we've asked for 10 million iterations!).

## Modifying the Strom clear member function

Modify {% indexfile strom.hpp %} by adding the bold, blue parts below to the `clear` member function:
~~~~~~
{{ "steps/step-14/src/strom.hpp" | polcodesnippet:"begin_clear-end_clear","f-g" }}
~~~~~~
{:.cpp}

## Modifying the Strom processCommandLineOptions member function

Continue modifying {% indexfile strom.hpp %} by adding the bold, blue parts below to the `Strom::processCommandLineOptions` function:
~~~~~~
{{ "steps/step-14/src/strom.hpp" | polcodesnippet:"begin_processCommandLineOptions-end_processCommandLineOptions","h-i" }}
~~~~~~
{:.cpp}

## Modifying the Strom run member function

Our final modifications to {% indexfile strom.hpp %} will be to add the bold, blue parts below to the `Strom::run` function:
~~~~~~
{{ "steps/step-14/src/strom.hpp" | polcodesnippet:"begin_run-end_run","j,k-kk,l-ll,m-mm,n-nn,o-oo" }}
~~~~~~
{:.cpp}

## Replacing the main function

You will also need to return {% indexfile main.cpp %} to the way it was before we changed it to test the `Lot` class. Replace everything in {% indexfile main.cpp %} with the following:
~~~~~~
{{ "steps/step-14/src/main.cpp" | polcodesnippet:"start-end","" }}
~~~~~~
{:.cpp}

## Revising your strom.conf file

Replace the contents of {% indexfile strom.conf %} with the following. This sets all of the model parameters to the values PAUP* estimated with the exception of ratevar, which we will start at 1.0 rather than 3.94280 so we can test whether our rate variance updater is working. 

We also set the number of rate categories to 4. This is necessary because, otherwise, the model would assume rate homogeneity and our one updatable parameter (gamma rate variance) would not be updated! 
~~~~~~
{{ "steps/step-14/test/strom.conf" | polcodesnippet:"start-end","" }}
~~~~~~
{:.bash-output}

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################
## Revising your meson.build file

Add the lines indicated below to your {% indexfile meson.build %} file. These 2 new lines copy {% indexfile rbcl10.nex %} and {% indexfile rbcl10.tre %} to your install directory. 
~~~~~~
{{ "steps/step-14/src/meson.build" | polcodesnippet:"","a,b" }}
~~~~~~
{:.meson}
Compile and install `strom`, as usual, by navigating into your build directory and typing `meson install`.

{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################

{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################

{% endif %}
[//]: ################################# ENDIF ######################################

## Expected output

You should see output from 1000 calls to the `nextStep` function of the `Chain` object: I've shown only the first 10 iterations (plus the very last iteration) of this output below
~~~~~~
{{ "steps/step-14/test/reference-output/output.txt" | polsnippet:"75-86,1076","" }}
~~~~~~
{:.bash-output}

Note that the acceptance percentage for our gamma rate variance updater quickly converges to 30%, which is the target we specified by default in the `Chain` constructor (actually, in `Chain::clear` function, which is called by the constructor). Right now, we are tuning throughout the MCMC run, but soon we will change things so that autotuning is done for our updaters only during the burn-in period.

Also note that the likelihood improves as the gamma rate variance converges from the starting value of 1.0 to a value close to the maximum likelihood estimate (MLE) 3.94280, suggesting that our MCMC chain is converging on a region of parameter space having higher posterior probability density. The log-likelihood also approaches (but of course can never exceed) the maximum likleihood of -6723.144 as the chain runs.

Finally, notice that the lnPrior column is always equal to -1 times the gamma rate variance value. This is because the prior is completely determined by the prior on the gamma rate variance parameter (our model currently only has one estimated parameter). This gamma rate variance prior is, by default, Gamma(1,1), which is specified in `Chain::clear`. The log of this probability density is `-log(1) – ratevar/1`, which just equals `-ratevar`.

