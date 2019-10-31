---
layout: steps
title: The QMatrix class
partnum: 11
subpart: 3
description: Create the QMatrix, QMatrixNucleotide, and QMatrixCodon classes
---
Classes that inherit from the `QMatrix` class handle everything associated with the instantaneous rate matrix. The `Model` simply has to send the exchangeabilities and state frequencies to its data member `_qmatrix`, and the `QMatrix` objects stored there handle computing the eigenvalues, eigenvectors, and inverse eigenvectors that BeagleLib needs.

This tutorial emplements two different kinds of models: (1) the GTR+I+G model for nucleotide data and (2) a codon model. These two models have rate matrices that are different size (4x4 for GTR and 61x61 for a codon model using the standard code) and, rather than create one generic class to handle both, we will create an abstract base class `QMatrix` and two derived classes, `QMatrixNucleotide` (for the GTR case) and `QMatrixCodon` (for the codon case).

Begin by creating a new header file named {% indexfile qmatrix.hpp %} and add the `QMatrix` class declaration and member function bodies:
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"start-end_class_declaration,begin_derived_class_placeholders-end","" }}
~~~~~~
{:.cpp}

That `QMatrix` is an {% indexshow abstract class %} is clear from the fact that most member functions are set equal to 0 in the class declaration and no body is provided for them. Being abstract, this class cannot be used to create objects; we must derive other classes from it that provide bodies for the member functions.

The `setActive`, `clear`, `fixStateFreqs`, `fixExchangeabilities`, `fixOmega`, and `normalizeFreqsOrExchangeabilities` member functions are defined (i.e. have a body), which means that inherited classes will have their functionality also. 

## Constructor and destructor
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_qmatrix_constructor-end_qmatrix_destructor","" }}
~~~~~~
{:.cpp}

## The setActive member function

If `setActive` is passed `false`, then this `QMatrix` (or `QMatrix`-derived) object becomes inactive, meaning it will accept new state frequencies, exchangeabilities, or omega values but will not calculate anything with those values. This will be useful later when we run an MCMC analysis without data, as it allows us to avoid expensive eigensystem computations when the likelihood is not even being calculated. The boolean value passed into this function is assigned to the `_is_active` data member, and, if `_is_active` has just become true, it calls `recalcRateMatrix` just in case the eigenvalues and eigenvectors are not up-to-date. Because `recalcRateMatrix` is a virtual function, the appropriate implementation of `recalcRateMatrix` will be used (`QMatrixNucleotide::recalcRateMatrix` vs. `QMatrixCodon::recalcRateMatrix`).
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_setActive-end_setActive","" }}
~~~~~~
{:.cpp}

## The clear member function

This function provides initial values for the four data members (`_is_active`, `_state_freqs_fixed`, `_exchangeabilities_fixed`, and `_omega_fixed`) that are defined in this base class. 
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_clear-end_clear","" }}
~~~~~~
{:.cpp}

## The fixStateFreqs, fixExchangeabilities, and fixOmega member functions

These three functions provide a way to change the values of the `_state_freqs_fixed`, `_exchangeabilities_fixed`, and `_omega_fixed` data members, respectively.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_fixStateFreqs-end_fixOmega","" }}
~~~~~~
{:.cpp}

## The isFixedStateFreqs, isFixedExchangeabilities, and isFixedOmega member functions

These three functions are accessors for the values of the `_state_freqs_fixed`, `_exchangeabilities_fixed`, and `_omega_fixed` data members, respectively.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_isFixedStateFreqs-end_isFixedOmega","" }}
~~~~~~
{:.cpp}

## The normalizeFreqsOrExchangeabilities member function

This function takes a shared pointer to either a state frequencies vector or an exhangeabilities vector and normalizes the vector so that its elements sum to 1.0.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_normalizeFreqsOrExchangeabilities-end_normalizeFreqsOrExchangeabilities","" }}
~~~~~~
{:.cpp}

# The QMatrixNucleotide class

We will keep all three classes in this same {% indexfile qmatrix.hpp %} file, so go ahead and add the declaration for the `QMatrixNucleotide` class after the body of `QMatrix::setActive` but before the right curly bracket that closes the `strom` namespace.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide-end_QMatrixNucleotide_class_declaration","" }}
~~~~~~
{:.cpp}
I will provide explanations for the member functions in `QMatrixNucleotide`, but then just provide the code without explanation for the `QMatrixCodon` class: the explanations are the same; it is only the actual implementations that differ.

The [Eigen library](eigen.tuxfamily.org) is used to do the work of computing eigenvalues and eigenvectors, which explains the `#include <Eigen/Dense>` at the top of the file. The typedefs `eigenMatrix4d_t` and `eigenVector4d_t` simplify specify matrices and vectors, respectively, that are used with the Eigen library. You'll note that these types specify vectors of length 4 and 4 by 4 matrices, so we are clearly designing our `QMatrixNucleotide` class to handle only 4-state nucleotide data. 

## The constructor, destructor, and clear functions

The constructor and destructor are trivial, as usual. The `clear` function sets up the parameters `_exchangeabilities` and `_state_freqs` according to the JC model. Model parameters are stored as shared pointers so that they can be easily linked across data subsets. 
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_constructor-end_QMatrixNucleotide_clear","" }}
~~~~~~
{:.cpp}

## The getExchangeabilitiesSharedPtr and getStateFreqsSharedPtr member functions

These two functions are accessors for `_exchangeabilities` and `_state_freqs`, respectively.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_getExchangeabilitiesSharedPtr-end_QMatrixNucleotide_StateFreqsSharedPtr","" }}
~~~~~~
{:.cpp}

## The getOmegaSharedPtr member function

You may be thinking that a member function named `getOmegaSharedPtr` makes no sense in the `QMatrixNucleotide` class. You are correct, but we must implement it anyway, otherwise `QMatrixNucleotide` would be an abstract class like `QMatrix` and we would not be able to create objects from it. Because it makes no sense to call this function, we use an `assert(false)` to let us know if this function is ever used and return `nullptr` to avoid a compiler error (we must return a pointer, even if it points to nothing!).
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_getOmegaSharedPtr-end_QMatrixNucleotide_getOmegaSharedPtr","" }}
~~~~~~
{:.cpp}

## The getEigenvectors, getInverseEigenvectors, and getEigenvalues member functions

These are accessors that return pointers to the C-style arrays beneath the hood of the {% indexcode Eigen::Matrix %} data members `_eigenvectors`, `_inverse_eigenvectors`, and `_eigenvalues`, respectively. The reason we do not return references to the `Eigen::Matrix` objects directly is that these functions are used to fill buffers in BeagleLib, which expects C-style arrays.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_getEigenvectors-end_QMatrixNucleotide_getEigenvalues","" }}
~~~~~~
{:.cpp}

## The getExchangeabilities and getStateFreqs member functions

These two functions return pointers to the C-style arrays beneath the hood of the data members `_exchangeabilities` and `_state_freqs`, respectively. The reason we do not return the shared pointers `_exchangeabilities` and `_state_freqs` themselves is that these functions are used to fill buffers in BeagleLib, which expects C-style arrays. In order to obtain the C-style array from a {% indexcode std::shared_ptr %} pointing to a {% indexcode std::vector %}, first obtain a reference to the vector pointed to (`*`), then obtain the memory address (`&`) of the first element of the vector (`[0]`).
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_getExchangeabilities-end_QMatrixNucleotide_getStateFreqs","" }}
~~~~~~
{:.cpp}

## The getOmega member function

This function should not be called, as the `QMatrixNucleotide` class does not use the omega parameter, so the function uses `assert(false)` to catch any usage in debug mode and returns 0.0.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_getOmega-end_QMatrixNucleotide_getOmega","" }}
~~~~~~
{:.cpp}

## The setEqualExchangeabilities member function

This function sets all 6 exchangeabilities to 1/6 and calls the `recalcRateMatrix` function before returning.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_setEqualExchangeabilities-end_QMatrixNucleotide_setEqualExchangeabilities","" }}
~~~~~~
{:.cpp}

## The setExchangeabilitiesSharedPtr member function

This function copies the supplied shared pointer to a vector of exchangeabilities (`xchg_ptr`) to the data member `_exchangeabilities` (after checking to make sure it is the correct length), then calls the `recalcRateMatrix` function before returning.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_setExchangeabilitiesSharedPtr-end_QMatrixNucleotide_setExchangeabilitiesSharedPtr","" }}
~~~~~~
{:.cpp}

## The setExchangeabilities member function

This function copies the supplied vector of exchangeabilities (`xchg`) to the vector pointed to by the data member `_exchangeabilities` (after checking to make sure it is the correct length), then calls the `recalcRateMatrix` function before returning.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_setExchangeabilities-end_QMatrixNucleotide_setExchangeabilities","" }}
~~~~~~
{:.cpp}

## The setEqualStateFreqs member function

This function sets all 4 state frequencies to 0.25 and calls the `recalcRateMatrix` function before returning.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_setEqualStateFreqs-end_QMatrixNucleotide_setEqualStateFreqs","" }}
~~~~~~
{:.cpp}

## The setStateFreqsSharedPtr member function

This function copies the supplied shared pointer to a vector of state frequencies (`freq_ptr`) to the data member `_state_freqs` (after checking to make sure it is the correct length). It then calls the `recalcRateMatrix` function before returning.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_setStateFreqsSharedPtr-end_QMatrixNucleotide_setStateFreqsSharedPtr","" }}
~~~~~~
{:.cpp}

## The setStateFreqs member function

This function copies the supplied vector of state frequencies (`freqs`) to the vector pointed to by the data member `_state_freqs` (after checking to make sure it is the correct length). It then calls the `recalcRateMatrix` function before returning.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_setStateFreqs-end_QMatrixNucleotide_setStateFreqs","" }}
~~~~~~
{:.cpp}

## The setOmegaSharedPtr amd setOmega member functions

Because the omega parameter is not used in the GTR model, these two functions should not be called at all, which explains why their only line is `assert(false)` to catch any usage of these functions in the debug version of the program.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_setOmegaSharedPtr-end_QMatrixNucleotide_setOmega","" }}
~~~~~~
{:.cpp}

## The recalcRateMatrix member function

This function requires more explanation than any other, and I’ve broken down the discussion into sections.

### GTR rate matrix

The instantaneous rate matrix **Q** for the GTR model has 6 exchangeability parameters (_a_, _b_, _c_, _d_, _e_, and _f_) and 4 nucleotide frequencies (&pi;<sub>A</sub>, &pi;<sub>C</sub>, &pi;<sub>G</sub> and &pi;<sub>T</sub>). It is stored in the data member `_qmatrix`.

![Q matrix for the GTR model]({{ /assets/img/Qgtr.png | relative_url }}){:.center-math-image}

The rows represent the "from" state (in the order A, C, G, T, from top to bottom), while the columns represent the "to" state (also in the order A, C, G, T, from left to right). The diagonal elements of this matrix are negative and equal to the sum of the other elements in the same row because any change from state A, for example, to a different state must reduce the amount of A, hence the rate of change is negative. The row sums are zero because this model assumes that the sequence length neither shrinks nor grows, and thus any increase in C, G, or T must occur at the expense of A.

### Matrices related to nucleotide frequencies

Two important diagonal matrices are defined by the vector of nucleotide frequencies stored in `_state_freqs`. These are shown below and are stored in the data members `_sqrtPi` (top) and `_sqrtPiInv` (bottom).

![Q matrix for the GTR model](../../assets/img/sqrtPi.png){:.center-math-image}

### Diagonalization of the rate matrix

The rate matrix **Q** can be represented as the matrix product of the (right) eigenvector matrix **V**, the diagonal matrix of eigenvalues **L**, and the inverse eigenvector matrix **V**<sup>-1</sup>.

![Q matrix for the GTR model](../../assets/img/Qdiagonalization.png){:.center-math-image}

### Computing the transition probability matrix

The transition probability matrix **P** is obtained by exponentiating the product of **Q** and time _t_. The matrix **Q** is scaled so that time _t_ can be measured in units of expected substitutions per site, which is why the rates in the **Q** matrix omit the overall rate of substitution and only include relative rate terms (the overall rate is subsumed in _t_). The calculation of **P** involves exponentiating only the middle matrix (diagonal matrix of eigenvalues multiplied by _t_).

![Q matrix for the GTR model](../../assets/img/Pdiagonalization.png){:.center-math-image}

### Scaling the rate matrix

The **Q** matrix must be scaled so that time _t_ is measured in expected number of substitutions per site. If this scaling is not performed, the edge lengths lose their interpretation as expected number of substitutions per site. The scaling factor needed may be obtained by effectively performing the following matrix multiplication and afterwards summing the off-diagonal elements: dividing each element of **Q** by this sum yields the desired normalization.

![Q matrix for the GTR model](../../assets/img/Qscaling.png){:.center-math-image}

### Symmetrizing the rate matrix

Computing eigenvalues and eigenvectors of a symmetric matrix is simpler and more efficient than computing them for an asymmetric matrix. Hence, it is common to obtain eigenvalues and eigenvectors of a symmetrical matrix **S** derived from the **Q** matrix rather than for the **Q** matrix itself. The eigenvalues for **S** and **Q** are identical, and the eigenvector matrix **W** obtained from **S** may be easily converted into the desired matrix **V** containing the eigenvectors of **Q**. Note that **V** may be obtained by premultiplying **W** by the diagonal matrix of inverse square roots of nucleotide frequencies.

![Q matrix for the GTR model](../../assets/img/Sdiagonalization.png){:.center-math-image}

## The recalcRateMatrix function

With all that in mind, hopefully the `recalcRateMatrix` function will be easier to understand.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixNucleotide_recalcRateMatrix-end_QMatrixNucleotide_recalcRateMatrix","" }}
~~~~~~
{:.cpp}

# The QMatrixCodon class
The codon model implemented in this tutorial is very simple. It allows rates to differ depending on whether a change is synonymous or nonsynonymous (the omega parameter governs the nonsynonymous/synonymous rate ratio) and it allows codon frequencies to be unequal, which allows for codon usage bias but is a bit costly computationally due to the fact that the state frequency vector has 61 elements (or 60 or 62, depending on the genetic code used). Our codon model does not take transition/transversion bias into account, but that could be fairly easily added. 

Here is the `QMatrixCodon` class in its entirety. Some explanation of how the rate matrix is constructed follows the code; explanations of the `QMatrixNucleotide` functions above apply equally well to most of the `QMatrixCodon` functions, with the exception of those member functions pertaining to the omega parameter, which of course are implemented in `QMatrixCodon`, while in this case it is the exchangeabilities that are not used (and `assert(false)` statements are used to detect any calls made to functions that get or set exchangeabilities). 

Note that we are using `Eigen::VectorXd` now as the type of `_eigenvalues` instead of the `Eigen::Vector4d` that we used in `QMatrixNucleotide`. The `Xd` means that the size of the vector is not hard coded, which is important because codon models vary in the number of states depending on which genetic code applies (due to variation in the number of stop codons). Also, the `Eigen::Matrix` type that is used for `_eigenvectors` and other matrix data members has `Eigen::Dynamic` instead of `4` as template arguments. This generality (with its added computational cost) is not required for the GTR model, so the polymorphism of the `QMatrix` family allows us to use fixed-size matrices and vectors in `QMatrixNucleotide` to make the eigensystem calculations more efficient for the very common case of nucleotide data and GTR-like models.
~~~~~~
{{ "steps/step-11/src/qmatrix.hpp" | polcodesnippet:"begin_QMatrixCodon-end_QMatrixCodon_recalcRateMatrix","" }}
~~~~~~
{:.cpp}

The element i,j of the instantaneous rate matrix in this codon model is 0 if codon states i and j differ by more than 1 nucleotide. The line 
~~~~~~
_Q = Eigen::MatrixXd::Zero(nstates,nstates);
~~~~~~
{:.bash-output}
initializes all elements of the rate matrix `_Q` to zero, so we only have to change those cells in which codons i and j are identical or differ by just one nucleotide. If the two codons differ by one nucleotide, and if the amino acid coded by both codons is identical, then this is a synonymous change and the rate is 1 times the frequency of codon j (the codon resulting from the change). If the "from" and "to" codons differ by just one nucleotide _and_ the amino acid coded by each differs, then this is a nonsynonymous change and the rate would be omega times the frequency of codon j (the "to" codon). If codons i and j are identical, then the rate is the negative sum of all other rates in the same row.

As was done for the nucleotide version, the rate matrix is calibrated so that one unit of time equals one nucleotide substitution per site. This calibration allows us to treat edge lengths (in units of substitutions per site) as if they were time. The calibration involves calculating the average rate implied by the matrix before calibration and then dividing each element of the rate matrix by `scaling_factor`, which is this average rate. We must also multiply by the factor 3 to account for the fact that each codon state is 3 nucleotides long.

