This could be an example directory structure you can use to keep your
projects, documentation, testing, examples, etc. organized. 

projects
    |
    +---doc
    |
    +---include
    |
    o---tools
        |
        +---[PROJECT NAME]
        |   +---doc
        |   |
        |   +---release
        |   |
        |   +---src
        |   |   |
        |   |   o---include
        |   |   
        |   o---tests
        .
        .
        .

doc
    This subdirectory should contain any general (not project related)
    documentation you want to keep for yourself.

include 
    This subdirectory should contain any header files that are global
    to your projects. By global, I mean that they are used by more
    than one of your projects.

    By placing your header files in include, they will be found
    automatically by the build system. For example, if you have a file
    include/note.lh, then your source files can include it simply with
    ";#include note.lh". It is even possible to group your include
    files into subcatagories. If you wanded to have a seperate
    catagory for Open/object DCL include files, you can place them in
    a subdirectory called ODCL and then your souce file can include
    them by using the directive ";#include ODCL/note.lh"

tools 
    This subdirectory should contain all of your source code for your
    projects. For each project that you build, you will have one
    directory in tools that will contain that program's source code. 

tools/[PROJECT NAME]/doc
    This subdirectory is where any documentation can be placed.
    Documentation can be either for the project itself or and research
    documentation you used while creating the poject (or both).

tools/[PROJECT NAME]/release
    This subdirectory will be where LiFP will place the assembled file.

tools/[PROJECT NAME]/src
    This subdirectory contains the source code for your project.

tools/[PROJECT NAME]/src/include
    This directory contains any header files that are not global to
    all your projects.

tools/[PROJECT NAME]/tests
    This directory contains any testing code you generate (Timetests,
    example ussages, etc.).
