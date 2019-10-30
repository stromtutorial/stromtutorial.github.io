---
layout: steps
title: Test building trees
partnum: 6
subpart: 3
description: In this section you test buildFromNewick function and will see how to handle run-time exceptions
---
To test both the `buildFromNewick` and the `makeNewick` function, change {% indexfile main.cpp %} to look like this:
~~~~~~
{{ "steps/step-06/src/main.cpp" | polcodesnippet:"start-end","" }}
~~~~~~
{:.cpp}

The main function now creates a tree from the newick string
~~~~~~
(1:0.3,2:0.3,(3:0.2,(4:0.1,5:0.1):0.1):0.1)
~~~~~~
{:.bash-output}
and then uses the `makeNewick` function to turn the tree in memory into a newick string, which is output. The program is working correctly if the output string represents the same tree as the input string (note that the number of decimal places used to depict edge lengths will differ, but the tree descriptions should depict exactly the same tree topology and edge lengths).

## Test of rerootAtNodeNumber function
These two lines in {% indexfile main.cpp %} test whether the rerootAtNodeNumber function works correctly.
~~~~~~
{{ "steps/step-06/src/main.cpp" | polcodesnippet:"a-b","" }}
~~~~~~
{:.cpp}
These lines ask the program to reroot using the node having `_number` equal to 4, which is actually the number labeled 5 in the newick string.

## Expected output
Here is the output that your program should produce. See the next section if you have problems running the program.
~~~~~~
{{ "steps/step-06/test/reference-output/output.txt" | polcodesnippet:"","" }}
~~~~~~
{:.bash-output}

## If your program halts at `assert(!nd->_left_child)`...

Assert statements test assumptions that you are making as you write source code. Any time you find yourself thinking "I'm pretty sure that will never happen", then it is probably time to use an `assert` statement to ensure that, if it does happen, you know about it and can find where it happened and under what circumstances it happened. Assert statements disappear in optimized code, so you will only see these trip if you are running the debug (non-optimized) version of your program (usually inside the IDE).

The fact that `assert` statements do not end up in the optimized (non-debug) version of your program means that you should use exceptions rather than asserts for assumptions you are making about the behavior of _users_ of your program. Asserts help protect your program from yourself; exceptions help protect your program from your users. {% indexhide asserts vs. exceptions %}

If your program trips the `assert(!nd->_left_child);` inside the `TreeManip::buildFromNewick` function, then almost certainly what has happened is that you've not removed all traces of `Tree::createTestTree()`, which you were instructed to do in the very last ("Before moving on...") section of the page [Create the TreeManip class]({{ "steps/step-04/01-create-treemanip-class.html" | relative_url}}). The problem is that your `Tree` constructor is building a default tree every time it is called, so you are never starting from a clean slate with respect to the tree that you are building. Thus, you must remove the call to `createTestTree()` in the `Tree::Tree` constructor and reinstate the `clear()` function there.

## Handling exceptions

If you would like to test the exception mechanism, try introducing an error in the newick tree description. For example, changing taxon "3" to "Z". This will create an exception because only newick descriptions with taxon names that can be converted to positive integers are allowed:
~~~~~~
std::string newick = std::string("(1:0.3,2:0.3,(Z:0.2,(4:0.1,5:0.1):0.1):0.1)");
~~~~~~
{:.cpp}
Running the program now should produce the following error message:
~~~~~~
Error: node name (Z) not interpretable as a positive integer
~~~~~~
{:.bash-output}
(Be sure to change the "Z" back to a "3" before continuing.)

