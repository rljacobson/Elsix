# Introduction 2 L6

## Statements

The typical statement has the form

```
label   IF test1 test2 ... testn THEN operation1 operation2 ... operationm symbol
──┬──   ────────────┬─────────── ──┬─ ───────────────────┬──────────────── ───┬──
Optional line label │              │                     │                    │
                    │              │                     │  Optional implicit GoTo
        The IF clause is optional. │                     │
        If ommitted, the operation │            Either one or more operations,
        and/or GoTo will occur     │            or a symbol to GoTo, or both,
        unconditionally.           │            must be provided.
                                   │
                            THEN must be present if an
                            IF clause is present. Other-
                            wise, THEN is optional and 
                            merely introduces the oper-
                            ation(s). 
```
