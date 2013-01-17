This is a "preprocessor" project. This tool started off intending to
extend the AutoLisp application development process (A language for
the AutoCAD program) in both a group and solo development environment
but I wanted to convert this simple application to use the LLVM
libraries as a learning exercise.

The command line flags are:
  -d=<string,string> - Specify a definition
  -i=<path>          - Specify include file location
  -license           - Print the license
  -o=<filename>      - Specify output filename
  -print             - Print informational messages


Command line flag notes:
The '-d' flag can be used two different ways.
    1. with a just a definition
    2. with a definition and a replacement.
This is meant as a reproduction of the C preprocessor directive #DEFINE 

The '-i' flag is meant to be used with a string like: ./include/ to
indicate where custom included can be located. This flag can also be used
several time to indicated other areas for this preprocessor to look. Use
like: -i ./this/location/ -i ../that/location/ -i ../../or/even/there/

The '-o' flag is meant to only be used once to specify an output file.
If the -o flag is ommited the output is stdout (console/screen).

The '-p' flag is meant to offer the user a simple report of what was and
what was not included in the final product. Use: -p



Begin original documentation (modified slightly for new command line
arguments):

Not every project is developed using the same methods so this tool was
designed to be as "helpful" as possible without being intrusive
(flexible).

=============================================================================
The basics.
=============================================================================

,----[ MainFile ]-
|
| ;#include print-line.lh
|
| (defun myfoo ( / )
|   (print-line "Hello") ;; function in the "print-line" header file
|   (princ)
|  )
`----

,----[ print-line.lh header ]-
|
| (defun print-line ( str )
|   (princ (strcat "\n" str))
|  )
`----

This preprocessor will parse the file given for the "include" and then look for the
header file and paste it into the main file. So after preprocessing you would have a
fully assembled lisp file to load into AutoCAD or copy to a network folder or post to
a forum.

,----[ assembled file ]-
|
| (defun print-line ( str )
|   (princ (strcat "\n" str))
|  )
|
| (defun myfoo ( / )
|   (print-line "Hello") ;; function in the "print-line" header file
|   (princ)
|  )
`----

File names could be something like this:

.lh     =       lisp header file
.ls     =       lisp source file
.lsp    =       final product

Syntax for calling preprocessor:
        
        myprocessor myfoo.ls -o myfoo.lsp
                or
        myprocessor myfoo.ls -i <CUSTOM/HEADER/LOCATION/> -o myfoo.lsp

The idea is to work in the `lisp source file' and generate a final product from it
using the `lisp header files'. 

Lisp header files can be anything from project specific functions to company or group
standard library functions.

In in effort not to inhibit current development habits I have hid the "include
statement" behind the semi colon (;) (a comment in the lisp language) so that a lisp
source file can be used/edited in the VLIDE as well as your favorite text editor.

The idea of keeping the lisp source file (.ls) and the final product (.lsp) stems
from the idea of ever changing standards, methods, tools, technologies. I wanted a
way to edit and update (maintain) several applications without having to open and
edit them. This tool allows me to keep a single directory of tools that I expect to
have to update (a standard library of functions) at new AutoCAD releases, company
standards, etc.. Using this tool after an update (be it company standards, or AutoCAD
release) I can "recreate" the final lisp files with the latest tools.

Another reason for the separation is the task of working in a group where I may not
be developing some or all of the support functions. Using this tool I can make sure I
fetch the latest support functions from the network before I "process" my final
product. 

A note should be made about the thoughts that went behind the building and intended
use of this tool in the case of libraries; because code libraries can become a
tangled web of "includes" something more like the following, I have built in some
intelligence into the "include" process.  I will attempt to demonstrate with the
following: 

,----[ MainFile ]-
|
| ;#include licence.lh
| ;#include print-line.lh
|
| (defun myfoo ( / )
|   (print-line "Hello") ;; function in the "print-line" header file
|   (princ)
|  )
`----

,----[ licence.lh header ]-
|
| ;;
| ;; (c) me <YEAR>
| ;;
|
`----

,----[ print-line.lh header ]-
|
| ;#include licence.lh
| ;#include strjoin.lh
|
| (defun print-line ( str )
|   (princ (strjoin "\n" str))
|  )
`----

,----[ strjoin.lh header ]-
|
| ;#include licence.lh
|
| (defun strjoin ( str1 str2 )
|   (strcat str1 str2))
|  )
`----

As you can see the "licence.lh" file is included several times throughout
several of the files. This preprocessor will only include a file once so the 
final output of the previous situation will become:

,----[ assembled file ]-
|
| ;;
| ;; (c) me <YEAR>
| ;;
|
| (defun print-line ( str )
|   (princ (strjoin "\n" str))
|  )
|
| (defun strjoin ( str1 str2 )
|   (strcat str1 str2))
|  )
|
| (defun myfoo ( / )
|   (print-line "Hello") ;; function in the "print-line" header file
|   (princ)
|  )
`----
