#include <iostream>

template <typename T,size_t N>
NDGrid<T,N>::NDGrid(int inWidth)
  : mSize(inWidth)
{

	//设置当前对象的  mCells 保存的对象类型 的数组大小
  mCells = new NDGrid<T,N-1>[mSize];
  std::cout << "constructor begin:\n";
  for(int i=0; i<mSize; ++i)
    {
      //mCells[i] =NDGrid<T,N-1>(mSize);
	  //对每个 NDGrid<T,N-1> 对象进行初始化，  
	  //因此 在调用 resize() 时，此时 N 为 N-1
	  //也可以重新调用 参数构造函数，但会多次构造、赋值、析构函数调用
      mCells[i].resize(inWidth); //设置当前对象的  mCells 保存的对象类型 的大小
    }
  std::cout <<"constructor end\n";
 }

template <typename T,size_t N>
NDGrid<T,N>::NDGrid(const NDGrid<T,N> &src)
{
  copyFrom(src);
}
template <typename T,size_t N>
NDGrid<T,N> & NDGrid<T,N>::operator=(const NDGrid<T,N> &src)
{
  if(this == & src)
    {
      return *this;
    }
  delete [] mCells;
  mCells=nullptr;
  copyFrom(src);
  return *this;
}
template <typename T,size_t N>
NDGrid<T,N>::~NDGrid()
{
  std::cout << "destruct\n";
  delete [] mCells;
  mCells=nullptr;
}
template <typename T,size_t N>
NDGrid<T,N-1>& NDGrid<T,N>::operator[](int x)
{
  return mCells[x];
}
template <typename T,size_t N>
const NDGrid<T,N-1>& NDGrid<T,N>::operator[](int x)const
{
  return mCells[x];
}
template <typename T,size_t N>
void NDGrid<T,N>::copyFrom(const NDGrid<T,N> &src)
{
  mSize = src.mSize;
  mCells = new NDGrid<T,N-1>[mSize];
  for(int i=0; i<mSize; ++i)
    {
      mCells[i]= src[i];
    }
}
template <typename T,size_t N>
void NDGrid<T,N>::resize(size_t inSize)
{
	//调用该方法的对象 的 mCells 保存的对象类型 新分配内存
  NDGrid<T,N-1> * newElmes = new NDGrid<T,N-1>[inSize];
  for(size_t i=0; i<inSize && i<mSize; ++i)
    {
		//赋值 到新地址
      newElmes[i] = mCells[i];
	  //调用 该方法的对象 的 mCells 保存的对象 的 resize()方法
      newElmes[i].resize(inSize);
    }
	//设置调用该方法的对象的 mSize 属性
  mSize=inSize;
  delete [] mCells;
  mCells = newElmes;
}



template <typename T>
NDGrid<T,1>::NDGrid(int inWidth)
  : mSize(inWidth)
{

  mCells = new T[mSize];
  for(int i=0; i<mSize;++i)
    {
      mCells[i]=T();
    }
}

template <typename T>
NDGrid<T,1>::NDGrid(const NDGrid<T,1> &src)
{
  copyFrom(src);
}
template <typename T>
NDGrid<T,1> & NDGrid<T,1>::operator=(const NDGrid<T,1> &src)
{
  if(this == & src)
    {
      return *this;
    }
  delete [] mCells;
  mCells=nullptr;
  copyFrom(src);
  return *this;
}
template <typename T>
NDGrid<T,1>::~NDGrid()
{
  delete [] mCells;
  mCells=nullptr;
}
template <typename T>
T& NDGrid<T,1>::operator[](int x)
{
  return mCells[x];
}
template <typename T>
const T& NDGrid<T,1>::operator[](int x)const
{
  return mCells[x];
}
template <typename T>
void NDGrid<T,1>::copyFrom(const NDGrid<T,1> &src)
{
  mSize = src.mSize;
  mCells = new T[mSize];
  for(int i=0; i<mSize; ++i)
    {
      mCells[i]= src[i];
    }
}
template <typename T>
void NDGrid<T,1>::resize(size_t inSize)
{
  T * newElems = new T[inSize];
  for(size_t i=0; i< inSize && i<mSize; ++i)
    {
      newElems[i] =mCells[i];
    }
  mSize =inSize;
  delete [] mCells;
  mCells = newElems;
}
