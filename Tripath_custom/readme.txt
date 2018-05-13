
===============================================================
  tripath: triangulation and path planning module of Graphsim
                (c) Marcelo Kallmann 2010
===============================================================

==== TERMS OF DISTRIBUTION ====

  Copyright 2010 Marcelo Kallmann. All Rights Reserved.
  This software is distributed for noncommercial use only, without
  any warranties, and provided that all copies contain the full copyright
  notice licence.txt located at the base folder of the distribution.

==== NOTES ====

  Measures have been implemented to achieve a robust code, however
  robustness cannot be guaranteed for all types of input because
  this toolkit relies only on floating point computations.
  
  This toolkit reflects the 2010 implementation for the SCA 2010 paper
  "Shortest Paths with Arbitrary Clearance from Navigation Meshes".
  Read also "Navigation Queries from Triangulated Meshes" (MIG 2010).

  The updated LCT definition, correct handling of dynamic operations,
  and robustness algorithms that are described in the TOG 2014 paper
  "Dynamic and Robust Local Clearance Triangulations" are not integrated
  in this toolkit.

==== INSTALLATION GUIDELINES ====

 - Windows: 
   .visual C++ projects are provided in the visualc9 and visualc10 folders

 - Linux: 
   .call make from the linux folder
   .the existing makefiles have not been tested for a while,
     so adjustments will be probably needed

==== DIRECTORIES ====

 - bin:      after compilation, will contain test executable
 - glut:     contains glut lib and header file
 - gsim:     header files 
 - lib:      after compilation, will contain the created library
 - linux:    contains makefiles for compilation under linux
 - src:      C++ sources for the library and test application
 - visualc:  projects for Microsoft Development Environment

