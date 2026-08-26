# Yuzu
> "Hell no, I'm a white man. I wrote my own f***ing compiler" -- Terry A. Davis (RIP)

## About
Yuzu is a statically typed programming language written in C++, designed to be syntactically
minimal. Though it is a compiled language, the basic evaluator is basically an interpreter.
The compiler (will) support JIT compilation, as well as AOT compilation. It is LLVM based.
It attempts to reconcile the low level nature of languages like C and C++ with a modern syntax, 
free of the historical chaos of said language. It has a tunable garbage collector (not sure how that
will work, will decide later when designing the GC). 

## Features
As of version 0.1.5, here are the features that has been implemented:
* [x] File reader
* [x] Lexer
* [X] Abstract syntax tree
* [X] Parser
* [ ] Semantic analysis
* [ ] Better type system
* [ ] LLVM Codegen
* [ ] Better README
