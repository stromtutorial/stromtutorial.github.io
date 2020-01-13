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

Now all that is needed is to make changes to the `Node` and `TreeManip` classes to accommodate flipping transition matrices and partials, make `Updater` a friend of `Tree` and `Node`, and update the `Strom::run` function to set up `Chain` and run it. For this exercise we will use another variation on the rbcL data set used before.

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

## Adding selectability to the Node class

Edit {% indexfile node.hpp %} and uncomment the `class Updater;` and `friend class Updater;` lines that are currently commented out. In addition, add the methods and data structures highlighted below in bold, blue text:
~~~~~~
{{ "steps/step-14/src/node.hpp" | polcodesnippet:"start-end","a,b,c-cc,d-dd,e,f" }}
~~~~~~
{:.cpp}
The `Flag` enum provides binary attributes for every node that can be turned on or off. The `select()`, `selectPartial()`, `selectTMatrix()`, `setAltPartial()`, and `setAltTMatrix()` member functions set (i.e. make the value 1) bits at positions 1, 2, 3, 4, and 5, respectively, while the member functions `deselect()`, `deselectPartial()`, `deselectTMatrix()`, `clearAltPartial()`, and `clearAltTMatrix()` unset (i.e. make the value 0 at) the same bit positions. As an example, suppose a node has 0 at all bit positions except position 4 (`Flag::AltPartial`). Here is what the node's flag data member looks like in binary representation (showing only the 5 bits relevant to this discussion):
~~~~~~
flags = 0 1 0 0 0
        | | | |  \Selected 
        | | |  \SelPartial
        | |  \SelTMatrix
        |  \AltPartial
         \AltTMatrix
~~~~~~
{:.bash-output}
If `selectTMatrix()` is called for this node, the result will be to set the 3rd bit (from the right) in addition to the bit already set in the 4th position (due to the fact that this is an OR operation):
~~~~~~
flags = 0 1 0 0 0
      | 0 0 1 0 0
-----------------
        0 1 1 0 0    
        | | | |  \Selected 
        | | |  \SelPartial
        | |  \SelTMatrix
        |  \AltPartial
         \AltTMatrix
~~~~~~
{:.bash-output}
If `clearAltPartial()` is called for this node, the result will be to unset only the 4th bit (note that `~(0 1 0 0 0) = 1 0 1 1 1`:
~~~~~~
flags = 0 1 1 0 0
      & 1 0 1 1 1
--------------------
        0 0 1 0 0
        | | | |  \Selected 
        | | |  \SelPartial
        | |  \SelTMatrix
        |  \AltPartial
        \AltTMatrix
~~~~~~
{:.bash-output}

If bit operations still seem confusing, take a look at [this explanation](https://en.wikipedia.org/wiki/Bitwise_operations_in_C).

These member functions allow us to:
* select or deselect a node (which we will not be using in the tutorial but which you may find useful for debugging - e.g. selected nodes can be enumerated or displayed differently than unselected nodes);
* select or deselect a node's transition matrix (to trigger recalculation after the node's edge length is modified);
* select or deselect a node's partial array (to trigger recalculation if anything above the node has changed);
* set or clear the alt status of a node's transition probability matrix (the alt state causes a different memory location in BeagleLib to be used to store the transition matrix);
* set or clear the alt status of a node's partials array (the alt state causes a different memory location in BeagleLib to be used to store the partials); and
* flip either the transition matrix or partials array to the alt state or back again (we will flip the state of relevant nodes before calculating the likelihood for a proposed update, then flip back again if the proposal was rejected).

## Add functions to TreeManip to select or deselect nodes

The added Node functions are normally called by `TreeManip` functions. Add the indicated function prototypes to the `TreeManip` class declaration:
~~~~~~
{{ "steps/step-14/src/tree_manip.hpp" | polcodesnippet:"begin_class_declaration-end_class_declaration","a-aa" }}
~~~~~~
{:.cpp}

Add the bodies of these functions somewhere below the class declaration and before the curly bracket closing the namespace:
~~~~~~
{{ "steps/step-14/src/tree_manip.hpp" | polcodesnippet:"begin_selectAll-end_flipPartialsAndTMatrices","" }}
~~~~~~
{:.cpp}

## Modifying the Likelihood class to allow alternate partials and transition matrix indices

The ability to set a flag for a node indicating that an alternate partials index (or transition matrix index) is only useful if the `Likelihood` class pays attention to these flags. The first step is to reserve enough memory in BeagleLIb to accommodate alternate sets of indices. The highlighted lines below double the amount of memory allocated for partials, transition matrices, and scaler buffers.
~~~~~~
{{ "steps/step-14/src/likelihood.hpp" | polcodesnippet:"begin_newInstance-end_newInstance","a,b,c" }}
~~~~~~
{:.cpp}

Next, modify `getScalerIndex`, `getTMatrixIndex`, and `getPartialIndex` so that they return the alternate indices when the appropriate node flags are set. Replace the bodies of each of these three functions with the versions below:
~~~~~~
{{ "steps/step-14/src/likelihood.hpp" | polcodesnippet:"begin_getScalerIndex-end_getTMatrixIndex","" }}
~~~~~~
{:.cpp}

Finally, tell `defineOperations` to only recalculate transition matrices or partials if the node is selected. This will be important later when only a few nodes are affected by a proposal (no sense in recalculating the partial likelihoods for a clade if that clade was not involved in the MCMC proposal). 
~~~~~~
{{ "steps/step-14/src/likelihood.hpp" | polcodesnippet:"begin_defineOperations-end_defineOperations","d,e,f,ff" }}
~~~~~~
{:.cpp}

## Make Updater a friend of Tree

Edit {% indexfile tree.hpp %} and uncomment the `class Updater;` and `friend class Updater;` lines that are currently commented out.

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
{{ "steps/step-14/src/strom.hpp" | polcodesnippet:"begin_run-end_run","j,k-kk,l-ll,m-mm,n-nn,o-oo,p-pp" }}
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

