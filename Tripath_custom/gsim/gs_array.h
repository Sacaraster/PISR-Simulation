/*=======================================================================
   Copyright 2010 Marcelo Kallmann. All Rights Reserved.
   This software is distributed for noncommercial use only, without
   any warranties, and provided that all copies contain the full copyright
   notice licence.txt located at the base folder of the distribution. 
  =======================================================================*/

# ifndef GS_ARRAY_H
# define GS_ARRAY_H

/** \file gs_array.h 
 * fast resizeable array template */

# include <gsim/gs.h> 
# include <gsim/gs_input.h> 
# include <gsim/gs_output.h> 

/*! \class GsArrayBase gs_array.h
    \brief Fast resizeable array base class

    This class is to be derived, and not to be directly used. See GsArray for
    a user-ready class. All memory management functions of GsArrayBase were
    written using quick memory block trasfer functions, and malloc() and free()
    functions are used. So that void pointers are used to refer to user's data.
    Most methods need to know the size of each element which is the parameter 
    sizeofx appearing several times. */
class GsArrayBase
 { protected :
    void* _data;     //!< Array pointer used for storage
    int   _size;     //!< Number of elements being used in array
    int   _capacity; //!< Number of allocated elements (>=size)

   protected :

    /*! Init with the sizeof of each element, size, and capacity. If the given
        capacity is smaller than the size, capacity is set to be equal to size */
    GsArrayBase ( unsigned sizeofx, int s, int c );

    /*! Copy constructor. Allocates and copies size elements. */
    GsArrayBase ( unsigned sizeofx, const GsArrayBase& a );

    /*! Constructor from a given buffer. No checkings are done, 
        its the user responsability to give consistent parameters. */
    GsArrayBase ( void* pt, int s, int c );

    /*! Will free the internal buffer if needed. The internal size and capacity 
        are not adjusted, so that this method should be called only by the 
        destructor of the derived class, as GsArrayBase has not a destructor. */
    void free_data ();

    /*! Changes the size of the array. Reallocation is done only when the size 
        requested is greater than the current capacity, and in this case,
        capacity becomes equal to the size. */
    void size ( unsigned sizeofx, int ns );

    /*! Changes the capacity of the array. Reallocation is done whenever a new
        capacity is requested. internal memory is freed in case of 0 capacity.
        The size is always kept inside [0,nc]. Parameter nc is considered 0
        if it is negative. */
    void capacity ( unsigned sizeofx, int nc );

    /*! Returns a valid index as if the given index references a circular
        array, ie, it returns index%size() for positive numbers. Negative
        numbers are also correctly mapped. */
    int vid ( int index ) const;

    /*! Makes size==capacity, freeing all extra capacity if any. */
    void compress ( unsigned sizeofx );

    /*! Removes n positions starting with position index i, moving all data
        correctly. Parameters i and n must be correct as they are not checked.
        In any case, no reallocation is done; only the size is decreased. */
    void remove ( unsigned sizeofx, int i, int n );

    /*! Inserts n positions, starting at pos i, moving all data correctly. 
        Parameter i can be in [0,size()], if i==size(), n positions are 
        appended. Parameters i and n must be correct as they are not checked.
        If reallocation is needed, the new capacity will be the double of
        the new size (size after insertion). */
    void insert ( unsigned sizeofx, int i, int n );

    /*! Inserts 1 position at the end of the array. If reallocation is
        needed, the array capacity is reallocated to contain two times
        the new size (current size+1). */
    void push ( unsigned sizeofx );

    /*! Copies n entries from src position to dest position. Regions are
        allowed to overlap. Uses the C function memmove.
        No parameter checkings are done. */
    void copy ( unsigned sizeofx, int dest, int src, int n );

    /*! Copies n entries from src position to dest position at destpt buffer. */
    void copyto ( unsigned sizeofx, GsArrayBase& desta, int dest, int src, int n ) const;

    /*! Sorts the array, with the compare function cmp, by calling the
        system function qsort(). */
    void sort ( unsigned sizeofx, gscompare cmp );

    /*! Linear search, returns index of the element found, or -1 if not found */
    int lsearch ( unsigned sizeofx, const void *x, gscompare cmp ) const;

    /*! Binary search for sorted arrays. Returns index of the element found,
        or -1 if not found. If not found and pos is not null, pos will have
        the position to insert the element keeping the array sorted. Faster 
        than the standard C library function bsearch() for large arrays. */
    int bsearch ( unsigned sizeofx, const void *x, gscompare cmp, int *pos=0 ) const;

    /*! Returns the position of the insertion. The space of sizeofx is
        created (with insert) and the index of the position is returned,
        but the contents of x are not moved to the inserted position.
        That way, x can be only a key to the data stored in the array. */
    int insort ( unsigned sizeofx, const void *x, gscompare cmp );

    /*! Same as insort(), but will not insert x in case there is already an
        element in the array equal to x. Returns the position of the insertion,
        or -1 if x was not inserted. */
    int uniqinsort ( unsigned sizeofx, const void *x, gscompare cmp );

    /*! Copy from another array. GsArrayBase will be a copy of the given array,
        but will reallocate only if needed in order to hold a.size elements. */
    void copyfrom ( unsigned sizeofx, const GsArrayBase& a );

    /*! Returns the internal buffer pointer and makes GsArrayBase an empty 
        and valid array. The returned pointer will be null or will contain
        allocated data with malloc(). The user will then be responsible to
        free this memory with free() when needed. */
    void* abandon ();

    /*! Adopts the data of the given array a, and lets a become an empty array.
        GsArrayBase will have the same data that a had before. This is done 
        without reallocation. */
    void adopt ( GsArrayBase& a );

    /*! Frees the current data of GsArrayBase, and then makes GsArrayBase to control
        the given buffer pt, with size and capacity as given.  */
    void adopt (  void* pt, int s, int c );
 };

/*! \class GsArray gs_array.h
    \brief Fast resizeable dynamic array

    All memory management functions of GsArray use quick memory block functions 
    and so be aware that constructors and destructors of class X are not called. 
    GsArray can be used only with classes or structs that do not have any internal
    allocated data, as GsArray will not respect them when resizing. internally, 
    malloc(), realloc() and free() functions are used throught GsArrayBase methods. 
    Note that the array size is automatically reallocated when needed (with a double
    size strategy), and so take care to not reference internal memory of GsArray
    that can be reallocated. For example, the following code is wrong: a.push()=a[x],
    because a[x] references a memory position that can be reallocated by push() */
template <typename X>
class GsArray : protected GsArrayBase
 { public:

    /*! Constructs with the given size and capacity. If the given capacity 
        is smaller than the size, capacity is set to be equal to size. */
    GsArray ( int s=0, int c=0 ) : GsArrayBase ( sizeof(X), s, c ) {}

    /*! Copy constructor. GsArray will be an exact copy of the given array, 
        but allocating as capacity only the size of a. 
        Attention: the operator= that X might have is not called ! */
    GsArray ( const GsArray& a ) : GsArrayBase ( sizeof(X), a ) {}

    /*! Constructor from a given buffer. No checkings are done, its the user 
        responsability to give consistent parameters. */
    GsArray ( X* pt, int s, int c ) : GsArrayBase ( (void*)pt, s, c ) {}

    /*! Destructor frees the array calling the base class free_data() method.
        Attention: elements' destructors are not called ! */
   ~GsArray () { GsArrayBase::free_data(); }

    /*! Returns true if the array has no elements, ie, size()==0; and false otherwise. */
    bool empty () const { return _size==0? true:false; }

    /*! Returns the capacity of the array. Capacity is used to be able to have a 
        larger storage buffer than the current size used. The method capacity() 
        will always return a value not smaller than size(). */
    int capacity () const { return _capacity; }

    /*! Returns the current size of the array. */
    int size () const { return _size; }

    /*! Changes the size of the array. Reallocation is done only when the size 
        requested is greater than the current capacity, and in this case, capacity
        becomes equal to the size. */
    void size ( int ns ) { GsArrayBase::size(sizeof(X),ns); }

    /*! Changes the capacity of the array. Reallocation is done whenever a new
        capacity is requested. Internal memory is freed in case of 0 capacity.
        The size is always kept inside [0,nc]. */
    void capacity ( int nc ) { GsArrayBase::capacity(sizeof(X),nc); }

    /*! Defines a minimum capacity to use, ie, sets the capacity to be c
        only if the current capacity is lower than c */
    void reserve ( int c ) { if ( capacity()<c ) capacity(c); }

    /*! Sets all elements as x, copying each element using operator = */
    void setall ( const X& x )
        { int i; for ( i=0; i<_size; i++ ) ((X*)_data)[i]=x; }

    /*! Makes capacity to be equal to size, freeing all extra capacity if any. */
    void compress () { GsArrayBase::compress ( sizeof(X) ); }

    /*! Returns a valid index as if the given index references a circular
        array, ie, it returns index%size() for positive numbers. Negative
        numbers are also correctly mapped. */
    int vid ( int index ) const { return GsArrayBase::vid(index); }

    /*! Gets a const reference to the element of index i. Indices start from 0 and must 
        be smaller than size(). No checkings are done to ensure that i is valid. */
    const X& cget ( int i ) const { return ((X*)_data)[i]; }

    /*! Gets a reference to the element of index i. Indices start from 0 and must 
        be smaller than size(). No checkings are done to ensure that i is valid. */
    X& get ( int i ) const { return ((X*)_data)[i]; }

    /*! Sets an element. Operator = is used here. Indices start from 0 and must 
        be smaller than size(). No checkings are done to ensure that i is valid. */
    void set ( int i, const X& x ) { ((X*)_data)[i]=x; }

    /*! Operator version of get(), which returns a non const reference.
        No checkings are done to ensure that i is valid. */
    X& operator[] ( int i ) const { return ((X*)_data)[i]; }

    /*! Access operator with a long integer */
    X& operator[] ( long i ) const {return ((X*)_data)[i]; }

    /*! Operator version of cget(), which returns a const reference.
        No checkings are done to ensure that i is valid. */
    const X& operator() ( int i ) const { return ((X*)_data)[i]; }

    /*! Returns a const pointer of the internal buffer. The internal buffer 
        will always contain a contiguous storage space of capacity() elements. 
        See also abandon() and adopt() methods. */
    operator const X* () const { return (X*)_data; }

    /*! Returns a reference to the last element, ie, with index size()-1.
        The array must not be empty when calling this method. */
    X& top () const { return ((X*)_data)[_size-1]; }

    /*! Returns a reference to the i-th element below the last element, ie, with index size()-i-1.
        The referenced position must exist in the array. */
    X& top ( int i ) const { return ((X*)_data)[_size-i-1]; }

    /*! Const version of top() method. The array must not be empty when calling this method. */
    const X& ctop () const { return ((X*)_data)[_size-1]; }

    /*! Const version of top(i) method. The referenced position must exist in the array. */
    const X& ctop ( int i ) const { return ((X*)_data)[_size-i-1]; }

    /*! Returns a reference to the last element, ie, with index size()-1, and
        then reduces the size of the array by one with no reallocation.
        The array must not be empty when calling this method. */
    X& pop () { return ((X*)_data)[--_size]; }

    /*! Method to append positions. If reallocation is needed, capacity is set
        to two times the new size. The first new element appended is returned
        as a reference. */
    X& push () { GsArrayBase::push(sizeof(X)); return top(); }

    /*! Pushes one position at the end of the array using the insert() method, and
        then copies the content of x using operator=(). */
    void push ( const X& x ) { GsArrayBase::push(sizeof(X)); top()=x; }

    /*! Inserts n positions, starting at pos i, moving all data correctly. 
        Parameter i can be between 0 and size(), if i==size(), n positions are
        appended. If reallocation is required, capacity is set to two times the
        new size. The first new element inserted (i) is returned as a reference. 
        The quantity of appended positions (n) has a default value of 1. */
    X& insert ( int i, int n=1 ) { GsArrayBase::insert(sizeof(X),i,n); return ((X*)_data)[i]; }

    /*! Removes n positions starting from pos i, moving all data correctly;
        n has a default value of 1. Attention: elements' destructors are not called! */
    void remove ( int i, int n=1 ) { GsArrayBase::remove(sizeof(X),i,n); }

    /*! Copies n entries from src position to dest position. Regions are
        allowed to overlap. Uses the C function memmove.
        No parameter checking is done. */
    void copy ( int dest, int src, int n ) { GsArrayBase::copy(sizeof(X),dest,src,n); }

    /*! Copies n entries from src position to dest position at desta array. */
    void copyto ( GsArray<X>& desta, int dest, int src, int n ) const
     { GsArrayBase::copyto(sizeof(X),desta,dest,src,n); }

    /*! Appends a copy of array a. */
    void push ( const GsArray<X>& a )
     { int n=size(); insert(n,a.size()); a.copyto(*this,n,0,a.size()); }

    /*! Copies all internal data of a to GsArray, with fast memcpy() functions,
        so that the operator=() that X might have is not used. This method has
        no effect if a "self copy" is called. */
    void operator = ( const GsArray<X>& a )
     { GsArrayBase::copyfrom ( sizeof(X), a ); }

    /*! Revert the order of the elements in the array. Copy operator of X is used. */
    void revert ()
     { int i, max=size()-1, mid=size()/2; X tmp;
       for ( i=0; i<mid; i++ ) { GS_SWAP(get(i),get(max-i)); }
     }

    /*! Revert the order of the elements in the range[i,j]. Copy operator of X is used. */
    void revert ( int i, int j )
     { int mid=i+((j-i+1)/2); X tmp;
       for (; i<mid; i++,j-- ) { GS_SWAP(get(i),get(j)); }
     }

    /*! Inserts the element x in the sorted array, moving all data correctly,
        and returns the position of the insertion. The space for insertion
        is created with insert(), and the contents of x are copied with 
        X's copy operator. A compare function int gs_compare(const X*,const X*)
        is required as argument.*/
    int insort ( const X& x, GS_COMPARE_FUNC )
     { int pos = GsArrayBase::insort ( sizeof(X), (void*)&x, (gscompare)cmpfunc );
       get(pos)=x;
       return pos;
     }

    /*! Same as insort(), but will not insert x in case there is already an
        element in the array equal to x. Returns the position of the insertion,
        or -1 if x was not inserted. */
    int uniqinsort ( const X& x, GS_COMPARE_FUNC )
     { int pos = GsArrayBase::uniqinsort ( sizeof(X), (void*)&x, (gscompare)cmpfunc );
       if ( pos>=0 ) get(pos)=x;
       return pos;
     }

    /*! Standard library qsort() wrapper call. The compare function is required
        as argument: int gs_compare(const X*,const X*). For example, for 
        GsArray<X*> the compare function is: int gs_compare(X*const*,X*const*),*/
    void sort ( GS_COMPARE_FUNC ) { GsArrayBase::sort ( sizeof(X), (gscompare)cmpfunc ); }

    /*! Linear search, returns the index of the element found, or -1 if not
        found. A compare function is required as argument:
        int gs_compare(const X*,const X*) */
    int lsearch ( const X& x, GS_COMPARE_FUNC ) const { return GsArrayBase::lsearch ( sizeof(X), (void*)&x, (gscompare)cmpfunc ); }

    /*! Binary search for sorted arrays. Returns index of the element found, 
        or -1 if not found. If not found and pos is not null, pos will have the 
        position to insert the element keeping the array sorted. Faster than
        the standard C library bsearch() for large arrays. A compare function
        is required as argument: int gs_compare(const X*,const X*) */
    int bsearch ( const X& x, GS_COMPARE_FUNC, int *pos=NULL ) const
     { return GsArrayBase::bsearch ( sizeof(X), (void*)&x, (gscompare)cmpfunc, pos ); }

    /*! Returns the internal buffer pointer that will be null or contain the address of
        the memory used and that was allocated with malloc(). The user will then be
        responsible to free this allocated memory with free(). After this call, the 
        array becomes an empty valid array. */
    X* abandon () { return (X*) GsArrayBase::abandon(); }

    /*! Frees the data of GsArray, and then makes GsArray be the given array a.
        After this, a is set to be a valid empty array. The data is moved without
        reallocation. */
    void adopt ( GsArray<X>& a ) { GsArrayBase::adopt ( (GsArrayBase&)a ); }

    /*! Frees the data of GsArray, and then makes GsArray to control the given 
        buffer pt, with size and capacity as given. Its the user reponsibility to
        pass correct values. Note also that the memory menagement of GsArray is
        done with malloc/realloc/free functions. */
    void adopt (  X* pt, int s, int c ) { GsArrayBase::adopt ( pt, s, c ); }

    /*! Output all elements of the array.
        Element type X must have its ouput operator << available.
        The output format is [e0 e1 ... en] */
    friend GsOutput& operator<< ( GsOutput& o, const GsArray<X>& a )
     { int i, m=a.size();
       o << '[';
       for ( i=0; i<m; i++ ) o  << ' ' << a[i];
       return o << ' ' << ']';
     }

    /*! Input all elements of the array from format [e0 e1 ... en].
        Element type X must have its input operator << available.
        The array is compressed after reading the data. */
    friend GsInput& operator>> ( GsInput& in, GsArray<X>& a )
     { a.size(0);
       in.get(); // delimiter [
       while ( in.check()!=GsInput::Delimiter ) in >> a.push();
       a.compress();
       in.get(); // delimiter ]
       return in;
     }
 };

/*! \class GsArrayPt gs_array.h
    \brief GsArray extension for pointers

    GsArrayPt only provides a destructor that will call delete to each element
    in the array at destruction time. It is the user responsibility to 
    maintain the array with properly allocated elements and correctly make
    use of the base class methods */
template <typename X>
class GsArrayPt : public GsArray<X>
 { public:
    /*! Only the default constructor is provided here */
    GsArrayPt () : GsArray<X> () {}

    /*! Destructor deletes each element in the array with delete X */
   ~GsArrayPt () { while(GsArray<X>::size()>0) delete GsArray<X>::pop(); }

    /*! Deletes each element in the array with delete X and set size to 0 */
    void init () { while(GsArray<X>::size()>0) delete GsArray<X>::pop(); }
 };

//============================== end of file ===============================

#endif // GS_ARRAY_H

