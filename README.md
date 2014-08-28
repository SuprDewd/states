
# states

`states` is a utility program for working with deterministic finite-state
automata (DFA), non-deterministic finite-state automata (NFA) and regular
expressions.

Planned features include:

- converting between NFA, DFA and regular expressions
- checking if a give string is accepted by the given NFA/DFA/regex (so it can be used like grep)
- incrementally finding all strings accepted by the given NFA/DFA/regex
- performing various operations on NFA/DFA/regex such as union, intersection, concatenation and Kleene closure
- supporting a nice syntax for representing NFA/DFA/regex
- exporting NFA/DFA/regex to TikZ and other formats
- minimize DFA
- diff the languages of two NFA/DFA/regex
- count how many strings are accepted of a give length

Some of these features have already been implemented.

## Installing

```
$ ./autogen.sh
$ ./configure
$ make
# make install
```

## Examples

The following example shows how `states` can read an NFA and then incrementally
output all strings that are accepted by it.

```
$ cd examples
$ cat nfa.st
$ cat nfa.st | states language | head -n 30
```

