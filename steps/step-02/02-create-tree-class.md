---
layout: steps
title: Create the Tree Class
partnum: 2
subpart: 2
description: Declare the `Tree` class and define its member functions in the header file *tree.hpp*
---
{% assign OS = site.operatingsystem %}

The next step is to create a C++ class encapsulating the notion of a phylogenetic tree.

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################

Create the header file {% indexfile tree.hpp %} in the {% indexfile src %} directory and fill it with the following text:

{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################

Create the header file {% indexfile tree.hpp %} in the same way you created {% indexfile node.hpp %}, then replace the default contents of {% indexfile tree.hpp %} with the following code and save the file.

{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################

Follow the procedure you used to create {% indexfile node.hpp %} to create the header file {% indexfile tree.hpp %} and fill it with the following text:

{% endif %}
[//]: ################################# ENDIF ######################################

~~~~~~
{{ "steps/step-02/src/tree.hpp" | polcodesnippet:"start-end","" }}
~~~~~~
{:.cpp}

## Explanation of *tree.hpp*
### Headers
Just after the `pragma` directive (explained earlier) are three lines that include header files. The contents of each of these header files (headers, for short) are effectively dumped into this file at the point where they are invoked. The `memory` header file provides the definition of `std::shared_ptr` (explained below). The `iostream` header file provides the definition of `std::cout`, which is used in the constructor and destructor function bodies. Finally, The `node.hpp` header file provides our definition of the `Node` class. The headers that are surrounded by `<angled brackets>` are system headers, while those surrounded by `"quotes"` are provided by you. This convention helps the compiler, which will generally look for system header files in a different place than user-provided headers.

### Inline keyword
You may note that the member functions are all labeled with the `inline` keyword. The `inline` keyword is a friendly request made to the compiler that you would like the function body to be simply copied into the place where it is called, which is often more efficient than a function call. The compiler gets to decide, however, whether the body of a particular function will actually be inlined at a particular place in the code.

### Constructor and destructor
The class declares a public constructor and a public destructor. As we saw for the `Node` class, the constructor is responsible for initializing the memory set aside to store a new `Tree` object, and the destructor is in charge of cleaning up the object before the memory for the object is released. Our `Tree` constructor calls the member function `clear()` to initialize the data members of the `Tree` class. I have (temporarily) placed code in both constructor and destructor to write output. This output will allow us to easily see when a `Tree` object is being constructed or destructed. We will comment out these output lines after we test the class.

### Member functions
The member function `clear()` is provided to restore a `Tree` object to its just-constructed state.

The functions `isRooted()` and `numLeaves()` are called accessors or accessor functions because they simply make the value of private data members available. You might ask why make `_is_rooted` and `_nleaves private` if you are going to allow public access to their values via these accessor functions. The answer is that this allows us to make the values of these data members available publicly without allowing their values to be changed. The only objects that can change `_nleaves` and `_is_rooted` are objects of class `Tree` or objects of classes that have been declared friends of `Tree`.

Finally, the member function `numNodes()` might be called a utility function because it calculates something, namely the length of the `_nodes` vector. It is a {% indexbold utility function %} rather than an accessor function because it does more work than simply returning the value of a data member.

### Data members
A boolean data member `_is_rooted` indicates whether the `Tree` object should be considered rooted or unrooted. A `Node` pointer `_root` will point to the `Node` object serving as the root node of the `Tree` object (note that every tree has a root node, even if `_is_rooted` is `false`: the distinction between rooted and unrooted lies in how the likelihood is computed, not in how the tree is stored in memory). An unsigned integer `_nleaves` stores the number of tip nodes (leaves) in the tree. Finally, two standard vector data members store nodes. The vector `_nodes` stores the `Node` objects themselves, while the vector `_preorder` stores pointers to the `Node` objects in `_nodes`.

### Shared pointers
A `SharedPtr` type is defined that represents a {% indexbold shared pointer %} (also known as a {% indexbold smart pointer %}) to our `Tree` class. Smart pointers keep track of how many references there are to a particular object. Once no other object is holding on to a reference (i.e. is pointing to) an object, the smart pointer takes care of deleting the object automatically. This makes memory management much easier because you, as the programmer, do not have to remember to delete objects that are no longer being used so long as you always manage them via smart pointers.

Note that the type name we’ve defined is just `SharedPtr`, which is not specific to `Tree`. The fact that this type is defined within the `Tree` class declaration, however, means that we will always know the object referred to by this `SharedPtr` because to use it we will have to qualify it by the class name: `Tree::SharedPtr tree`.

### Friends
As for the `Node` class, the `TreeManip` and `Likelihood` classes will need to have access to the private member functions and data members of the `Tree` class. Because `TreeManip` and `Likelihood` have not yet been created, these friend {% indexhide friend %} declarations are currently commented out.

