/****************************************************************************
Copyright (c) 2010 ForzeField Studios S.L. http://forzefield.com
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#ifndef __CCARRAY_H__
#define __CCARRAY_H__

#define CC_USE_ARRAY_VECTOR 0

#if CC_USE_ARRAY_VECTOR
#include <vector>
#include <algorithm>
#include "cocoa/CCObject.h"
#include "ccMacros.h"
#else
#include "support/data_support/ccCArray.h"
#endif


#if CC_USE_ARRAY_VECTOR
/**
 * A reference counting-managed pointer for classes derived from RCBase which can
 * be used as C pointer
 * Original code: http://www.codeproject.com/Articles/64111/Building-a-Quick-and-Handy-Reference-Counting-Clas
 * License: http://www.codeproject.com/info/cpol10.aspx
 */
template < class T >
class RCPtr
{
public:
	//Construct using a C pointer
	//e.g. RCPtr< T > x = new T();
	RCPtr(T* ptr = nullptr)
    : _ptr(ptr)
	{
        if(ptr != nullptr) {ptr->retain();}
	}

	//Copy constructor
	RCPtr(const RCPtr &ptr)
    : _ptr(ptr._ptr)
	{
//        printf("Array: copy constructor: %p\n", this);
		if(_ptr != NULL) {_ptr->retain();}
	}

    //Move constructor
	RCPtr(RCPtr &&ptr)
    : _ptr(ptr._ptr)
	{
//        printf("Array: Move Constructor: %p\n", this);
        ptr._ptr = nullptr;
	}

	~RCPtr()
	{
//        printf("Array: Destructor: %p\n", this);
        if(_ptr != nullptr) {_ptr->release();}
	}

	//Assign a pointer
	//e.g. x = new T();
	RCPtr &operator=(T* ptr)
	{
//        printf("Array: operator= T*: %p\n", this);

        //The following grab and release operations have to be performed
        //in that order to handle the case where ptr == _ptr
        //(See comment below by David Garlisch)
        if(ptr != nullptr) {ptr->retain();}
        if(_ptr != nullptr) {_ptr->release();}
        _ptr = ptr;
        return (*this);
	}

	//Assign another RCPtr
	RCPtr &operator=(const RCPtr &ptr)
	{
//        printf("Array: operator= const&: %p\n", this);
        return (*this) = ptr._ptr;
	}

	//Retrieve actual pointer
	T* get() const
	{
        return _ptr;
	}

    //Some overloaded operators to facilitate dealing with an RCPtr
    //as a conventional C pointer.
    //Without these operators, one can still use the less transparent
    //get() method to access the pointer.
    T* operator->() const {return _ptr;}		//x->member
    T &operator*() const {return *_ptr;}		//*x, (*x).member
    explicit operator T*() const {return _ptr;}		//T* y = x;
    explicit operator bool() const {return _ptr != nullptr;}	//if(x) {/*x is not NULL*/}
    bool operator==(const RCPtr &ptr) {return _ptr == ptr._ptr;}
    bool operator==(const T *ptr) {return _ptr == ptr;}

private:
    T *_ptr;	//Actual pointer
};
#endif // CC_USE_ARRAY_VECTOR


/**
 * @addtogroup data_structures
 * @{
 */

/** @def CCARRAY_FOREACH
A convenience macro to iterate over a Array using. It is faster than the "fast enumeration" interface.
@since v0.99.4
*/

/*
In cocos2d-iphone 1.0.0, This macro have been update to like this:

#define CCARRAY_FOREACH(__array__, __object__)                                                \
if (__array__ && __array__->data->num > 0)                                                    \
for(id *__arr__ = __array__->data->arr, *end = __array__->data->arr + __array__->data->num-1;    \
__arr__ <= end && ((__object__ = *__arr__) != nil || true);                                        \
__arr__++)

I found that it's not work in C++. So it keep what it's look like in version 1.0.0-rc3. ---By Bin
*/

#if CC_USE_ARRAY_VECTOR
#define CCARRAY_FOREACH(__array__, __object__)                  \
    if (__array__) \
    for( auto __it__ = (__array__)->data.begin();              \
        __it__ != (__array__)->data.end() && ((__object__) = __it__->get()) != nullptr;                     \
        ++__it__)


#define CCARRAY_FOREACH_REVERSE(__array__, __object__)          \
    if (__array__) \
    for( auto __it__ = (__array__)->data.rbegin();             \
    __it__ != (__array__)->data.rend() && ((__object__) = __it__->get()) != nullptr;                        \
    ++__it__ )


#define CCARRAY_VERIFY_TYPE(__array__, __type__) void(0)

#else // ! CC_USE_ARRAY_VECTOR --------------------------

#define CCARRAY_FOREACH(__array__, __object__)                                                                         \
    if ((__array__) && (__array__)->data->num > 0)                                                                     \
    for(Object** __arr__ = (__array__)->data->arr, **__end__ = (__array__)->data->arr + (__array__)->data->num-1;    \
    __arr__ <= __end__ && (((__object__) = *__arr__) != NULL/* || true*/);                                             \
    __arr__++)

#define CCARRAY_FOREACH_REVERSE(__array__, __object__)                                                                  \
    if ((__array__) && (__array__)->data->num > 0)                                                                      \
    for(Object** __arr__ = (__array__)->data->arr + (__array__)->data->num-1, **__end__ = (__array__)->data->arr;     \
    __arr__ >= __end__ && (((__object__) = *__arr__) != NULL/* || true*/);                                              \
    __arr__--)

#if defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)
#define CCARRAY_VERIFY_TYPE(__array__, __type__)                                                                 \
    do {                                                                                                         \
        if ((__array__) && (__array__)->data->num > 0)                                                           \
            for(Object** __arr__ = (__array__)->data->arr,                                                     \
                **__end__ = (__array__)->data->arr + (__array__)->data->num-1; __arr__ <= __end__; __arr__++)    \
                CCASSERT(dynamic_cast<__type__>(*__arr__), "element type is wrong!");                            \
    } while(false)
#else
#define CCARRAY_VERIFY_TYPE(__array__, __type__) void(0)
#endif

#endif // ! CC_USE_ARRAY_VECTOR


// Common defines -----------------------------------------------------------------------------------------------

#define arrayMakeObjectsPerformSelector(pArray, func, elementType)    \
do {                                                                  \
    if(pArray && pArray->count() > 0)                                 \
    {                                                                 \
        Object* child;                                                \
        CCARRAY_FOREACH(pArray, child)                                \
        {                                                             \
            elementType pNode = static_cast<elementType>(child);      \
            if(pNode)                                                 \
            {                                                         \
                pNode->func();                                        \
            }                                                         \
        }                                                             \
    }                                                                 \
}                                                                     \
while(false)

#define arrayMakeObjectsPerformSelectorWithObject(pArray, func, object, elementType)   \
do {                                                                  \
    if(pArray && pArray->count() > 0)                                 \
    {                                                                 \
        Object* child;                                                \
        CCARRAY_FOREACH(pArray, child)                                \
        {                                                             \
            elementType pNode = static_cast<elementType>(child);      \
            if(pNode)                                                 \
            {                                                         \
                pNode->func(object);                                 \
            }                                                         \
        }                                                             \
    }                                                                 \
}                                                                     \
while(false)


NS_CC_BEGIN

class CC_DLL Array : public Object, public Clonable
{
public:

    /** Creates an empty array. Default capacity is 10 */
    static Array* create();
    /** Create an array with objects */
    static Array* create(Object* object, ...) CC_REQUIRES_NULL_TERMINATION;
    /** Create an array with one object */
    static Array* createWithObject(Object* object);
    /** Create an array with a default capacity */
    static Array* createWithCapacity(unsigned int capacity);
    /** Create an array with from an existing array */
    static Array* createWithArray(Array* otherArray);
    /**
     @brief   Generate a Array pointer by file
     @param   pFileName  The file name of *.plist file
     @return  The Array pointer generated from the file
     */
    static Array* createWithContentsOfFile(const char* pFileName);
    
    /*
     @brief The same meaning as arrayWithContentsOfFile(), but it doesn't call autorelease, so the
     invoker should call release().
     */
    static Array* createWithContentsOfFileThreadSafe(const char* pFileName);
    
    ~Array();

    /** Initializes an array */
    bool init();
    /** Initializes an array with one object */
    bool initWithObject(Object* object);
    /** Initializes an array with some objects */
    bool initWithObjects(Object* object, ...) CC_REQUIRES_NULL_TERMINATION;
    /** Initializes an array with capacity */
    bool initWithCapacity(unsigned int capacity);
    /** Initializes an array with an existing array */
    bool initWithArray(Array* otherArray);

    // Querying an Array

    /** Returns element count of the array */
    unsigned int count() const
    {
#if CC_USE_ARRAY_VECTOR
        return data.size();
#else
        return data->num;
#endif
    }
    /** Returns capacity of the array */
    unsigned int capacity() const
    {
#if CC_USE_ARRAY_VECTOR
        return data.capacity();
#else
        return data->max;
#endif
    }
    /** Returns index of a certain object, return UINT_MAX if doesn't contain the object */
    int getIndexOfObject(Object* object) const;
    CC_DEPRECATED_ATTRIBUTE int indexOfObject(Object* object) const { return getIndexOfObject(object); }

    /** Returns an element with a certain index */
    Object* getObjectAtIndex(int index)
    {
        CCASSERT(index>=0 && index < count(), "index out of range in getObjectAtIndex()");
#if CC_USE_ARRAY_VECTOR
        return data[index].get();
#else
        return data->arr[index];
#endif
    }
    CC_DEPRECATED_ATTRIBUTE Object* objectAtIndex(int index) { return getObjectAtIndex(index); }
    /** Returns the last element of the array */
    Object* getLastObject()
    {
#if CC_USE_ARRAY_VECTOR
        return data.back().get();
#else
        if(data->num > 0)
            return data->arr[data->num-1];
        
        return nullptr;
#endif
    }
    CC_DEPRECATED_ATTRIBUTE Object* lastObject() { return getLastObject(); }
    /** Returns a random element */
    Object* getRandomObject();
    CC_DEPRECATED_ATTRIBUTE Object* randomObject() { return getRandomObject(); }
    /** Returns a Boolean value that indicates whether object is present in array. */
    bool containsObject(Object* object) const;
    /** @since 1.1 */
    bool isEqualToArray(Array* otherArray);
    // Adding Objects

    /** Add a certain object */
    void addObject(Object* object);
    /** Add all elements of an existing array */
    void addObjectsFromArray(Array* otherArray);
    /** Insert a certain object at a certain index */
    void insertObject(Object* object, int index);
    /** sets a certain object at a certain index */
    void setObject(Object* object, int index);
    /** sets a certain object at a certain index without retaining. Use it with caution */
    void fastSetObject(Object* object, int index)
    {
#if CC_USE_ARRAY_VECTOR
        setObject(object, index);
#else
        // no retain
        data->arr[index] = object;
#endif
    }

    void swap( int indexOne, int indexTwo )
    {
        CCASSERT(indexOne >=0 && indexOne < count() && indexTwo >= 0 && indexTwo < count(), "Invalid indices");
#if CC_USE_ARRAY_VECTOR
        std::swap(data[indexOne], data[indexTwo]);
#else
        std::swap(data->arr[indexOne], data->arr[indexTwo]);
#endif
    }

    // Removing Objects

    /** Remove last object */
    void removeLastObject(bool releaseObj = true);
    /** Remove a certain object */
    void removeObject(Object* object, bool releaseObj = true);
    /** Remove an element with a certain index */
    void removeObjectAtIndex(unsigned int index, bool releaseObj = true);
    /** Remove all elements */
    void removeObjectsInArray(Array* otherArray);
    /** Remove all objects */
    void removeAllObjects();
    /** Fast way to remove a certain object */
    void fastRemoveObject(Object* object);
    /** Fast way to remove an element with a certain index */
    void fastRemoveObjectAtIndex(unsigned int index);

    // Rearranging Content

    /** Swap two elements */
    void exchangeObject(Object* object1, Object* object2);
    /** Swap two elements with certain indexes */
    void exchangeObjectAtIndex(unsigned int index1, unsigned int index2);

    /** Replace object at index with another object. */
    void replaceObjectAtIndex(unsigned int index, Object* object, bool releaseObject = true);

    /** Revers the array */
    void reverseObjects();
    /* Shrinks the array so the memory footprint corresponds with the number of items */
    void reduceMemoryFootprint();
  
    /* override functions */
    virtual void acceptVisitor(DataVisitor &visitor);
    virtual Array* clone() const;

    // ------------------------------------------
    // Iterators
    // ------------------------------------------
#if CC_USE_ARRAY_VECTOR
    typedef std::vector<RCPtr<Object>>::iterator iterator;
    typedef std::vector<RCPtr<Object>>::const_iterator const_iterator;

    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    const_iterator cbegin() { return data.cbegin(); }
    const_iterator cend() { return data.cend(); }

#else
    class ArrayIterator : public std::iterator<std::input_iterator_tag, Object*>
    {
    public:
        ArrayIterator(int index, Array *array) : _index(index), _parent(array) {}
        ArrayIterator(const ArrayIterator& arrayIterator) : _index(arrayIterator._index), _parent(arrayIterator._parent) {}

        ArrayIterator& operator++()
        {
            ++_index;
            return *this;
        }
        ArrayIterator operator++(int dummy)
        {
            ArrayIterator tmp(*this);
            ++_index;
            return tmp;
        }
        
        ArrayIterator& operator--()
        {
            --_index;
            return *this;
        }
        ArrayIterator operator--(int dummy)
        {
            ArrayIterator tmp(*this);
            --_index;
            return tmp;
        }
        
        int operator-(const ArrayIterator& rhs) const
        {
            return _index - rhs._index;
        }
        ArrayIterator operator-(int d)
        {
            _index -= d;
            return *this;
        }
        const ArrayIterator& operator-=(int d)
        {
            _index -= d;
            return *this;
        }
        
        ArrayIterator operator+(int d)
        {
            _index += d;
            return *this;
        }
        
        const ArrayIterator& operator+=(int d)
        {
            _index += d;
            return *this;
        }
        
        // add these function to make compiler happy when using std::sort(), it is meaningless
        bool operator>=(const ArrayIterator& rhs) const { return false; }
        bool operator<=(const ArrayIterator& rhs) const { return false; }
        bool operator>(const ArrayIterator& rhs) const { return false; }
        bool operator<(const ArrayIterator& rhs) const { return false; }
        
        bool operator==(const ArrayIterator& rhs) { return _index == rhs._index; }
        bool operator!=(const ArrayIterator& rhs) { return _index != rhs._index; }
        reference operator*() { return _parent->data->arr[_index]; }
        value_type operator->() { return _parent->data->arr[_index];; }

    private:
        int _index;
        Array *_parent;
    };

    // functions for range-based loop
    typedef ArrayIterator iterator;
    typedef ArrayIterator const_iterator;
    iterator begin();
    iterator end();

#endif
    


public:
#if CC_USE_ARRAY_VECTOR
    std::vector<RCPtr<Object>> data;
#else
    ccArray* data;
#endif

    Array();
    Array(unsigned int capacity);
};

// end of data_structure group
/// @}

NS_CC_END

#endif // __CCARRAY_H__
