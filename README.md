## What is the Strom Tutorial?
The Strom Tutorial shows how to create C++ software for estimating phylogenetic trees using Bayesian MCMC. It is intended to provide a means for getting up to speed if you seriously want or need to learn how phylogenetics software works. A use case would be a graduate student interested in developing and testing a new phylogenetic method for their thesis research, or a postdoctoral researcher who obtained a Ph.D. in systematics but is beginning work in a laboratory that develops new phylogenetic methods.

## How does the Strom tutorial web site work?
The code stored in source branch of this repository is used to regenerate the web site for the tutorial. The static web site itself is stored in the master branch and is served as a GitHub Pages site. Jekyll is used to regenerate the static content. There are currently three versions of the tutorial:
- the **Linux version** (uses the meson/ninja build system and ilustrates how to write software without using an integrated development environment (IDE)
- the **Mac version** (illustrates how to develop the software using the Xcode IDE)
- the **Windows version** (illustrates how to develop the software using the Visual Studio IDE)

### Source code for each step

Note that it is possible to obtain the source code for any step in the tutorial from the source branch. Thus, if you ever want to use a particular step in the tutorial as the starting point for a project, you can do that by copying the _src_ directory associated with that step.

### Version history

#### 3.0

This is the first version of the tutorial that uses the GitHub Pages web site. The former
version of the tutorial is still available at [phylogeny.uconn.edu/tutorial-v2/](https://phylogeny.uconn.edu/tutorial-v2/). Version 3 of the tutorial adds data partitioning to the model.

#### 3.1

This update fixes two bugs. First, the Hastings ratio was incorrect in `TreeUpdater::proposeNewState`. Rather than fix the Hastings ratio, it seemed more reasonable to modify the proposal so that it was symmetric and only modifies edge length proportions, not edge lengths themselves. This accords with the fact that the model parameterizes edge lengths into a tree length and a vector of edge length proportions. Now, `TreeUpdater` modifies only the edge length proportions (and possibly the tree topology), while `TreeLengthUpdater` modified only the tree length. Second, the Gamma-Dirichlet prior on tree length and edge length proportions was being applied twice (once by `TreeUpdater` and then again by TreeLengthUpdater). Now, `Chain::calcLogJointPrior` fixes this by not letting `TreeLengthUpdater` contribute to the calculation of the joint prior.