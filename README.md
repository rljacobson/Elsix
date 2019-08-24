# Elsix: An implementation of L^6^, Bell Laboratories' Low-Level Linked List Language

&copy; 2019 Robert Jacobson. Distributed under the MIT license. See LICENSE.txt for more details.

## Introduction

This project implements the programming language L^6^, Bell Laboratories' Low-Level Linked List Language, designed and implemented by Kenneth C. Knowlton in 1965 for the IBM 7094 computer at Bell Labs. This language has fallen into obscurity and, to my knowledge, has no extant implementation.

## Elsix (This Project)

This project implements an L^6^ interpreter in modern C++17. It seems appropriate, however, to honor the memory of the computing pioneers in the construction of this software. I have chosen to do so by following the principles of handmade software as expressed in the [Handmade Manifesto](https://handmade.network/manifesto). 

Information about how to build and install it will eventually be found in the file [doc/BUILDING.md](doc/BUILDING.md). Right now, however, the project is an incomplete work in progress.

The rest of this README discusses the historical significance of L^6^ and its syntax and primary features.


### Original Implementations

The original L^6^ implementation, "7094 L^6^," described in [@cite:Knowlton], was developed by Kenneth C. Knowlton for the IBM 7094 at Bell Labs in Fortan Assembly Programming. The 7094 had a 36-bit word size, 15-bit addressing, and 6-bit character encoding. It was capable of floating point operations, but L^6^ has no floating point literal type.

In [@cite:Knowlton], Knowlton reports that "an extended version of \[L^6^] is being written for the GE-635 and GE-645." The GE systems were a logical choice: "\[A]s a result of the 36-bit needs, \[The GE-635] ended up acting a lot like the 7094. In fact GE offered a box to connect to the 635 called a 9SA that actually allowed the 635 to run 7094 programs."

Elsewhere in the paper, Knowlton references future versions of L^6^:
> One restriction on fields for current 7094 L^6^ is that they may not include bits 0-2 or 18-20 of the 0th word. This restriction will be removed in subsequent versions and the examples in this paper do in fact violate it.

In 1968, another implementation, a "London version" called Lsix, was written by R. J. W. Housden for the London University Atlas computer and described in [@cite:Housden]. The Atlas had a 48-bit word size and 24-bit addressing. Lsix retained the maximum limit of 36 for the number of user definable fields, as this limit is imposed by the total number of single character names that may be formed from only digits (10) and letters (26). Likewise, both implementations allow up to 128 word blocks.

### Kenneth C. Knowlton

Kenneth C. Knowlton is among the pioneers of the computer age. He began his career as many of the pioneers did, at Bell Labs, in the Computer Techniques Research Department, where he made a variety of contributions to graphics generation, representation, display, and transmission. Knowlton is especially well-known for his pioneering contributions to computer generated art. He developed a variety of programming languages at Bell Labs throughout the 60s for creating computer graphics: BEFLIX, EXPLOR, ATOMS, SPHERES. But he also made contributions in other areas, including human-computer interaction, operating systems, and programming languages.

## Language Basics

### Blocks

Blocks are the basic data structure in L^6^ and consist of contiguous words of length 2^n^ up to an original maximum of 2^7^ on the IBM 7094 (maximum not yet determined in this implementation).

The programmer may define up to 36 fields in a block with single character names 0 through 9 and A through Z. The programmer specifies which range of bits within the block each field name references. A field may overlap another field and may be of length 0 up to word length. Fields may be redefined dynamically.

Fields are _untyped_. There are operations specific to different interpretations of the data they operate on. It is up to the programmer to choose the correct operation to match the intended interpretation of the operand.

### Bugs and Pointers

A "bug" is a word-length built-in register. There are 26 bugs named `A` through `Z`. The word bug is meant to evoke the bug sitting on the memory block it points to, crawling across the network to other memory blocks as necessary.

Pointers can be followed via name concatenation. A block with field `D` pointed to by a bug `W` can be referenced as `WD`. Likewise, if `D` points to bug `X` with field `E`, field `E` may be referenced from bug `W` as `WDXE`. 

Pointer arithmetic is allowed.

## Runtime Environment

The runtime environment supplies the following read-only "fields":

| Field | Meaning                                                                                                                                     |
|:-----:|:--------------------------------------------------------------------------------------------------------------------------------------------|
|  T.   | The time in milliseconds since the program began.                                                                                           |
|  n.   | The (potential) number of blocks available from the allocator, where n ranges between 1 and 128 (max not yet determined on modern systems). |

## Literals

Octals, decimals, or "Hollerith" literals, which are strings. No special notation distinguishes octals from decimals, though we will use that standard C/C++ notation in this documentation. Rather, the functions carry the type of the data they operate on. Hollerith literals are not delimited with any quotation markers.

| Type      | Operator Suffix | Mneumonic | Examples |        Range         |
|:----------|:---------------:|:---------:|---------:|:--------------------:|
| Octal     |       `O`       |     o     |     `42` |   0 ... 2^64^ - 1    |
| Decimal   |       `D`       |     d     |     `89` |   0 ... 2^64^ - 1    |
| Hollerith |       `H`       |     h     |  `ELSIX` | 0...1, A...Z, and . |

Strings, the Hollerith literals, include only the digits 0-9, the 26 characters A-Z, and a period. Other characters must be represented by their octal equivalents.

# Character Sets

## IBM 7094 Character Set

Also called binary coded decimal, or BCD.

| <br>Character | <br>10 | Base<br>8 | <br>2  | <br>Character | <br>10 | Base<br>8 | <br>2  | <br>Character | <br>10 | Base<br>8 | <br>2  |
|:--------------|-------:|:---------:|:------:|:--------------|-------:|:---------:|:------:|:--------------|-------:|:---------:|:------:|
| '0'           |      0 |    00     | 000000 | 'E'           |     21 |    25     | 010101 | 'R'           |     41 |    51     | 101001 |
| '1'           |      1 |    01     | 000001 | 'F'           |     22 |    26     | 010110 | 0&#773;       |     42 |    52     | 101010 |
| '2'           |      2 |    02     | 000010 | 'G'           |     23 |    27     | 010111 | ''            |     43 |    53     | 101011 |
| '3'           |      3 |    03     | 000011 | 'H'           |     24 |    30     | 011000 | '*'           |     44 |    54     | 101100 |
| '4'           |      4 |    04     | 000100 | 'I'           |     25 |    31     | 011001 | ' ' (Space)   |     48 |    60     | 110000 |
| '5'           |      5 |    05     | 000101 | <span class="sfrac nowrap;"><span style="display:none; display:inline-block; text-align:center;"><span style="display:block; line-height:0.8em; font-size:70%; padding:0 0.1em;">+</span><span style="display:block; line-height:1em; padding:0 0.1em;">0</span></span></span> |     26 |    32     | 011010 | '/'           |     49 |    61     | 110001 |
| '6'           |      6 |    06     | 000110 | '.'           |     27 |    33     | 011011 | 'S'           |     50 |    62     | 110010 |
| '7'           |      7 |    07     | 000111 | ')'           |     28 |    34     | 011100 | 'T'           |     51 |    63     | 110011 |
| '8'           |      8 |    10     | 001000 | '-'           |     32 |    40     | 100000 | 'U'           |     52 |    64     | 110100 |
| '9'           |      9 |    11     | 001001 | 'J'           |     33 |    41     | 100001 | 'V'           |     53 |    65     | 110101 |
| '='           |     11 |    13     | 001011 | 'K'           |     34 |    42     | 100010 | 'W'           |     54 |    66     | 110110 |
| ' " '         |     12 |    14     | 001100 | 'L'           |     35 |    43     | 100011 | 'X'           |     55 |    67     | 110111 |
| '+'           |     16 |    20     | 010000 | 'M'           |     36 |    44     | 100100 | 'Y'           |     56 |    70     | 111000 |
| 'A'           |     17 |    21     | 010001 | 'N'           |     37 |    45     | 100101 | 'Z'           |     57 |    71     | 111001 |
| 'B'           |     18 |    22     | 010010 | 'O'           |     38 |    46     | 100110 | '±'           |     58 |    72     | 111010 |
| 'C'           |     19 |    23     | 010011 | 'P'           |     39 |    47     | 100111 | ','           |     59 |    73     | 111011 |
| 'D'           |     20 |    24     | 010100 | 'Q'           |     40 |    50     | 101000 | '('           |     60 |    74     | 111100 |

Character 26 (octal 032) and 42 (octal 052) are labeled "Special-1" and "Special-2" respectively. See [@cite:Harper] for more details and a wealth of other collected information about the IBM 7094.


## Atlas Inner Character Set

| <br>Character      | <br>10 | Base<br>8 | <br>2  | <br>Character | <br>10 | Base<br>8 | <br>2  |
|:-------------------|-------:|:---------:|:------:|:--------------|-------:|:---------:|:------:|
| (Not Assigned)     |      0 |    00     | 000000 | '             |     32 |    40     | 100000 |
| Space              |      1 |    01     | 000001 | A             |     33 |    41     | 100001 |
| Tabulate           |      2 |    02     | 000010 | B             |     34 |    42     | 100010 |
| Backspace          |      3 |    03     | 000011 | C             |     35 |    43     | 100011 |
| Shift to outer set |      4 |    04     | 000100 | D             |     36 |    44     | 100100 |
| Shift to inner set |      5 |    05     | 000101 | E             |     37 |    45     | 100101 |
| Shift to LC/LS     |      6 |    06     | 000110 | F             |     38 |    46     | 100110 |
| Shift to UC/FS     |      7 |    07     | 000111 | G             |     39 |    47     | 100111 |
| (                  |      8 |    10     | 001000 | H             |     40 |    50     | 101000 |
| )                  |      9 |    11     | 001001 | I             |     41 |    51     | 101001 |
| ,                  |     10 |    12     | 001010 | J             |     42 |    52     | 101010 |
| π or £             |     11 |    13     | 001011 | K             |     43 |    53     | 101011 |
| ?                  |     12 |    14     | 001100 | L             |     44 |    54     | 101100 |
| &                  |     13 |    15     | 001101 | M             |     45 |    55     | 101101 |
| *                  |     14 |    16     | 001110 | N             |     46 |    56     | 101110 |
| /                  |     15 |    17     | 001111 | O             |     47 |    57     | 101111 |
| 0                  |     16 |    20     | 010000 | P             |     48 |    60     | 110000 |
| 1                  |     17 |    21     | 010001 | Q             |     49 |    61     | 110001 |
| 2                  |     18 |    22     | 010010 | R             |     50 |    62     | 110010 |
| 3                  |     19 |    23     | 010011 | S             |     51 |    63     | 110011 |
| 4                  |     20 |    24     | 010100 | T             |     52 |    64     | 110100 |
| 5                  |     21 |    25     | 010101 | U             |     53 |    65     | 110101 |
| 6                  |     22 |    26     | 010110 | V             |     54 |    66     | 110110 |
| 7                  |     23 |    27     | 010111 | W             |     55 |    67     | 110111 |
| 8                  |     24 |    30     | 011000 | X             |     56 |    70     | 111000 |
| 9                  |     25 |    31     | 011001 | Y             |     57 |    71     | 111001 |
| <                  |     26 |    32     | 011010 | Z             |     58 |    72     | 111010 |
| >                  |     27 |    33     | 011011 | (Unassigned)  |     59 |    73     | 111011 |
| =                  |     28 |    34     | 011100 | (Unassigned)  |     60 |    74     | 111100 |
| +                  |     29 |    35     | 011101 | (Unassigned)  |     61 |    75     | 111101 |
| -                  |     30 |    36     | 011110 | (Unassigned)  |     62 |    76     | 111110 |
| .                  |     31 |    37     | 011111 | Fault         |     63 |    77     | 111111 |

Note that character 11 can be either π or £ depending on the output device. See [@cite:Atlas] for more information.

# ​Differences from L^6^

This section is still evolving. I anticipate using machine sized words, which means a word will be 64 bits rather than 36 or 48. I also anticipate using UTF-8 with the legacy character sets as options for nostalgic compatibility. The two historic implementations both keep the original limits on user definable fields (36) and number of words per block (128), so it seems appropriate to keep them here.

Possible optional language extensions:

* More than 128 words per block
* Multi-character user-definable field names
* More than 36 user definable fields
* More bugs
* C-style number literal representations.

# References

[@cite]: [[#]]

{#cite:Knowlton} Kenneth C. Knowlton. 1966. A programmer's description of L^6^. Commun. ACM 9, 8 (August 1966), 616-625. DOI: https://doi.org/10.1145/365758.365792

{#cite:Housden} R. J. W. Housden, The Definition and Implementation of Lsix in BCL, The Computer Journal, Volume 12, Issue 1, February 1969, Pages 15–23, https://doi.org/10.1093/comjnl/12.1.15

{#cite:Harper} Harper, Jack (2001-08-21). "IBM 7090/94 Character Representation". Online: http://www.frobenius.com/characters.htm.

{#cite:Atlas} International Computers and Tabulators Unlimited, _I.C.T. ATLAS 1 Computer Programming Manual for ATLAS Basic Langauge (ABL)_, I.C.T. House, London, 1965. Original Online: http://www.chilton-computing.org.uk/acl/pdfs/cs348a.pdf. Transcription Online: http://www.chilton-computing.org.uk/acl/literature/manuals/atlas/p018.htm

{#cite:CCH} "GE-635 System Manual," The Centre for Computing History, ref. i.d. CH15671. Online: http://www.computinghistory.org.uk/det/15671/GE-635-System-Manual/

BibTex:
```tex
@article{Knowlton:1966:PDL:365758.365792,
 author = {Knowlton, Kenneth C.},
 title = {A Programmer's Description of L6},
 journal = {Commun. ACM},
 issue_date = {Aug. 1966},
 volume = {9},
 number = {8},
 month = aug,
 year = {1966},
 issn = {0001-0782},
 pages = {616--625},
 numpages = {10},
 url = {http://doi.acm.org/10.1145/365758.365792},
 doi = {10.1145/365758.365792},
 acmid = {365792},
 publisher = {ACM},
 address = {New York, NY, USA},
}

@article{10.1093/comjnl/12.1.15,
    author = {Housden, R. J. W.},
    title = "{The Definition and Implementation of Lsix in BCL}",
    journal = {The Computer Journal},
    volume = {12},
    number = {1},
    pages = {15-23},
    year = {1969},
    month = {02},
    abstract = "{This paper describes the implementation on the London University Atlas computer of the Bell Telephone Laboratories low level linked list language L6. A syntactical definition of L6 is given in terms of BCL, a general purpose programming language with special emphasis on data structures. The description of the implementation in BCL includes details of the general field handling routines.}",
    issn = {0010-4620},
    doi = {10.1093/comjnl/12.1.15},
    url = {https://doi.org/10.1093/comjnl/12.1.15},
    eprint = {http://oup.prod.sis.lan/comjnl/article-pdf/12/1/15/884858/12-1-15.pdf},
}
```
