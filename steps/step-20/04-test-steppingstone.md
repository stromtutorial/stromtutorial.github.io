---
layout: steps
title: Test steppingstone
partnum: 20
subpart: 4
description: Test the steppingstone method
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

Replace your `strom.conf` file with the following version. 
~~~~~~
datafile         = rbcl10.nex
treefile         = rbcl10.tre
tree             = default:1
subset           = default[nucleotide]:1-1314
statefreq        = default:0.25, 0.25, 0.25, 0.25
ratevar          = default:2.0
rmatrix          = default:1.0, 1.0, 1.0, 1.0, 1.0, 1.0
ncateg           = default:4
pinvar           = default:[0.0]
nchains          = 20
burnin           = 1000
niter            = 100000
samplefreq       = 10
printfreq        = 1000
seed             = 76543
usedata          = yes
gpu              = no
ambigmissing     = yes
underflowscaling = yes
allowpolytomies  = no
resclassprior    = yes
topopriorC       = 1.0
steppingstone    = yes
ssalpha          = 0.25
~~~~~~
{:.bash-output}

## Expected result

The configuration file specifies that steppingstone should be performed using 20 chains. The model is GTR+G, and the marginal likelihood is -6801.52128. The run took about 3.5 minutes on my laptop using the release version. Try setting ncateg to 1, which eliminates the among-site rate heterogeneity component of the model. Is the marginal likelihood higher without rate heterogeneity or with rate heterogeneity accommodated? (Spoiler: you should find that the marginal likelihood is higher for the model that allows rate heterogeneity by more than 397 log units.)

{% comment %}
GTR+G = -6801.52128
GTR   = -7199.10046
-------------------
diff  =   397.57918
{% endcomment %}

## The end!

This is the current end of the tutorial. I continue to add to it, however, so more steps will be added in future editions. I hope you have found this tutorial to be helpful, and please feel free to contact me (`paul.lewis@uconn.edu`) if you find errors or have difficulty understanding something.

If I fix errors, I will increment the minor version number and describe (as best I can) what changed in the Version History section of the source branch README file.

Also, remember that the source code is available separately for each step of the tutorial on the GitHub repository. Visit [https://github.com/stromtutorial/stromtutorial.github.io](https://github.com/stromtutorial/stromtutorial.github.io) and select the `source` branch, then look for the source for the step in question in the `src` directory for that step. You can also checkout the entire repository and switch to the source branch as follows:

~~~~~~
git clone https://github.com/stromtutorial/stromtutorial.github.io.git
cd stromtutorial
git checkout source
~~~~~~
{:.bash}
