# Elsix: An implementation of L^6^, Bell Laboratory's Low-Level Linked List Language

&copy; 2019 Robert Jacobson. Distributed under the MIT license. See LICENSE.txt for more details.

## Introduction

This project implements the programming language L^6^, Bell Laboratories' Low-Level Linked List Language, designed and implemented by Kenneth C. Knowlton in 1965 for the IBM 7094 computer at Bell Labs. This language has fallen into obscurity and, to my knowledge, has no extant implementation.


This project implements an L^6^ interpreter in modern C++17. It seems appropriate, however, to honor
the memory of the computing pioneers in the construction of this software in some way. I have chosen
to do so by following the principles of _handmade software_ as expressed in the
[Handmade Manifesto](https://handmade.network/manifesto).

1. Information about how to build and install it will eventually be found in the file [doc/BUILDING.md](doc/BUILDING.md). Right now, however, the project is an incomplete work in progress.
2. A very brief overview of the L^6^ language is given in [doc/Basics.md](doc/Basics.md).
3. A complete list of tests and operations is available in [docs/Operations.md](docs/Operations.md).
3. L^6^ is context sensitive. Some notes documenting important context sensitivities are collected
   in [docs/ContextSensitivity.md](docs/ContextSensitivity.md). This document is particularly
   interesting to anyone wishing to implement L^6^ themselves.
5. A couple of historic character sets likely to have been used by L^6^ are given in
   [doc/CharacterSets.md](doc/CharacterSets.md). The initial release will use ASCII, but a
   "nostalgia mode" using one of the other alternative character sets is planned.

The rest of this README discusses the historical significance of L^6^ and how the language this
project implements differs from the historic L^6^.

## History

### Original Implementations

The original L^6^ implementation, "7094 L^6^," described in [@cite:Knowlton], was developed by Kenneth C. Knowlton for the IBM 7094 at Bell Labs in Fortan Assembly Programming. The 7094 had a 36-bit word size, 15-bit addressing, and 6-bit character encoding. It was capable of floating point operations, but L^6^ has no floating point literal type.

In [@cite:Knowlton], Knowlton reports that "an extended version of \[L^6^] is being written for the GE-635 and GE-645." The GE systems were a logical choice: "\[A]s a result of the 36-bit needs, \[The GE-635] ended up acting a lot like the 7094. In fact GE offered a box to connect to the 635 called a 9SA that actually allowed the 635 to run 7094 programs." [@cite:Wikipedia]

Elsewhere in the paper, Knowlton references future versions of L^6^:
> One restriction on fields for current 7094 L^6^ is that they may not include bits 0-2 or 18-20 of the 0th word. This restriction will be removed in subsequent versions and the examples in this paper do in fact violate it.

In 1968, another implementation, a "London version" called Lsix, was written by R. J. W. Housden for the London University Atlas computer and described in [@cite:Housden]. The Atlas had a 48-bit word size and 24-bit addressing. Lsix retained the maximum limit of 36 for the number of user definable fields, as this limit is imposed by the total number of single character names that may be formed from only digits (10) and letters (26). Likewise, both implementations allow up to 128 word blocks.

### Kenneth C. Knowlton

Kenneth C. Knowlton is among the pioneers of the computer age. He began his career as many of the pioneers did, at Bell Labs, in the Computer Techniques Research Department, where he made a variety of contributions to graphics generation, representation, display, and transmission. Knowlton is especially well-known for his pioneering contributions to computer generated art. He developed a variety of programming languages at Bell Labs throughout the 60s for creating computer graphics: BEFLIX, EXPLOR, ATOMS, SPHERES. But he also made contributions in other areas, including human-computer interaction, operating systems, and programming languages.



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

{#cite:CCH} "GE-635 System Manual," The Centre for Computing History, ref. i.d. CH15671. Online: http://www.computinghistory.org.uk/det/15671/GE-635-System-Manual/

{#cite:Wikipedia} "GE-600 series", Wikipedia, Accessed September 4, 2019. Online: https://en.wikipedia.org/wiki/GE-600_series#History

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
