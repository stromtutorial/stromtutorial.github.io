---
layout: steps
title: Specifying a model
partnum: 11
subpart: 1
description: How the user will specify a partitioned model.
---
So far in the tutorial, I have waited to update the user interface until after introducing all the code needed to implement the new feature. This time, however, it seems prudent to begin with the user interface, as that will dictate the structure and functioning of the new `Model` class.

### Configuration file setup
From this point on, the program will always be run using a configuation file: it will be impossible to put everything on the command line that needs to be specified except in cases of very simple models. 

We need to be able to specify potentially different model parameters for each partition subset. To do this, we will allow the user to enter constructs such as this in the configuration file:
~~~~~~
rmatrix = first,second:1,1,1,1,1,1
rmatrix = third:1,2,1,1,2,1
~~~~~~
{:.bash-output}
This says that we wish the first and second subsets to have the same GTR exchangeabilities (the so-called R matrix) and they are specified to be those values compatible with the JC69 model (i.e. all 1). Also, we wish the third subset to have GTR exchangeabilities compatible with a model that allows transitions to evolve twice as fast as transversions.

As you can see, our approach will not use named models. That is, we will _not_ specify that a GTR+I+G model will be used for subsets first and second and an F81+G model will be used for the third subset. Instead, we specify which subsets have distinct model components. In the example above, we specified the actual R matrix values. Those will serve as the starting values in our (future) MCMC analyses, but for now it allows us to set precise values for each parameter that can be used to test whether the likelihood calculation machinery is working correctly (and that we've set up the likelihood machinery correctly). 

The user may wish for some parameters to have different values for each subset. Here is an example involving the proportion of invariable sites:
~~~~~~
pinvar      = first,second:0.1
pinvar      = third:0.5
~~~~~~
{:.bash-output}

Parameters can (but are not required to) have the same values across all subsets. For example, this statement in the configuration file would specify that all three defined partition subsets have state frequencies 0.1, 0.2, 0.3, and 0.4.
~~~~~~
statefreq = first,second,third:.1,.2,.3,.4
~~~~~~
{:.bash-output}

It will be very tedious to list all subset names if there are many subsets, so we will also allow these constructs:
~~~~~~
statefreq = default:.1,.2,.3,.4
rmatrix = default:1,1,1,1,1,1
rmatrix = third:1,2,1,1,2,1
~~~~~~
{:.bash-output}
The default settings apply to any partition not specifically singled out to have its own set of parameter values. If used, the default settings must come before any subset-specific definitions.

If a parameter is not even mentioned in the configuration file, then it will receive default values that are linked across all subsets. For example, if no `statefreq` statement appears in the configuration file, the following is assumed:
~~~~~~
statefreq = default:.25,.25,.25,.25
~~~~~~
{:.bash-output}

Finally, it could become tedious for the user to specify the values for some parameters. For example, there are 61 state frequencies in a codon model using the standard genetic code, and it seems excessive to ask the user to provide all 61, especially if the user just wants to start off with equal codon frequencies. Thus, the keyword `equal` is allowed in place of the list of values for state frequencies (`statefreq`). For example, this is equivalent to the previous `statefreq` statement:
~~~~~~
statefreq = default:equal
~~~~~~
{:.bash-output}

## Updating the configuration file
Modify the {% indexfile strom.conf %} file to contain the text provided below. You can omit the 6 comment lines at the top if you want; these just provide an overview of the pattern of parameter linking across subsets. The numbers in parentheses are default values that are set automatically but which are not used because that parameter is not even present in the model for that subset (rate variance is irrelevant for the `second` subset because there is just 1 gamma category; the proportion of invariable sites is irrelevant for the `third` subset because that model assumes all sites are variable since no non-zero proportion of invariable sites parameter value was assigned).
~~~~~~
{{ "steps/step-11/test/strom.conf" | polcodesnippet:"","" }}
~~~~~~
{:.bash-output}
