---
layout: steps
title: The Lot Class
partnum: 13
subpart: 1
description: Create a class whose objects can generate pseudorandom numbers. 
---
{{ page.description | markdownify }}

MCMC involves random numbers at several levels. Proposed changes to the tree topology, edge lengths, or other substitution model parameters require random numbers, as does the decision to accept or reject a proposed change. The {% indexcode Lot %} class will generate the pseudorandom numbers that will be used to make these decisions.

The main member functions we will use are:

__setSeed__

> Providing an integer to this function completely determines the sequence of pseudorandom numbers that will be issued by this object, allowing for exact replication of an analysis.
   
__uniform and logUniform__

> These functions provide random deviates from a Uniform(0,1) distribution, with the latter function returning the logarithm of a Uniform(0,1) random deviate.

__normal__

> Provides a standard normal, i.e. Normal(0,1), random deviate.

__gamma__

> Provides a random deviate from a Gamma distribution with the specified shape and scale (mean equals shape*scale in this parameterization)

__randint__

> Provides a random draw from a discrete uniform distribution such that the returned value is greater than or equal to low and less than or equal to high.

Create a new file {% indexfile lot.hpp %} and populate it with the following class declaration. 
~~~~~~
{{ "steps/step-13/src/lot.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

All of the heavy lifting in our `Lot` class will be done by the Boost random library. While I will briefly describe the member functions, I refer you to the [Boost random documentation](https://www.boost.org/doc/libs/1_65_1/doc/html/boost_random.html) for further details.

## The constructor and destructor
Boost random offers a choice of several pseudorandom number generators. I have chosen to use the [Mersenne Twister 19937](https://en.wikipedia.org/wiki/Mersenne_Twister). This is why the `_generator` data member has type {% indexcode boost::mt19937 %}. The constructor sets the seed of the Mersenne Twister using the current time. It is always a good idea to set your own seed so that an analysis can be repeated exactly if needed, so a `setSeed` member function is also provided (see below).

The constructor also creates 4 function objects (variate generators) that can translate the pseudorandom numbers output by the Mersenne Twister into random deviates from particular probability distributions. The 4 specialized functors are: 
* `_uniform_variate_generator`, which produces Uniform(0,1) random deviates,
* `_normal_variate_generator`, which produces Normal(0,1) random deviates,
* `_gamma_variate_generator`, which produces Gamma(`_gamma_shape`, 1) random deviates, and
* `_uniform_int_generator`, which produces discrete uniform deviates in the range `_low` to `_high`, inclusive.
All are stored as shared pointers and so must be dereferenced to be called as functions. 

The data member `_gamma_shape` is initialized to 1 and represents the shape parameter of the gamma distribution encapsulated by `_gamma_variate_generator`. It is possible to change the value of `_gamma_shape` using the gamma member function. 

The data members _low, and _high store parameters of the discrete uniform distribution encapsulated by `_uniform_int_generator`. It is possible to change the values of `_low` and `_high` using the `randint` member function.

The destructor resets all 4 shared pointers so that the objects they point to can be destroyed.
~~~~~~
{{ "steps/step-13/src/lot.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The setSeed member function
This function allows you to set the seed used by the `Lot` object's `_generator`. If the seed supplied is equal to 0, the seed is taken from the system clock.
~~~~~~
{{ "steps/step-13/src/lot.hpp" | polcodesnippet:"begin_setSeed-end_setSeed","" }}
~~~~~~
{:.cpp}

## The uniform member function
This function calls the `_uniform_variate_generator` functor to generate a draw from a Uniform(0,1) distribution.
~~~~~~
{{ "steps/step-13/src/lot.hpp" | polcodesnippet:"begin_uniform-end_uniform","" }}
~~~~~~
{:.cpp}

## The logUniform member function
This function calls the `_uniform_variate_generator` functor to generate a draw from a Uniform(0,1) distribution, then returns the log of that value.
~~~~~~
{{ "steps/step-13/src/lot.hpp" | polcodesnippet:"begin_logUniform-end_logUniform","" }}
~~~~~~
{:.cpp}

## The normal member function
This function calls the `_normal_variate_generator` functor to generate a draw from a Normal(0,1) (i.e. Standard Normal) distribution.
~~~~~~
{{ "steps/step-13/src/lot.hpp" | polcodesnippet:"begin_normal-end_normal","" }}
~~~~~~
{:.cpp}

## The gamma member function
If the `shape` parameter equals `_gamma_shape`, this function calls the `_gamma_variate_generator` functor to generate a draw from a Gamma(`shape`,1) distribution, then returns that value multiplied by the `scale` parameter. If the `shape` parameter does not equal `_gamma_shape`, then `_gamma_variate_generator` is first set to point to a new `gamma_variate_generator_t` object with the new `shape` parameter and `_gamma_shape` is set to `shape`. 
~~~~~~
{{ "steps/step-13/src/lot.hpp" | polcodesnippet:"begin_gamma-end_gamma","" }}
~~~~~~
{:.cpp}

## The randint member function

If the `low` parameter equals `_low` and the `high` parameter equals `_high`, this function calls the `_uniform_int_generator` functor to generate a draw from a Discrete Uniform(`_low`,`_high`) distribution. If either the `low` or `high` parameter does not equal its data member counterpart, then `_uniform_int_generator` is first set to point to a new `_uniform_int_generator_t` object with the new `low` and `high` parameters, and `_low` and `_high` are set to `low` and `high`, respectively. 
~~~~~~
{{ "steps/step-13/src/lot.hpp" | polcodesnippet:"begin_randint-end_randint","" }}
~~~~~~
{:.cpp}

