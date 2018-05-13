/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef GS_SHARED_H
# define GS_SHARED_H

/*! \file gs_shared.h 
    Reference counter for smart pointer behavior.
    Note: attention is required to avoid circular references */
class GsShared
 { private :
    unsigned int _ref;

   protected :
    /*! Constructor initializes the reference counter as 0 */
    GsShared () { _ref=0; };

    /*! Destructor in derived classes should be declared as
        protected in order to oblige users to call always unref(),
        instead of delete */
    virtual ~GsShared() {};

   public :
    /*! Returns the current reference counter value. */
    unsigned getref () const { return _ref; }

    /*! Increments the reference counter. */
    void ref () { _ref++; }

    /*! Decrements the reference counter (if >0), and if the
        counter becomes 0, the object is automatically self deleted. */
    void unref() { if(_ref>0) _ref--; if(_ref==0) delete this; }
 };

//============================== end of file ===============================

# endif  // GS_SHARED_H
