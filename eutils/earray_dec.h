#ifndef EARRAY_DEC_H
#define EARRAY_DEC_H

#include "eutils.h"
#include "ebasicarray_dec.h"
#include "ebasearray.h"

using namespace std;
class estr;

template <class T>
class earray;

template <class T>
ostream &operator<<(ostream &stream,const earray<T> &var);

template <class T>
class earray : public ebasicarray<T*>
{
 public:
   earray();
//   earray(const T* arr,int n);
   earray(const earray<T>& arr);
   earray(const T& value);
   earray(const T& value1,const T& value2);
   earray(const T& value1,const T& value2,const T& value3);
   earray(const T& value1,const T& value2,const T& value3,const T& value4);
   earray(const T& value1,const T& value2,const T& value3,const T& value4,const T& value5);
   earray(const T& value1,const T& value2,const T& value3,const T& value4,const T& value5,const T& value6);
   earray(const T& value1,const T& value2,const T& value3,const T& value4,const T& value5,const T& value6,const T& value7);
   earray(const T& value1,const T& value2,const T& value3,const T& value4,const T& value5,const T& value6,const T& value7,const T& value8);
   earray(const T& value1,const T& value2,const T& value3,const T& value4,const T& value5,const T& value6,const T& value7,const T& value8,const T& value9);
   ~earray();

   void init(size_t count);
   void init(size_t count,const T& initval);

   inline void reserve(size_t rsize) { ebasicarray<T*>::reserve(rsize); }
   inline size_t size() const { return(ebasicarray<T*>::size()); }

   earray<T>& operator=(const earray<T> &arr);

   inline       T& operator[](size_t i)       { return(*ebasicarray<T*>::operator[](i)); }
   inline const T& operator[](size_t i) const { return(*ebasicarray<T*>::operator[](i)); }

   earray<T> operator[](const eintarray& i) const;

   inline       T& at(size_t i)               { return(operator[](i)); }
   inline const T& at(size_t i) const         { return(operator[](i)); }
   
   inline       T& back()                  { return(*ebasicarray<T*>::back()); }
   inline const T& back() const            { return(*ebasicarray<T*>::back()); }

   inline earray<T>& operator+=(const T& value) { add(value); return(*this); }
   inline T&         add(const T& value)        { ebasicarray<T*>::push_back(new T(value)); return(back()); }
   inline T&         addref(T* value)        { ebasicarray<T*>::push_back(value); return(back()); }
   inline T&         insert(size_t i,const T& value)     { ebasicarray<T*>::insert(i,new T(value)); return(back()); }
   inline T&         insertref(size_t i,T* value)     { ebasicarray<T*>::insert(i,value); return(back()); }

   inline void erase(size_t i);

   inline void swap(size_t i,size_t j){ ebasicarray<T*>::swap(i,j); }  //T *tmp; tmp=ebasicarray<T*>::operator[](j); ebasicarray<T*>::operator[](j)=ebasicarray<T*>::operator[](i); ebasicarray<T*>::operator[](i)=tmp; }

   earray<T>& operator+=(const earray<T>& arr);
   earray<T>& operator-=(const earray<T>& arr);

   earray<T> operator+(const earray<T>& arr) const;
   earray<T> operator-(const earray<T>& arr) const;

//   inline const T operator[](int i) const { return( ((ebasicarray<T*>)(*this))[i] ); }
//   inline T &operator[](int i){ return( ((ebasicarray<T*>)(*this))[i] ); }

//   earray<T> operator[](const earray<int> &iarr);
   earray<T> subset(long i,long l=-1) const;
 
   void clear();

   static bool equal(const T &a,const T &b){ return(a==b); }


   long find(const T &value,size_t i=0) const; // less general function added due to efficiency
 
   long find(const T &value,size_t i,bool (*match)(const T&,const T&)) const;   
   eintarray findall(const T &value,size_t i=0,bool (*match)(const T&,const T&)=equal) const;
   eintarray findall(const earray<T> &value,size_t i=0,bool (*match)(const T&,const T&)=equal) const;
   
//   earray<T> afindall(const T &value,int i=0,bool (*match)(const T&,const T&)=equal) const;
   earray<T> afindall(const earray<T> &value,size_t i=0,bool (*match)(const T&,const T&)=equal) const;

//   void erase(const earray<int> &iarr);
   friend ostream &operator<< <>(ostream &stream,const earray<T> &var);

   virtual void addvar(evar& var);
   virtual evar getvar(size_t i) const;

   void serial(estr& data) const;
   size_t unserial(const estr& data,size_t i=0);
};

#endif

