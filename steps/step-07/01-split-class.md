---
layout: steps
title: The Split class
partnum: 7
subpart: 1
description: The `Split` class provides an object that can store the set of taxa that represent descendants of a given `Node` object. We will provide a `Split` object for every `Node`.
---
{{ page.description | markdownify }}

Create a new class named `Split` in a header file named {% indexfile split.hpp %}.
~~~~~~
{{ "steps/step-07/src/split.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

### Constructors and destructor

Here are the bodies of the two constructors and the destructor. Copy these into the {% indexfile split.hpp %} header file just above the closing curly bracket defining the end of the `strom` namespace.
~~~~~~
{{ "steps/step-07/src/split.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

The destructor for this class simply reports that a `Split` object is being destroyed. There are two constructors, each of which sets the value of the `_bits_per_unit` data member to the number of bits in the data type specified by `split_unit_t`. The calculation involves multiplying the constant {% indexcode CHAR_BIT %} (number of bits in a byte) by the number of bytes in the data type specified by `split_unit_t`. (`CHAR_BIT` is a constant provided in the climits header file.) This value will determine how many units of that data type will be needed to store a 0 or 1 for each taxon. These units are stored in the vector data member `_bits`, which has type `split_t`. (Besides `split_unit_t` and `split_t`, two other types are defined here, namely `treeid_t` and `treemap_t`, but I will postpone explaining these until later when they are used in the `TreeSummary` class)

For example, suppose `split_unit_t` is defined to be an integer of type `char`, which is 8 bits in length. If there are 10 taxa, then 2 `char` units will be required to store each split. A split that specifies that taxa having indices 0, 2, 6, and 9 are descendants of a node would be encoded as follows (note that bits within each unit are counted starting from the right, and because there are 10 taxa and 16 bits total, the leftmost 6 bits of unit 1 will always be set to 0):
~~~~~~
  6   2 0       9   <-- taxon index
|01000101|00000010| <-- _bits vector
| unit 0 | unit 1 |
~~~~~~
{:.bash-output}
In our case, `split_unit_t` is defined to be an `unsigned long integer` rather than a `char`. On my computer, a `char` comprises only 8 bits and is the smallest integer type, whereas an `unsigned long` has 64 bits. Thus, we can get by with fewer units if we use `unsigned long` rather than `char`.

#### Default constructor versus copy constructor
The first constructor defined above is the default constructor. It sets `_nleaves` to 0 and calls the `clear` function to set everything else to their default values. The second constructor is a {% indexbold copy constructor %}. It sets the `_nleaves` data member to the value of `_nleaves` in  `other`, the reference to which is supplied as an argument to the copy constructor. It also copies the `_bits` vector from `other`. As a result, the new split will be an exact copy of `other`.

### The clear member function

The `clear` function simply sets all bits in all units of the `_bits` vector to 0. The for loop assigns the reference `u`, in turn, to each unit in `_bits`. It is important that the variable `u` is a reference: if we used `for (auto u : _bits)` instead, then each unit would be *copied* to `u` and any changes made to `u` in the body of the {% indexhide auto %}loop would affect the copy but not the original!

The `L` in `0L` specifies that we are assigning a long integer (long integers typically comprise twice as many bits as an ordinary integer) with value 0 to `u`: there is little consequence if we forget the `L` because the compiler would automatically convert `0` to `0L` for us, but it is always best to be as explicit as possible.
~~~~~~
{{ "steps/step-07/src/split.hpp" | polcodesnippet:"begin_clear-end_clear","" }}
~~~~~~
{:.cpp}

## Operators

We will eventually need to sort splits, and sorting algorithms need to be able to ask whether one `Split` object is less than another `Split` object, and also need to know if two `Split` objects are identical. Special functions called {% indexbold operators %} are used to provide this functionality. Once these operators are defined, you can treat `Split` objects like ordinary numbers with respect to the operators involved: for example, this code would work:
~~~~~~
(Note: this is an example; do not add this code to split.hpp!)

Split a;
Split b;
if (a < b)
  std::cout << "Split a is less than Split b";
else if (a == b)
  std::cout << "Split a equals Split b";
else
  std::cout << "Split a is greater than Split b";
~~~~~~
{:.bash-output}
The equals and less-than operators both compare the current object to another `Split` object for which the reference `other` is provided. Note that we are assuming that two `Split` objects are equal if their `_bits` vectors are equal, and one `Split` object is less than another `Split` object if its `_bits` vector is less than `other._bits`. For the less-than operator, we assert that the two `_bits` vectors being compared have the same number of elements (the `operator<` defined for standard containers such as `std::vector` do not provide any warning if the containers being compared are of different lengths).

We also define an assignment operator that allows us to easily copy one `Split` object to another. Note the difference between `operator=` (assignment operator) and `operator==` (equivalence operator): the first copies one object to another and returns a reference to the new copy, while the second asks whether one object is equal to another object and returns a bool (true or false) result.
~~~~~~
{{ "steps/step-07/src/split.hpp" | polcodesnippet:"begin_assignment-end_lessthan","" }}
~~~~~~
{:.cpp}

## The resize member function

Given the number of taxa (`_nleaves` below), we can use `_bits_per_unit` to determine how many units the `_bits` vector should have. For example, if `_bits_per_unit` was 8, then up to 8 taxa could be represented by a `_bits` vector of length 1; however 9 taxa would require the `_bits` vector to have length 2. Dividing `_nleaves - 1` by `_bits_per_unit` and adding 1 provides the correct number of units. If `_nleaves` = 8, (8-1)/8 = 0.875, which is truncated to 0 when converted to an integer, so adding 1 to (8-1)/8 yields the correct number of units (1). If `_nleaves` = 9, (9-1)/8 = 1.0, which is truncated to 1 when converted to an integer, and adding 1 to (9-1)/8 yields the correct number of units (2). Once `nunits` is computed, the function simply resizes `_bits` to have length `nunits` and then calls the `clear` function to ensure that all bits in all units are set to 0.
~~~~~~
{{ "steps/step-07/src/split.hpp" | polcodesnippet:"begin_resize-end_resize","" }}
~~~~~~
{:.cpp}

## The setBitAt member function

I'll first show you the `setBitAt` function, then explain it in detail below the code block:
~~~~~~
{{ "steps/step-07/src/split.hpp" | polcodesnippet:"begin_setBitAt-end_setBitAt","" }}
~~~~~~
{:.cpp}
The way to set the 6th bit involves shifting the value 1 to the left 6 places using the {% indexbold bit shift operator %}. The bit shift operator is made up of two less-than symbols (`<<`), and is the same operator that you have used to output text to an output stream such as `std::cout`. Here are some examples assuming that the data type char (8 bits) is used:
~~~~~~
(Note: this is an example; do not add this code to split.hpp!)

char zero = (char)0;
char one  = (char)1;
zero looks like this: |00000000|
one looks like this:  |00000001|
one << 0 produces |00000001|
one << 1 produces |00000010|
one << 2 produces |00000100|
one << 3 produces |00001000|
one << 4 produces |00010000|
one << 5 produces |00100000|
one << 6 produces |01000000|
one << 7 produces |10000000|
~~~~~~
{:.bash-output}
To set a specific bit in a value that may already have some bits set, use the {% indexbold bitwise OR %} operator:
~~~~~~
(Note: this is an example; do not add this code to split.hpp!)

char x = 13;
x looks like this:     |00001101|
char y = 1 << 4
y looks like this:     |00010000|
char z = x | y;
z looks like this:     |00011101|
x |= y;
x now looks like z:    |00011101|
~~~~~~
{:.bash-output}
Note that `x | y` simply sets all bits that are set in either `x` or `y`, and `x |= y` does the same but assigns the result back to `x`. Given that background, the function `setBitAt` should now be understandable. Given a `leaf_index` (where `leaf_index` ranges from 0 to the number of taxa minus 1), the function first determines which unit to modify (`unit_index`), then the specific bit within that unit to set (`bit_index`), and finally uses the bit shift and bitwise OR operators to set the bit.

### A subtle but important point about setting bits
Note that, in creating the `unity` variable, we cast the value 1 to be of type `split_unit_t` before using the bit shift operator (`<<`). This casting is important because the value 1 may be, by default, an integer type with fewer bits than `split_unit_t` type. If 1 has only 32 bits, but `split_unit_t` has 64, and we try `1 << 33`, then the program will set bits but the bits set will necessarily be less than 33 because there are only 32 bits available. Using a version of the value 1 that has 64 bits (by casting it to `split_unit_t`) prevents us from shifting the 1 out of the memory allocated to store the value. {% indexhide bit setting gotcha %}

{% comment %}
If you ever need to debug what is happening with bits, the `std::bitset` {% indexhide std::bitset %}template is very useful for displaying binary representations of values. For example, try inserting these lines inside your `main` function (e.g. before the line that outputs `Starting...`):
~~~~~~
// Show sizes of values (in bits):
std::cout << "8*sizeof(1)             = " << 8*sizeof(1) << std::endl;
std::cout << "8*sizeof(unsigned)      = " << 8*sizeof(unsigned) << std::endl;
std::cout << "8*sizeof(unsigned long) = " << 8*sizeof(unsigned long) << std::endl;

unsigned long a;
for (unsigned i = 0; i < 33; i++)
    {
    // Shift the value 1 to the left i positions and assign the resulting value to a
    a = (unsigned)1 << i;

    // Show what shift operation is being performed
    std::cout << boost::str(boost::format("     (unsigned)1 << %2d  | ") % i);

    // std::bitset<64> creates a bitset object with 64 bits
    // passing the value a into the constructor flips those bits set in a
    // outputting a bitset object to a stream shows us which bits were set in a
    std::cout << std::bitset<64>(a) << std::endl;
    }
// Finally, show that if you cast 1 to an unsigned long, things work as expected
a = (unsigned long)1 << 32;
std::cout << "(unsigned long)1 << 32  | " << std::bitset<64>(a) << std::endl;
~~~~~~
{:.cpp}

You will also need to include the header file:
~~~~~~
#include &lt;bitset&gt;
~~~~~~
{:.cpp}

Compiling and running your program should now output the following before the normal output:
~~~~~~
8*sizeof(1)             = 32
8*sizeof(unsigned)      = 32
8*sizeof(unsigned long) = 64
     (unsigned)1 <<  0  | 0000000000000000000000000000000000000000000000000000000000000001
     (unsigned)1 <<  1  | 0000000000000000000000000000000000000000000000000000000000000010
     (unsigned)1 <<  2  | 0000000000000000000000000000000000000000000000000000000000000100
     (unsigned)1 <<  3  | 0000000000000000000000000000000000000000000000000000000000001000
     (unsigned)1 <<  4  | 0000000000000000000000000000000000000000000000000000000000010000
     (unsigned)1 <<  5  | 0000000000000000000000000000000000000000000000000000000000100000
     (unsigned)1 <<  6  | 0000000000000000000000000000000000000000000000000000000001000000
     (unsigned)1 <<  7  | 0000000000000000000000000000000000000000000000000000000010000000
     (unsigned)1 <<  8  | 0000000000000000000000000000000000000000000000000000000100000000
     (unsigned)1 <<  9  | 0000000000000000000000000000000000000000000000000000001000000000
     (unsigned)1 << 10  | 0000000000000000000000000000000000000000000000000000010000000000
     (unsigned)1 << 11  | 0000000000000000000000000000000000000000000000000000100000000000
     (unsigned)1 << 12  | 0000000000000000000000000000000000000000000000000001000000000000
     (unsigned)1 << 13  | 0000000000000000000000000000000000000000000000000010000000000000
     (unsigned)1 << 14  | 0000000000000000000000000000000000000000000000000100000000000000
     (unsigned)1 << 15  | 0000000000000000000000000000000000000000000000001000000000000000
     (unsigned)1 << 16  | 0000000000000000000000000000000000000000000000010000000000000000
     (unsigned)1 << 17  | 0000000000000000000000000000000000000000000000100000000000000000
     (unsigned)1 << 18  | 0000000000000000000000000000000000000000000001000000000000000000
     (unsigned)1 << 19  | 0000000000000000000000000000000000000000000010000000000000000000
     (unsigned)1 << 20  | 0000000000000000000000000000000000000000000100000000000000000000
     (unsigned)1 << 21  | 0000000000000000000000000000000000000000001000000000000000000000
     (unsigned)1 << 22  | 0000000000000000000000000000000000000000010000000000000000000000
     (unsigned)1 << 23  | 0000000000000000000000000000000000000000100000000000000000000000
     (unsigned)1 << 24  | 0000000000000000000000000000000000000001000000000000000000000000
     (unsigned)1 << 25  | 0000000000000000000000000000000000000010000000000000000000000000
     (unsigned)1 << 26  | 0000000000000000000000000000000000000100000000000000000000000000
     (unsigned)1 << 27  | 0000000000000000000000000000000000001000000000000000000000000000
     (unsigned)1 << 28  | 0000000000000000000000000000000000010000000000000000000000000000
     (unsigned)1 << 29  | 0000000000000000000000000000000000100000000000000000000000000000
     (unsigned)1 << 30  | 0000000000000000000000000000000001000000000000000000000000000000
     (unsigned)1 << 31  | 0000000000000000000000000000000010000000000000000000000000000000
     (unsigned)1 << 32  | 0000000000000000000000000000000000000000000000000000000000000001
(unsigned long)1 << 31  | 0000000000000000000000000000000010000000000000000000000000000000
(unsigned long)1 << 32  | 0000000000000000000000000000000100000000000000000000000000000000
~~~~~~
{:.bash-output}
Notice how `(unsigned)1 << 32` is identical to `(unsigned)1 << 0`: that is not good!
{% endcomment %}

## The getBits member function
The `getBits` member function simply returns the unit at the supplied `unit_index`. This
accessor function is needed because the `_bits` vector is a private data member.
~~~~~~
{{ "steps/step-07/src/split.hpp" | polcodesnippet:"begin_getBits-end_getBits","" }}
~~~~~~
{:.cpp}

## The getBitAt member function

The `getBitAt` member function provides a way to query a split to find out if the bit
corresponding to a particular taxon is set. The taxon is specified using `leaf_index`.
See the documentation for the `createPatternRepresentation` function below for an explanation
of how this is determined.
~~~~~~
{{ "steps/step-07/src/split.hpp" | polcodesnippet:"begin_getBitAt-end_getBitAt","" }}
~~~~~~
{:.cpp}

## The addSplit member function

This function creates a union of the taxon set defined by this `Split` object and the taxon set defined by `other`, then assigns that union to this `Split` object. The for loop iterates through all elements of the `_bits` vector and bitwise ORs that unit with the corresponding unit from other. This function is useful when constructing splits for interior nodes of a tree: the bits set in an interior node are the union of all sets corresponding to its descendant nodes.
~~~~~~
{{ "steps/step-07/src/split.hpp" | polcodesnippet:"begin_addSplit-end_addSplit","" }}
~~~~~~
{:.cpp}

## The createPatternRepresentation member function

This function creates a coded representation of a split in which `*` represents bits that are "on" (i.e. set to 1) and `-` represents "off" bits (i.e. set to 0).

Here is an example involving 10 taxa where `bits_per_unit = 8` and taxa 1, 7 and 9 are "on" and taxa 0, 2-6, and 8 are "off":
~~~~~~
+--------+--------+
|10000010|00000010|
+--------+--------+
 _bits[0]  _bits[1]
~~~~~~
{:.bash-output}
Note the nasty tangle we have to navigate to write out a sensible split representation! First we read unit 0 from right to left: the 1 on the far left of unit 0 corresponds to the "on" bit for taxon 7. After unit 0 has been interpreted, move to unit 1: the rightmost bit in unit 1 (0) corresponds to the "off" bit for taxon 8. This split would be output as follows:
~~~~~~
        -*-----*-*
taxon 0 ^        ^ taxon 9
~~~~~~
{:.bash-output}
Note that we must be careful to avoid looking at the extra bits: the two units together contain 16 bits, yet there are only 10 taxa, so whatever is stored in the last 6 bits of `_bits[1]` should not be considered. The algorithm works by letting `i` iterate over units, letting `j` iterate over the bits within each unit, and testing whether bit `j` is set in `_bits[i]`.

For example, suppose i=0. Shifting 1 to the left j places yields, as j takes on values from 0 to 7:
~~~~~~
 1 << j     j
-------------
00000001    0
00000010    1
00000100    2
00001000    3
00010000    4
00100000    5
01000000    6
10000000    7
~~~~~~
{:.bash-output}
Now consider the result of a {% indexbold bitwise AND %} between `1 << j` (when `j=7`) and the value of `_bits[0]`:
~~~~~~
10000000    1 << j, where j=7
10000010    unit[0]
-----------------------------------
10000000    (1 << 7) & unit[0]
~~~~~~
{:.bash-output}
The bitwise AND operator (`&`) yields 1 for a bit position whenever the two numbers being compared both have a 1 in that position and produces 0 otherwise. The bitwise AND operator can thus be used to determine whether any particular bit position contains a 1 or 0.
~~~~~~
{{ "steps/step-07/src/split.hpp" | polcodesnippet:"begin_createPatternRepresentation-end_createPatternRepresentation","" }}
~~~~~~
{:.cpp}

## The isEquivalent member function

The `isEquivalent` member function returns true if this split and `other` are equivalent{% indexhide equivalent splits %}. Equivalence is less strict than equality. Two splits can be equivalent even if they are not equal because of the position of the root. For example, splits `a` and `b` are both equal and equivalent because `a & b == a == b`:
~~~~~~
split a: -***---*--
split b: -***---*--
  a & b: -***---*-- (equals both a and b)
~~~~~~
{:.bash-output}
Splits `c` and `d` are not equal but they are nevertheless equivalent: they specify the same bipartition of taxa because one is the complement of the other. Thus, equivalence means `c & ~d = c` and `~c & d = d`:
~~~~~~
split c: -****-*---
split d: *----*-***
     ~c: *----*-***
     ~d: -****-*---
  c & d: ----------
 c & ~d: -****-*--- (equal to c)
 ~c & d: *----*-*** (equal to d)
~~~~~~
{:.bash-output}
Splits `e` and `f`, on the other hand, are neither equal nor equivalent because `e & f` equals neither `e` nor `f`, `e & ~f` does not equal `e`, and `~e & f` does not equal `f`:
~~~~~~
split e: -***---*--
split f: ---***---*
  e & f: ---*------ (equals neither e nor f)
 e & ~f: -**----*-- (not equal to e)
 ~e & f: ----**---* (not equal to f)
~~~~~~
{:.bash-output}
Here is the body of the `isEquivalent` function:
~~~~~~
{{ "steps/step-07/src/split.hpp" | polcodesnippet:"begin_isEquivalent-end_isEquivalent","" }}
~~~~~~
{:.cpp}

## The isCompatible member function

The `isCompatible` member function returns true if this split and `other` are compatible {% indexhide compatible splits %}, which means that they can co-exist in the same tree. The two splits `a` and `b` are incompatible if `a & b` is nonzero and also not equal to either `a` or `b`. For example, these two splits
~~~~~~
  split a: -***---*--
  split b: ----***--*
    a & b: ----------
~~~~~~
{:.bash-output}
are compatible, because `a & b == 0`. 

The two splits below are also compatible, even though `a & b != 0`, because `a & b == b`:
~~~~~~
  split a: -****-*---
  split b: --**--*---
    a & b: --**--*---
~~~~~~
{:.bash-output}
These two splits, on the other hand, are not compatible because `a & b != 0` and `a & b` is not equal to either `a` or `b`:
~~~~~~
  split a: -***---*--
  split b: ---***---*
    a & b: ---*------
~~~~~~
{:.bash-output}
Here is the body of the `isCompatible` function:
~~~~~~
{{ "steps/step-07/src/split.hpp" | polcodesnippet:"begin_isCompatible-end_isCompatible","" }}
~~~~~~
{:.cpp}

## The conflictsWith member function

The `conflictsWith` member function simply calls `isCompatible(other)` and returns the opposite.
~~~~~~
{{ "steps/step-07/src/split.hpp" | polcodesnippet:"begin_conflictsWith-end_conflictsWith","" }}
~~~~~~
{:.cpp}
