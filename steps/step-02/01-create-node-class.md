---
layout: steps
title: Create the Node Class
partnum: 2
subpart: 1
description: Declare the `Node` class and define its member functions in the header file *node.hpp*
---
{% assign OS = site.operatingsystem %}

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################
Create the header file {% indexfile node.hpp %} in the {% indexfile src %} directory and fill it with the following text:

{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################

In the Xcode Project Navigator pane on the left, you should now see your strom project (beside a blue project icon :blueproject:), and inside that you should see a strom folder (beside a yellow folder icon :yellowfolder:), and inside that you should see a file named _main.cpp_. (If you do not see anything like this, it may be because your Project Navigator pane is not visible. You can always get it back using _View > Navigators > Show Project Navigator_ from the main menu.)

Command-click the strom folder (be sure to click the strom folder, with the yellow icon :yellowfolder:, not the strom project) and choose _New File..._ from the popup menu.

Select _Header File_ and press the _Next_ button. Name the new file _node.hpp_, check the _strom_ target, and press the _Create_ button to save _node.hpp_ in the strom folder.

You should now have a strom folder inside your strom project, and inside the strom folder you should see 2 files: {% indexfile node.hpp %} and {% indexfile main.cpp %}.

Replace the default contents of {% indexfile node.hpp %} with the following code and save the file.

{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################
## Creating a header file

*Important: whenever I ask you to create a new header file, use the procedure below. Right now, we are just creating files; you will fill in these files later in the tutorial.*

1. Right-click on the _Header Files_ folder inside the _strom_ project and choose _Add > New Item..._, then choose _Header File (.h)_, set the _Name_ field to {% indexfile node.hpp %} and press the _Add_ button
1. If you forgot to set the name of the header file before creating it, you can easily change the name of the default header file created from {% indexfile Header.h %} to {% indexfile node.hpp %} by right-clicking its name and choosing _Rename_ from the popup menu.

Now that you have the header file {% indexfile node.hpp %}, fill it with the following code.
{% endif %}
[//]: ################################# ENDIF ######################################
~~~~~~
{{ "steps/step-02/src/node.hpp" | polcodesnippet:"start-end","" }}
~~~~~~
{:.cpp}

## Explanation of *node.hpp*
### Pragmas
The first line is a {% indexhide pragma %} `pragma`, which is an instruction to the compiler that can be either used or ignored, depending on the compiler. The `once` pragma says that this header file should not be included more than once.

### Including header files
The {% indexhide include %} `#include <string>` statement near the beginning of the file causes the code defining a standard string object to be inserted at that location in {% indexfile node.hpp %}, replacing the `#include <string>` line as if you had carried out a search and replace operation. This allows us to create and use objects of type `std::string` inside {% indexfile node.hpp %} (and any file other that specifies `#include "node.hpp"`). Similarly, `#include <vector>` allows us to use the `std::vector` container to create arrays of Node objects, and `#include <iostream>` allows us to use `std::cout` objects to output information to the console.

You have probably noticed the `#include "split.hpp"` statement that has been commented out by preceding it with a double slash (`//`). We will uncomment this line later after we create the {% indexfile split.hpp %} file.

### Namespaces
The `Node` class is wrapped in a {% indexshow namespace %} just in case this class is used along with other code that defines a class of the same name. The namespace we will be using to wrap everything in this tutorial is {% indexhide strom %} `strom`, which means "tree" in Czech. Note that the `std` in `std::string` is also a namespace. In order to use a standard string, you must qualify the name string with the namespace (i.e. `std::string`). While we will not do it, if you were to define a `string` class inside {% indexfile node.hpp %}, you would need to refer to it as `strom::string` outside the `strom` namespace.

### Data members
A {% indexshow data member %} is a variable defined for objects of a particular C++ class. Note that all data members have names beginning with a single underscore character. This is not required by C++ but is a convention that will be used throughout to make it easy to recognize data members as opposed to local variables or function parameters.

#### Node data members
The data members of the {% indexhide Node class %} `Node` class include three pointers to other `Node` objects.

`_parent`

: The `_parent` pointer points to the ancestor of the current `Node`. If `_parent` is 0, then the current `Node` is the root node.

`_left_child`

: The `_left_child` pointer points to the left-most child node of the current `Node` in the tree. If `_left_child` is 0, then the `Node` is a leaf node in the tree.

`_right_sib`

: Finally, the `_right_sib` pointer points to the next sibling `Node` on the right. If `_right_sib` is 0, then the current `Node` is the rightmost child of its ancestor.

There are four other data members of the `Node` class.

`_name`

: This is a string that represents the taxon name of a leaf and is often (but not necessarily) an empty string for interior nodes.

`_number`

: This is the node number, which serves as an index into the `Tree::_nodes` vector.

`_edge_length`

: This is the length of the edge between this `Node` and its ancestor (i.e. `_parent`).

`_smallest_edge_length`

: This is the length of the smallest allowable edge length. This is a static data member, which means that it exists and its value can be set and used even if no object of this class is ever created. As such it functions as a global variable that can be used anywhere, but has the advantage of still being nestled within the `Node` class so there is no danger of it being confused with some global variable with the same name introduced by, for example, a third-party header file. Because it is static, we don’t initialize it in the construtor; instead, it will be initialized in {% indexfile main.cpp %}, which is our only source code file (all other files in this project are header files).

Finally, there is one data member (`_split`) that is currently commented out. You will uncomment this line later.

### Member functions
#### Accessors and setters
The `Node` class has several {% indexshow member functions %}. Most of these functions are {% indexbold accessors %}: they provide access to the private data members that are not otherwise accessible except by a member function of the `Node` class itself. The accessor functions are `getParent()`, `getLeftChild()`, `getRightSib()`, `getNumber()`, `getName()`, `getEdgeLength()`, and (commented out for now) `getSplit()`.

Note that these functions are defined (i.e. their function body is provided) directly in the class declaration (i.e. the part between `class Node` and `};`). This is fine for really simple functions, but for functions with even slightly more complicated bodies, we will move the bodies further down in the file to avoid making the class declaration too difficult to comprehend at a glance.

The member function `setEdgeLength` is a {% indexbold setter %}: it sets the value of a particular data member to the specified floating point value. The function body is a bit too long to include in the class declaration, so its body is provided near the bottom (but inside the `strom namespace` code block). This function enforces a minimum length (`_smallest_edge_length`) for any edge.

#### Constructors and destructors
Two member functions are special: the {% indexbold constructor %} function and the {% indexbold destructor %} function. You can identify these by the fact that they have no return type (not even `void`!) and their names are identical to the class name. The constructor is called automatically when a new object of the class is created, so it is the logical place for doing setup tasks that should be done before an object is used for the first time. The destructor is called automatically when the object is being deleted, and is the logical place for cleanup tasks that should be done before the object goes away.

The constructor defined here just reports that an object of type `Node` has been created, and then calls the `clear()` function to initialize data members. The destructor simply reports that a `Node` object is being destroyed. We will eventually comment out these `std::cout` statements to avoid cluttering the output, but for now it is nice to be able to see when objects are created and destroyed.

### Public versus private
{% indexhide public vs. private %} You might wonder "why don’t we just make all data members public?" It is always wise to expose as little as possible to the outside world. Accessors provide read-only access to anyone who is interested, but do not allow just anyone to make changes to the data members of the `Node` class. This makes it harder for someone (maybe even yourself at a later time!) who does not fully understand your code to introduce errors when modifying it.

We will soon find that some classes use private members of `Node` to such an extent that we will make each of these classes a {% indexbold friend %} of `Node`. Any class declared as a `friend` of `Node` has full access to private data members. Because the classes `Tree` (which comprises `Node` objects), `TreeManip` (whose purpose is to manipulate `Tree` objects), and `Likelihood` (whose purpose is to compute the likelihood of a tree) do not yet exist, these `friend` declarations are currently commented out.

