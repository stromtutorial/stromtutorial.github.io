---
layout: steps
title: Test tree creation
partnum: 3
subpart: 2
description: Test your new `createTestTree()` function.
---
Try running the program again. You should get an error message that says something like cannot access private member declared in class `strom::Node`. This is because we are trying to change data members (such as `_left_child`) in `Node` objects from outside the `Node` class, and this is not allowed because those data members were declared in the private section of the `Node` class declaration.

We could easily fix this by simply commenting out the `private:` statement in the `Node` class declaration, thus making all those previously private attributes public. This is not a very elegant way of dealing with the problem because it eliminates all the benefits of having private data members in the first place (i.e. it prevents you or other programmers using your code from unintentionally modifying these variables).

Because `Tree` objects will legitimately need to make changes to their `Node` objects, we can make the `Tree` class a friend of the `Node` class. Friend classes are allowed to change the values of private data members. This effectively creates an exception to the privacy rules we established when creating the `Node` class.

You need to uncomment two lines in {% indexfile node.hpp %} to make `Tree` a friend of `Node`. Uncomment the bold blue lines shown below by removing the `//` at the beginning of each line:
~~~~~~
{{ "steps/step-03/src/node.hpp" | polcodesnippet:"start-end","a,b"}}
~~~~~~
{:.cpp}
The first line simply assures the compiler that a class named `Tree` exists. Note that we have not included the {% indexfile tree.hpp %} header file, so the compiler does not have any idea what a `Tree` object is at this point. The alternative to adding this line would be to include the file {% indexfile tree.hpp %}, but that is complicated by the fact that {% indexfile tree.hpp %} includes {% indexfile node.hpp %}! The declaration above solves this chicken-and-egg issue, specifying exactly what `Tree` is in a situation where the compiler does not need to know anything more about `Tree` than the fact that it is the name of a class defined somewhere.

The second line tells the compiler that the class `Tree` is allowed to do whatever it likes with the private data members of `Node`.

After uncommenting those two lines in {% indexfile node.hpp %}, running the program should now produce this output:
~~~~~~
Starting...
Constructing a Tree
Creating Node object
Creating Node object
Creating Node object
Creating Node object
Creating Node object
Creating Node object

Finished!
Destroying a Tree
Destroying Node object
Destroying Node object
Destroying Node object
Destroying Node object
Destroying Node object
Destroying Node object
~~~~~~
{:.bash-output}

Note that 1 `Tree` object and 6 `Node` objects were created and 1 `Tree` object and 6 `Node` objects were destroyed when the program finished.

## Something to try
Try changing the tree by adding a node `fourth_leaf` as sister to `third_leaf`.

