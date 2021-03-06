! Copyright (C) 2004, 2009 Slava Pestov.
! See http://factorcode.org/license.txt for BSD license.
USING: arrays assocs continuations debugger generic hashtables
init io io.files kernel kernel.private make math memory
namespaces parser parser.notes prettyprint sequences splitting
system vectors vocabs vocabs.loader words destructors ;
QUALIFIED: bootstrap.image.private
IN: bootstrap.stage1

"Bootstrap stage 1..." print flush

"vocab:bootstrap/primitives.factor" run-file

load-help? off
{ "resource:core" } vocab-roots set

! Create a boot quotation for the target by collecting all top-level
! forms into a quotation, surrounded by some boilerplate.
[
    [
        ! Rehash hashtables first, since bootstrap.image creates
        ! them using the host image's hashing algorithms.
        [ hashtable? ] instances [ rehash ] each
        boot
    ] %

    "math.integers" require
    "math.floats" require
    "memory" require

    "io.streams.c" require
    "vocabs.loader" require

    "syntax" require
    "bootstrap.layouts" require

    [
        f parser-quiet? set-global

        init-resource-path

        "resource:basis/bootstrap/stage2.factor"
        dup exists? [
            run-file
        ] [
            "Cannot find " write write "." print
            "Please move " write image write " into the same directory as the Factor sources," print
            "and try again." print
            1 (exit)
        ] if
    ] %
] [ ] make
bootstrap.image.private:bootstrap-startup-quot set
