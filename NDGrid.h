#ifndef ND_GRID_H
#define ND_GRID_H
template<typename T,size_t N>
 class NDGrid
{
 public:
  NDGrid(int inWidth=kDefaultWidth);
  NDGrid(const NDGrid<T,N> &src);
  NDGrid<T,N> & operator=(const NDGrid<T,N> &src);
  virtual ~NDGrid();
  //NDGrid<T,N-1>& getElementAt(int x) const;
  //NDGrid<T,N-1>& getElementAt(int x) ;
  //void setElement(int x, const NDGrid<T,N-1> & val);
  NDGrid<T,N-1>& operator[](int x);
  const NDGrid<T,N-1>& operator[](int x)const;
  size_t getSize() const{return mSize;}
  void resize(size_t inSize);

 protected:
   NDGrid<T,N-1>* mCells;
  const static  int kDefaultWidth=10;
  void copyFrom(const NDGrid<T,N> &src);
  size_t mSize;
};

template<typename T>
class NDGrid<T,1>
{
 public:
  NDGrid(int inWidth=kDefaultWidth);
  NDGrid(const NDGrid<T,1> &src);
  NDGrid<T,1> & operator=(const NDGrid<T,1> &src);
  virtual ~NDGrid();
  //NDGrid<T,N-1>& getElementAt(int x) const;
  //NDGrid<T,N-1>& getElementAt(int x) ;
  //void setElement(int x, const NDGrid<T,N-1> & val);
  T& operator[](int x);
  const T& operator[](int x)const;
  size_t getSize() const{return mSize;}
  void resize(size_t inSize);

 protected:
  T * mCells;
  const static  int kDefaultWidth=10;
  void copyFrom(const NDGrid<T,1> &src);
  size_t mSize;
};

#include "NDGrid.cpp"
#endif
