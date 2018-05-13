/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef VMESH_H
# define VMESH_H

//******************************************************************
// Here is an example of the types required for defining a mesh
// containing only user data in the vertices (2d coordinates)
//******************************************************************

//******************************************************************
//  First, we define the types we will use
//******************************************************************

class VmVertex; // will be defined later containing my own data
class VmEdge;   // this will be a default element
class VmFace;   // this will be a default element

// Define VmSymEdge to have nice type casts defined:
typedef Se < VmVertex, VmEdge, VmFace > VmSymEdge;

// Define VmMesh also to have nice type casts defined:
typedef SeMesh < VmVertex, VmEdge, VmFace > VmMesh;

//******************************************************************
//  Second, we define the edge and face as default elements
//******************************************************************

SE_DEFINE_DEFAULT_ELEMENT(VmEdge,VmSymEdge);
SE_DEFINE_DEFAULT_ELEMENT(VmFace,VmSymEdge);

//******************************************************************
//  Finally, we define the vertex class we want to use
//******************************************************************

class VmVertex : public SeElement
 { public :
    float c[2]; // 2d coordinates

   public :
    SE_ELEMENT_CASTED_METHODS(VmVertex,VmSymEdge);

    VmVertex () { c[0]=c[1]=0; }

    VmVertex ( const VmVertex& v ) : SeElement() { c[0]=v.c[0]; c[1]=v.c[1]; }

    void set ( float x, float y ) { c[0]=x; c[1]=y; }

    friend GsOutput& operator<< ( GsOutput& out, const VmVertex& v )
           { return out << v.c[0] << gspc << v.c[1]; }

    friend GsInput& operator>> ( GsInput& inp, VmVertex& v )
           { return inp >> v.c[0] >> v.c[1]; }

    static inline int compare ( const VmVertex* v1, const VmVertex* v2 ) { return 0; } // not used
 };

# endif // VMESH_H
