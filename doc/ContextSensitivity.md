Here we collect a variety of aspects of the L^6 language that affect how L^6 source code is parsed
and interpreted. L^6 is not context-free. We document several context sensitivities below.

# Conventions

## Data types

This follows Knowlton.

| Type Annotation | Lexical Form | Example | Meaning |
|:---:|:---|:---|:---|
| `d` | Decimal number literal | `934` | |
| `o` | Octal number literal | `173` | No prefix |
| `b` | Binary number literal | `101` | No prefix |
| `h` | Holerith (string) literal | `hello` | No quotes |
| `c` | The name of a bug or a remote field. | `AW2` | Dereferences `c` yielding the value in `c`. |
| `a` | The name of a bug or a remote field. | `CD1` | "Affected field," the *location* to write to. Lexically identical to `c`, but `a` is a reference, while `c` is dereferenced (value semantics). |
| `s` | Symbol | `LOOP` | Label of a program location (a *symbol*) or a *symbolic address* of a memory location.* |

\* In (Knowlton 1966), No definition or example is given for "symbolic address" beyond alphabetic
line labels, nor are valid label names characterized. However, (Housden 1968) prescibes the
following:

> "Any combination of alphanumeric characters terminated by a space is accepted as a label. Only the
> first eight characters are significant...."

We also find in Housden the example of the "setup storage" operation `(*20000000,ss,4,*20000400)`.
Here we have a raw memory address given in decimal and dereferences with `*`.


## Terminology

**List:** The syntactic form `(X, Y,..., Z)` is a list. Lists represent either tests or operations.
The words *test* and *operation* refer to semantic meaning rather than syntactic form.

**Length:** We often don't know the number of arguments an operation expression has until after
context analysis. For this reason, in both the code and documentation we use the word *length* to
refer to the number of list items enclosed within parentheses. Thus, `(X, Y)` has length two, `(X,
Y, Z)` has length three, and so forth.

**Operator/Operation:** The operator is the token indicating which operation is being expressed. The
word operation refers to the semantic meaning of the expression. We often refer to the list itself
as an operation in contexts where the identity of the operation the list refers to is understood.

**Argument/Parameter:** On the other hand, we still wish to refer to the parameters of an operation,
that is, the data that the operation acts upon, as distinct from the operator. If `OP` is an operator, then `(X, OP, Y)` is said to have two
arguments, the first being `X` and the second `Y`.

**Arity:** The arity of an operation is how many parameters the operation takes. If `OP` is an
operator, then `(X, OP, Y)` is said to have arity two, `(X, OP, Y, Z)` has arity three, and so
forth. It is not always the case that `arity = length - 1` for at least two reasons:

1. The `P` operation has a short form that omits the `P` operator altogether.
2. Several operations have multiple valid arities. For example, the right shift operation has a form
   of arity two, `(a, R, cd)`, and a form of arity three, `(a, R, cd, co)`.

# Data types

Data types are determined both by context and by syntax.

* Operations with signatures containing

# `DO` and operations of length two

There are a handful of special cases. The operators of arity one are all special cases:

```
(DO, s) 
(DO, STATE) 
(DO, DUMP) 
(DO, ADVANC) 
(c, d) ; A shortcut for (c, P, d) 
```

Note that one must decide how to resolve the ambiguity between these `DO` forms and the short form
of `P`: 

1. If we allow decimal numbers as line labels, we cannot use the short form of `P` for a bug named
   "`DO`", as it would not be clear if `(DO,` *d* `)` means `(c, P, `*d*`)` or "jump to code label
   *d*."
2. We may disallow decimal number labels, in which case the `DO` operation would be distinguished
   from the short form `P` operation by the (syntactically defined) datatype of the second
   parameter. If the parameter is numeric, it's the `P` operation; otherwise it's the `DO`
   operation. But this contradicts (Housden 1968).
   
The `DO` operation also has a two-argument form: `(s2, DO, s)`.

# Other special cases

## Overloaded operators

The following operators are special cases because they are overloaded, distinguished only by the
arguments they are given:

* `FR` as `(a, FR, 0)` or `(a, FR, c)`
* `FC` as `(S, FC, c)` or `(R, FC, c)`
* `FD` as `(S, FD, c)` or `(R, FD, c)`

The DEFINE FIELD operator, "Df" for f any field name, is only distinguished from other operators
that start with the letter 'D' by the fact that it takes three arguments while the others only take
two or four.

### Variadic operators

The following operations may take either three or four arguments:

* `GT`
* `L`
* `R`
* `PL`
* `DL`

The `DO` operation can take one or two arguments. The `P` operation always takes two arguments but
can be written as a list of length two by omitting the operator `P`.
