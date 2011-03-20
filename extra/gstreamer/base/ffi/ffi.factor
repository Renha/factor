! Copyright (C) 2010 Anton Gorenko.
! See http://factorcode.org/license.txt for BSD license.
USING: alien alien.libraries alien.syntax combinators
gobject-introspection kernel system vocabs.loader ;
IN: gstreamer.base.ffi

<<
"gstreamer.ffi" require
>>

LIBRARY: gstreamer.base

<<
"gstreamer.base" {
    { [ os winnt? ] [ drop ] }
    { [ os macosx? ] [ drop ] }
    { [ os unix? ] [ "libgstbase-0.10.so" cdecl add-library ] }
} cond
>>

GIR: vocab:gstreamer/base/GstBase-0.10.gir
