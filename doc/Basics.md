# Language Basics

## Blocks

Blocks are the basic data structure in L^6^ and consist of contiguous words of length 2^n^ up to an original maximum of 2^7^ on the IBM 7094 (maximum not yet determined in this implementation).

The programmer may define up to 36 fields in a block with single character names 0 through 9 and A through Z. The programmer specifies which range of bits within the block each field name references. A field may overlap another field and may be of length 0 up to word length. Fields may be redefined dynamically.

Fields are _untyped_. There are operations specific to different interpretations of the data they operate on. It is up to the programmer to choose the correct operation to match the intended interpretation of the operand.

## Bugs and Pointers

A "bug" is a word-length built-in register. There are 26 bugs named `A` through `Z`. The word bug is meant to evoke the bug sitting on the memory block it points to, crawling across the network to other memory blocks as necessary.

Pointers can be followed via name concatenation. A block with field `D` pointed to by a bug `W` can be referenced as `WD`. Likewise, if `D` points to bug `X` with field `E`, field `E` may be referenced from bug `W` as `WDXE`.

Pointer arithmetic is allowed.

# Runtime Environment

The runtime environment supplies the following read-only "fields":

| Field | Meaning                                                                                                                                     |
|:-----:|:--------------------------------------------------------------------------------------------------------------------------------------------|
|  T.   | The time in milliseconds since the program began.                                                                                           |
|  n.   | The (potential) number of blocks available from the allocator, where n ranges between 1 and 128 (max not yet determined on modern systems). |

# Literals

Octals, decimals, or "Hollerith" literals, which are strings. No special notation distinguishes octals from decimals, though we will use that standard C/C++ notation in this documentation. Rather, the functions carry the type of the data they operate on. Hollerith literals are not delimited with any quotation markers.

| Type      | Operator Suffix | Mneumonic | Examples |        Range         |
|:----------|:---------------:|:---------:|---------:|:--------------------:|
| Octal     |       `O`       |     o     |     `42` |   0 ... 2^64^ - 1    |
| Decimal   |       `D`       |     d     |     `89` |   0 ... 2^64^ - 1    |
| Hollerith |       `H`       |     h     |  `ELSIX` | 0...1, A...Z, and . |

Strings, the Hollerith literals, include only the digits 0-9, the 26 characters A-Z, and a period. Other characters must be represented by their octal equivalents.

