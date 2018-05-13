/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <stdlib.h>
# include <string>
# include <gsim/gs.h>
# include <gsim/gs_output.h>
# include <gsim/gs_string.h>

//====================== Sym Edge Tutorial =====================

extern void uxas_main(float xstart, float ystart, float xend, float yend, std::string map, int nfz);

// int main ( int argc, char** argv )
int main ( int argc, char** argv )
{
  float xstart, ystart, xend, yend;  
  std::string map;
  int nfz;

  //Path begin/end points
  xstart = atof(argv[1]);
  ystart = atof(argv[2]);
  xend = atof(argv[3]);
  yend = atof(argv[4]);

  //Which map and no-fly zone to use
  map = argv[5];
  nfz = atoi(argv[6]);

  gsout << "Finding path ...\n\n";  
  
  uxas_main(xstart, ystart, xend, yend, map, nfz);  
}

