/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef GS_DEQUE_H
# define GS_DEQUE_H

/** \file gs_deque.h 
 * double-ended queue */

# include <gsim/gs_array.h> 

/*! \class GsDeque gs_deque.h
    \brief double-ended queue
    A double-ended queue template based on GsArray. */
template <typename X>
class GsDeque
 { private :
    GsArray<X> _array;
    int _base;
    bool _topmode;

   public : // methods to manipulate both ends of the deque

    GsDeque () { _base=0; _topmode=true; }
    GsDeque ( int cap ) { init(cap); _topmode=true; }
    GsDeque ( const GsDeque<X>& d ) : _array(d._array) { _base=d._base; _topmode=d._topmode; }

    int size () { return _array.size()-_base; }
    void init ( int cap ) { _array.capacity(cap); _array.size(cap/2); _base=_array.size(); }
    void init () { _array.size(_array.capacity()/2); _base=_array.size(); }
    void compress () { _array.remove(0,_base); _base=0; _array.compress(); }

    X& top ( int i ) { return _array[_array.size()-i-1]; }
    X& top () { return _array.top(); }
    X& popt () { return _array.pop(); }
    X& pusht () { return _array.push(); }

    X& bottom ( int i ) { return _array[_base+i]; }
    X& bottom () { return _array[_base]; }
    X& popb () { return _array[_base++]; }
    X& pushb ()
       { if (_base==0) { _base=_array.size(); _array.insert(0,_base); }
         return _array[--_base];
       }

    void remove ( int i ) { _array.remove(_base+i); }
    X& operator[] ( int i ) { return _array[_base+i]; }

   public : // methods based on topmode state

    bool topmode () const { return _topmode; }
    void topmode ( bool b ) { _topmode=b; }
    X& get ( int i ) { return _topmode? top(i):bottom(i); }
    X& get () { return _topmode? top():bottom(); }
    X& pop () { return _topmode? popt():popb(); }
    X& push () { return _topmode? pusht():pushb(); }
 };

//============================== end of file ===============================

#endif // GS_DEQUE_H
