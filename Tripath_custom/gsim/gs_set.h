/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/
# ifndef GS_SET_H
# define GS_SET_H

/** \file gs_set.h 
 * indexed set of pointers */

# include <gsim/gs_array.h> 
# include <gsim/gs_string.h> 
# include <gsim/gs_manager.h> 

/*! GsSetBase saves in an array, pointers for user data, which can be
    later retrieved with indices. Indices are unique and do not changed.
    When an extraction (removal) is done gaps are created and the
    free positions are marked with null pointers and stored for later
    reuse. GsSetBase is a low level management class and there is no
    destructor to delete stored data. The user is completely
    responsible of the data allocation and deletion.
    Here is an example of how to delete all data in the set:
      for ( int id=0; id<=set.maxid(); id++ )
        delete ((UserClassTypeCast*)set.get(id));
      set.init_arrays(); */
class GsSetBase
 { protected :
    GsArray<void*> _data;   // data list
    GsArray<int> _freepos;  // keeps freed positions in the array

   public :
    /*! Will set the sizes of the internal arrays to 0. This is
        to be called only in case it is guaranteed that all referenced
        objects are already deleted. Use with care! */
    void init_arrays () { _data.size(0); _freepos.size(0); }

    /*! Inserts pt in one empty position and return its index.
        Note: if pt is 0 (ie null), a null pointer is added at the
        end of the set and its position is stored as a free position. */
    int insert ( void* pt );

    /*! Removes and returns the element stored at position i. */
    void* extract ( int i );

    /*! Compress internal arrays, removing possible nonused positions. */
    void compress ();

    /*! Removes all internal gaps changing current existing indices.
        Old indices can be updated with their new values, which are
        stored in the given Array newindices in the following way:
        newindices[oldindex] == newindex */
    bool remove_gaps ( GsArray<int>& newindices );

    /*! Returns the number of elements in the Set */
    int elements () const { return _data.size()-_freepos.size(); }

    /*! Returns true if the number of elements is 0, or false otherwise */
    bool empty() const { return elements()==0? true:false; }

    /*! maxid() returns the maximum existing id or -1 if the set is empty */
    int maxid () const { return _data.size()-1; }

    /*! Returns the element pointer stored at position i.
        It will return null for previously deleted positions; and
        it is the user responsability to ensure that i is not out of range. */
    void* get ( int i ) const { return _data[i]; }

    /*! Const version of get() */
    const void* cget ( int i ) const { return _data[i]; }
 };
 
/*! GsSet saves in an array, pointers for user data, which can be later
    retrieved with indices. Indices are unique and are not changed
    during insertions and removals. Indices are always >=0.
    As no rearrangements are performed when removals are done,
    internal free positions (gaps) are created. */
template <class X>
class GsSet : public GsSetBase
 { public:
    /*! Default Constructor */
    GsSet () : GsSetBase() {}

    /*! Copy Constructor */
    GsSet ( const GsSet& s ) : GsSetBase() { copy(s); }

    /*! Destructor deletes all data */
   ~GsSet () { while(_data.size()) delete (X*) _data.pop(); }

    /*! Deletes all data internally referenced, and make an empty Set */
    void init ()
     { while ( _data.size() ) delete (X*) _data.pop();
       _freepos.size(0); 
     }

    /*! Init Set and make it become a copy of s, with the same indices
        and possible internal gaps. Uses copy constructor of X. */
    void copy ( const GsSet<X>& s )
     { int i;
       _freepos = s._freepos;
       while ( _data.size() ) delete (X*) _data.pop();
       for ( i=0; i<s._data.size(); i++ )
         _data.push() = s._data[i]? new X( *((X*)s._data[i]) ) : 0;
     }

    /*! Allocates one element, inserts it in one empty position and
        returns its index */
    int insert () { return GsSetBase::insert(new X); }

    /*! Inserts x in one empty position and returns its index */
    int insert ( X* x ) { return GsSetBase::insert ( (void*)x ); }

    /*! Deletes and removes the element stored at position i */
    void remove ( int i ) { delete (X*) GsSetBase::extract(i); }

    /*! Extract (remove) the pointer from the set without deleting it */
    X* extract ( int i ) { return (X*)GsSetBase::extract(i); }

    /*! Access to the pointer at position i */
    X* get ( int i ) const { return (X*)GsSetBase::get(i); }

    /*! Const access to the pointer at position i */
    const X* cget ( int i ) const { return (const X*)GsSetBase::cget(i); }

    /*! Access operator */
    X* operator[] ( int i ) const { return (X*)GsSetBase::get(i); }

    /*! Const access operator */
    X* operator() ( int i ) const { return (X*)GsSetBase::cget(i); }

    /*! Copy operator */
    void operator = ( const GsSet& s ) { copy ( s ); }

    /*! Output */
    friend GsOutput& operator<< ( GsOutput& out, const GsSet<X>& s )
     { out<<"[ ";
       for ( int i=0; i<s._data.size(); i++ )
        if ( s(i) ) { out << i << ':' << *s(i) << gspc; }
       return out<<']';
     }

    /*! Input */
    friend GsInput& operator>> ( GsInput& inp, GsSet<X>& s )
     { s.init ();
       inp.get(); // '['
       while ( inp.get()!=GsInput::Delimiter )
        { int id = inp.ltoken().atoi();
          while ( s._data.size()<id ) s.insert(0);
          inp.get(); // ':'
          s._data.push() = new X;
          inp >> *((X*)s._data.top());
        }
       return inp;
     }
 };

//============================== end of file ===============================

#endif // GS_SET_H
