/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# include <gsim/se_elements.h>

//================================= SeElement ====================================

SeElement* SeElement::_remove ()
 { 
   _next->_prior = _prior; 
   _prior->_next = _next; 
   return this; 
 }

// same insert_prior() implementation as in GsListNode
SeElement* SeElement::_insert ( SeElement* n )
 { 
   n->_prior = _prior;
   _prior->_next = n;
   n->_next = this;
   _prior = n;
   return n;
 } 

//=== End of File ===================================================================
