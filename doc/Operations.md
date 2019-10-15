------------------------------------------------
TABLE II. RESUME OF TESTS AND OPERATIONS OF L6
------------------------------------------------

# I. TESTS

Equality
```l6
(c, E, cd)
(c, EO, o)
(c, EH, h)
```

Inequality
```l6
(c, N, cd)
(c, NO, o)
(c, NH, h)
```

Greater Than
```l6
(c, G, cd)
(c, GO, o)
(c, GH, h)
```

Less Than
```l6
(c, L, cd)
(c, LO, o)
(c, LH, h)
```


Pointers to
Same Block
```l6
(c1, P, c2)
```

One-Bits of: (b & ~a) = 0
```l6
(c, O, co)
(c, OD, d
(c, OH, h)
```

Zero-Bits of: (a & ~b) = 0
```l6
(c, Z, co)
(c, ZD, d)
(c, ZH, h)
```

# II. OPERATIONS
### SETUP STORAGE, GET AND FREE BLOCKS

Setup Storage
```l6
; (first word, SS, size, last word)
(s1, SS, d, s2)
```

Define Field
```l6
; (first bit, Df, word, last bit)
(cd1, Df, cd2, cd2)
```

Get Block
```l6
(a, GT, cd)
(a, GT, cd, a2)
```

Free Block
```l6
(a, FR, 0)
(a, FR, c)
```

### COPY BLOCKS AND FIELDS

Copy Field
```l6
(a, E, cd)
(a, EO, o)
(a, EH, h)
```

Duplicate Block
```l6
(a, DP, c)
```

Interchange Field Contents
```l6
(a1, IC, a2)
```

Point to Same Block as
```l6
(a, P, c)
(a, Δ) ; = abbrev. for (a, P, aΔ)
```

### ARITHMETIC OPERATIONS

Add
```l6
(a, A, cd)
(a, AO, o)
(a, AH, h)
```

Subtract
```l6
(a, S, cd)
(a, SO, o)
(a, SH, h)
```

Multiply
```l6
(a, M, cd)
(a, MO, o)
(a, MH, h)
```

Divide
```l6
(a, V, cd)
(a, VO, o)
(a, VH, h)
```

### LOGICAL OPERATIONS

Logical Or
```l6
(a, O, co)
(a, OD, d)
(a, OH, h)
```

Logical And
```l6
(a, N, co)
(a, ND, d)
(a, NH, h)
```

Exclusive Or
```l6
(a, X, co)
(a, XD, d)
(a, XH, h)
```

Complement
```l6
(a, C, co)
(a, CD, d)
(a, CH, h)
```

### SHIFTS AND BIT COUNTS

Shift Left
```l6
(a, L, cd)
(a, L, cd, co)
(a, LD, cd, d)
(a, LH, cd, h)
```

Shift Right
```l6
(a, R, cd)
(a, R, cd, co)
(a, RD, cd, d)
(a, RH, cd, h)
```

Locate Bits
```l6
(a, LO, c)
(a, LZ, c)
(a, RO, c)
(a, RZ, c)
```

Count Bits
```l6
(a, OS, c)
(a, ZS, c)
```

### INPUT/OUTPUT AND CONVERSION

Input
```l6
(a, IN, cd)
```

Print
```l6
(cd, PR, co)
(cd, PRH, h)
```

Print List
```l6
(c, PL, f)
(c, PL, f, cd)
```

Punch
```l6
(cd, PU, co)
(cd, PUH, h)
```

Convert
```l6
(a, BZ, c)
(a, ZB, c)
(a, BD, c)
(a, BO, c)
(a, DB, c)
(a, OB, c)
```

Microfilm
```l6
(cdxmin, XR, cdxmax)
(cdymin, YR, cdymax
(cdx0, DL, cdy0, cdx1, cdy1)
(cdx0, DL, cdy0)
(cdx0, TV, cdy0, co, cd)
(cdx0, TVH, cdy0, h, cd)
(cdx0, TH, cdy0, co, cd)
(cdx0, THH, cdy0, h, cd)
(DO, ADVANC)
```

### PUSHDOWN AND POP-UP OPERATIONS

Save and
Restore Field
Contents
```l6
(S, FC, c)
(R, FC, c)
```

Save and
Restore Field
Definition
```l6
(S, FD, f)
(R, FD, f)
```

Do Subroutine
```l6
(DO, s)
(s2, DO, s)
(DO, STATE)
(DO, DUMP)
(DO, ADVANC)
```
