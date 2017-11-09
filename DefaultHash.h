#ifndef HASH_MAP_H
#define HASH_MAP_H
#include <iostream>
#include <stdexcept>
#include <string>
#include <functional>
#include <utility>
#include <list>
#include <vector>
#include <initializer_list>
#include <iterator>

using namespace std;
template <typename T>
class DefaultHash
{
public:
    DefaultHash(size_t numBuckets=101) throw (invalid_argument);
    size_t hash(const T & key ) const;
    size_t getBucketNum()const {return mNumBuckets;}
protected:
    size_t mNumBuckets;
};

template <typename T>
DefaultHash<T>::DefaultHash(size_t numBuckets) throw(invalid_argument)
{
    if(numBuckets <=0)
    {
        throw invalid_argument(" numBuckets must be  > 0");
    }
    mNumBuckets = numBuckets;
}
//生成 hash 值
//对于 string 类型，则会对 string 对象进行处理，而不是字符串内容
//因此造成同一个字符串内容 的 hash 值不同
//故 对string 类型进行具体化模板
template <typename T>
size_t DefaultHash<T>::hash(const T & key) const
{
    size_t bytes = sizeof(key);
    size_t res=0;
    for(size_t i =0; i<bytes; ++i)
    {
        unsigned char b =*((unsigned char*)&key+i);
        res +=b;
    }
    return (res% mNumBuckets);
}

//string 类型 具体化模板
template <>
class DefaultHash<string>
{
public:
    DefaultHash(size_t numBuckets=101) throw (invalid_argument);
    size_t hash(const string & key ) const;
    size_t getBucketNum()const {return mNumBuckets;}
protected:
    size_t mNumBuckets;
};

DefaultHash<string>::DefaultHash(size_t numBuckets) throw(invalid_argument)
{
    if(numBuckets <=0)
    {
        throw invalid_argument(" numBuckets must be  > 0");
    }
    mNumBuckets = numBuckets;
}

size_t DefaultHash<string>::hash(const string & key) const
{
    size_t bytes = key.size();
    size_t res=0;
    for(size_t i =0; i<bytes; ++i)
    {
        
        res +=(unsigned char) key[i];
    }
    return (res % mNumBuckets);
}

//hashmap接口
//声明迭代器 模板
template<typename Key,typename T, typename Compare, typename Hash>
    class HashIterator;

template <typename Key, typename T, typename Compare =std::equal_to<Key>,
          typename Hash=DefaultHash<Key> >
class hashmap
{
public:

    typedef Key key_type;

   typedef T mapped_type;
   //实现 stl 标准
   typedef pair<const Key, T> value_type;
typedef pair<const Key,T>&  reference;  //元素引用类型
typedef const pair<const Key,T>& const_reference; //元素const 引用类型
typedef size_t size_type; //元素个数 类型
typedef ptrdiff_t difference_type; //容器的两个 iterator 差值类型
	//实现 关联容器
	typedef Compare key_compare;
	//值比较器
	class value_compare: public binary_function<value_type,value_type,bool>
	{
		friend class hashmap<Key,T,Compare,Hash>;
		public:
		bool operator()(const value_type & val1, const value_type & val2) const
		{
			return comp(val1.first,val2.first);
		}
		protected:
		Compare comp;
		value_compare(Compare c):comp(c){}
	};
		
    explicit hashmap(const Compare & comp =Compare(),
                     const Hash& hash=Hash()) throw(invalid_argument);
    ~hashmap();
    hashmap(const hashmap<Key,T,Compare,Hash> & src);
    hashmap( hashmap<Key,T,Compare,Hash> && src);
    hashmap<Key,T,Compare,Hash>& operator=(const hashmap<Key,T,Compare,Hash> & rhs ) ;
    hashmap<Key,T,Compare,Hash>& operator=(hashmap<Key,T,Compare,Hash> && rhs ) ;
	//修改以下方法为标准 方法	
    //void insert(const value_type & x);
    //void erase(const key_type & x);
    //value_type * find(const key_type & x) const;
    T & operator[](const key_type & x);
// stl 标准方法
bool empty() const;
size_type size() const;
size_type max_size() const;
//交换 mElems mSize mComp mHash 成员
void swap(hashmap<Key,T,Compare,Hash>& hashIn);
//关联容器 方法
template <typename InputIterator>
hashmap(InputIterator first,InputIterator last,
		const Compare & comp=Compare(), const Hash& hash=Hash()
		)throw(invalid_argument);
//C++11 
hashmap(initializer_list<value_type> il,const Compare & comp=Compare(),
		const Hash & hash=Hash()
		)throw(invalid_argument);
hashmap<Key,T,Compare,Hash> & operator=(initializer_list<value_type> il)
		throw(invalid_argument); 
bool operator==(const hashmap<Key,T,Compare,Hash>& hashIn);
bool operator!=(const hashmap<Key,T,Compare,Hash>& hashIn);
bool operator<(const hashmap<Key,T,Compare,Hash>& hashIn);
bool operator>(const hashmap<Key,T,Compare,Hash>& hashIn);
bool operator<=(const hashmap<Key,T,Compare,Hash>& hashIn);
bool operator>=(const hashmap<Key,T,Compare,Hash>& hashIn);

typedef HashIterator<Key,T,Compare,Hash> iterator;
typedef HashIterator<Key,T,Compare,Hash> const_iterator;
iterator begin();
iterator end();
const_iterator begin() const;
const_iterator end() const;
const_iterator cbegin() const;
const_iterator cend() const;

//insert()
pair<iterator,bool> insert(const value_type & x);
iterator insert(iterator position,const value_type & x);
//跨容器插入
template <typename InputIterator>
void insert(InputIterator first,InputIterator last);
void insert(initializer_list<value_type> il);
//c++11 emplace()
pair<iterator,bool> emplace(value_type && x);
iterator emplace_hint(iterator hint,value_type && x);
//元素删除
void erase(iterator position);
size_type erase(const key_type & x);
void erase(iterator first,iterator last);
void clear();
//stl 标准方法
key_compare key_comp() const;
value_compare value_comp() const;
iterator find(const key_type & x);
const_iterator find(const key_type & x) const;
size_type count(const key_type & x) const;
protected:
    typedef list<value_type> ListType;
    vector<ListType> * mElems;
    size_t mSize;
    Compare mComp;
    Hash mHash;
    typename list<pair<const Key,T> >::iterator findElement(const key_type & x, size_t & bucket) const;
friend class HashIterator<Key,T,Compare,Hash>;

};

template <typename  Key, typename T, typename Compare,typename Hash >
    hashmap<Key,T,Compare,Hash>::hashmap(const Compare & comp ,const Hash& hash) throw(invalid_argument)
{
    mSize=0;
    mComp=comp;
    mHash=hash;
    if(mHash.getBucketNum()<0)
    {
        throw invalid_argument("Number of buckets must be positive");
    }
    //指定vector 大小为 桶 的最大数
    mElems = new vector<ListType>(mHash.getBucketNum());
}
template <typename  Key, typename T, typename Compare,typename Hash  >
hashmap<Key,T,Compare,Hash>::~hashmap()
{
    mSize=0;
    delete mElems;
    mElems=nullptr;
}
template <typename  Key, typename T, typename Compare,typename Hash  >
hashmap<Key,T,Compare,Hash>::hashmap(const hashmap<Key,T,Compare,Hash> & src)
{
    mSize=src.mSize;
    mComp=src.mComp;
    mHash=src.mHash;
    //调用 复制构造函数
    mElems= new vector<ListType>(*(src.mElems));
    /*
    if(mSize>0)
    {
        for(int i=0 ; i< mSize; i++)
        {
            /*
            list<value_type> temp_list;
            auto temp_end = (src.mElems)[i].cend();
            for(auto it=(src.mElems)[i].cbegin(); it !=temp_end; ++it)
            {
                temp_list.push_back(*it);
            }
            (*mElems)[i]=temp_list;
            
    //  (*mElems)[i]=(src.mElems)[i];
        }
    }
    */
    
}
template <typename  Key, typename T, typename Compare,typename Hash  >
hashmap<Key,T,Compare,Hash>::hashmap( hashmap<Key,T,Compare,Hash> && src)
{
    mSize=src.mSize;
    mComp=src.mComp;
    mHash=src.mHash;
    mElems=src.mElems;
    //  delete src.mElems;
    src.mElems=nullptr;
    src.mSize=0;
    
    
}
template <typename  Key, typename T, typename Compare,typename Hash >
hashmap<Key,T,Compare,Hash>& hashmap<Key,T,Compare,Hash>::operator=(const hashmap<Key,T,Compare,Hash> & rhs )
{
    if(this == &rhs)
    {
        return *this;
    }
    mSize=rhs.mSize;
    mComp=rhs.mComp;
    mHash=rhs.mHash;
    //清除当前对象的分配内存
    delete mElems;
    //调用 复制构造函数
    mElems= new vector<ListType>(*(rhs.mElems)) ;
    /*
    if(mSize>0)
    {
        for(int i=0; i< mSize; ++i)
        {
            (*mElems)[i]=(src.mElems)[i];
        }
    }
    */
    return *this;

}
//移动赋值运算
template <typename  Key, typename T, typename Compare,typename Hash  >
hashmap<Key,T,Compare,Hash>& hashmap<Key,T,Compare,Hash>::operator=(hashmap<Key,T,Compare,Hash> && rhs )
{
    if(this == &rhs)
    {
        return *this;
    }
    mSize=rhs.mSize;
    mComp=rhs.mComp;
    mHash=rhs.mHash;
    //清除当前对象的分配内存
    delete mElems;
    
    mElems= rhs.mElems ;
    rhs.mElems=nullptr;
    rhs.mSize=0;
    /*
    if(mSize>0)
    {
        for(int i=0; i< mSize; ++i)
        {
            (*mElems)[i]=(src.mElems)[i];
        }
    }
    */
    return *this;

}

//查找元素
template <typename  Key, typename T, typename Compare,typename Hash >
typename list<pair<const Key,T> >::iterator hashmap<Key,T,Compare,Hash>::findElement(const key_type & x, size_t & bucket) const
{
        size_t index = mHash.hash(x);
        bucket=index;
        for(auto it =(*mElems)[index].begin();
            it !=(*mElems)[index].end();
            ++it
            )
        {
            //调用 mComp()比较函数判断键名是否相同
            if(mComp(it->first,x))
            {
                
                return it;
            }
        }
        return (*mElems)[index].end();
}
//插入元素， 不能插入相同元素
/*
template <typename  Key, typename T, typename Compare,typename Hash >
void hashmap<Key,T,Compare,Hash>::insert(const value_type & x) 
{
    size_t bucket;
    auto it = findElement(x.first,bucket);
    if(it != (*mElems)[bucket].end())
    {
        return ;
        
        
    }
    //执行插入操作
    mSize++;
    //vector 中的元素已在创建时初始化
    (*mElems)[bucket].insert((*mElems)[bucket].end(),x);
    
}
template <typename  Key, typename T, typename Compare , typename Hash  >
void hashmap<Key,T,Compare,Hash>::erase(const key_type & x)
{
    size_t bucket;
    auto it = findElement(x,bucket);
    if(it == (*mElems)[bucket].end())
    {
        return ;
    }
    (*mElems)[bucket].erase(it);
    --mSize;
    
    
}
//查找元素，没有找到返回 nullptr
template <typename  Key, typename T, typename Compare ,  typename Hash  >
  typename  hashmap<Key,T,Compare,Hash>::value_type * hashmap<Key,T,Compare,Hash>::find(const key_type & x) const
{
    size_t bucket;
    auto it = findElement(x,bucket);

    if(it == (*mElems)[bucket].end())
    {
        return nullptr;
    }
    //指针类型转化
    return &(*it);
}
//当没有找到时，则执行插入操作
////返回引用，因此 对返回值进行修改，会影响到原值 
template <typename  Key, typename T, typename Compare,  typename Hash >
T & hashmap<Key,T,Compare,Hash>::operator[](const key_type & x)
{
    value_type * value = find(x);
    if(value ==nullptr)
    {
        insert(make_pair(x,T()));
        value= find(x); 
    }
    return value->second;
}
*/
template <typename  Key, typename T, typename Compare,  typename Hash >
    bool hashmap<Key,T,Compare,Hash>::empty() const
{
    return mSize==0;
}
template <typename  Key, typename T, typename Compare,  typename Hash >
typename  hashmap<Key,T,Compare,Hash>::size_type hashmap<Key,T,Compare,Hash>::size() const
{
    return mSize;
}
template <typename  Key, typename T, typename Compare,  typename Hash >
typename hashmap<Key,T,Compare,Hash>::size_type hashmap<Key,T,Compare,Hash>::max_size() const
{
    return (*mElems)[0].max_size();
}
template <typename  Key, typename T, typename Compare,  typename Hash >
void hashmap<Key,T,Compare,Hash>::swap(hashmap<Key,T,Compare,Hash>& hashIn)
{
    //调用 std::swap() 需指定限定符，否则造成递归调用该方法
    std::swap(*this,hashIn);
}
template <typename  Key, typename T, typename Compare,  typename Hash >
bool hashmap<Key,T,Compare,Hash>::operator==(const hashmap<Key,T,Compare,Hash>& hashIn)
{
    if(mSize !=hashIn.mSize)
    {
        return false;
    }
    size_t len = mElems->size();
    size_t in_len =(hashIn.mElems)->size();
    if(len !=in_len)
    {
        return false;
    }
    for(int i=0; i<len; ++i)
    {
        if((*mElems)[i] != (*(hashIn.mElems))[i])
        {
            return false;
        }
    }
    return true;
}
template <typename  Key, typename T, typename Compare,  typename Hash >
bool hashmap<Key,T,Compare,Hash>::operator!=(const hashmap<Key,T,Compare,Hash>& hashIn)
{
    return !(this->operator==(hashIn));
}
template <typename  Key, typename T, typename Compare,  typename Hash >
bool hashmap<Key,T,Compare,Hash>::operator<(const hashmap<Key,T,Compare,Hash>& hashIn)
{
    size_t len = mElems->size();
    
    for(int i=0; i<len; ++i)
    {
        if((*mElems)[i] < (*(hashIn.mElems))[i])
        {
            return true;
        }else if((*mElems)[i]>(*(hashIn.mElems))[i])
        {
            return false;
        }
    }
    //元素相同时，比较长度大小
       if(mSize <hashIn.mSize)
    {
        return true;
    }else if(mSize >hashIn.mSize)
       {
           return false;
       }
 
    size_t in_len =(hashIn.mElems)->size();
    if(len <=in_len)
    {
        return false;
    }else{
        return true;
    }
}
template <typename  Key, typename T, typename Compare,  typename Hash >
bool hashmap<Key,T,Compare,Hash>::operator>(const hashmap<Key,T,Compare,Hash>& hashIn)
{
    return !(this->operator<(hashIn)); 
}

template <typename  Key, typename T, typename Compare,  typename Hash >
bool hashmap<Key,T,Compare,Hash>::operator<=(const hashmap<Key,T,Compare,Hash>& hashIn)
{
    size_t len = mElems->size();
    
    for(int i=0; i<len; ++i)
    {
        if((*mElems)[i] < (*(hashIn.mElems))[i])
        {
            return true;
        }else if((*mElems)[i]>(*(hashIn.mElems))[i])
        {
            return false;
        }
    }
    
    //元素相同时，比较长度大小
       if(mSize <hashIn.mSize)
    {
        return true;
    }else if(mSize > hashIn.mSize)
       {
           return false;
       }
 
    size_t in_len =(hashIn.mElems)->size();
    if(len <in_len)
    {
        return false;
    }else{
        return true;
    }
}
template <typename  Key, typename T, typename Compare,  typename Hash >
bool hashmap<Key,T,Compare,Hash>::operator>=(const hashmap<Key,T,Compare,Hash>& hashIn)
{
    return !(this->operator<=(hashIn));
}
template <typename  Key, typename T, typename Compare,  typename Hash >
typename hashmap<Key,T,Compare,Hash>::iterator hashmap<Key,T,Compare,Hash>::begin()
{
    if(mSize ==0)
    {
        return end();
    }
    for(size_t i=0 ; i<mElems->size(); ++i)
    {
        if(!(*mElems)[i].empty())
        {
            return HashIterator<Key,T,Compare,Hash>(i,(*mElems)[i].begin(),this);
        }
    }
    return end();
}
template <typename  Key, typename T, typename Compare,  typename Hash >
typename hashmap<Key,T,Compare,Hash>::iterator hashmap<Key,T,Compare,Hash>::end()
{
    return HashIterator<Key,T,Compare,Hash>(mElems->size()-1,
                                            (*mElems)[mElems->size()-1].end(),this); 
}
template <typename  Key, typename T, typename Compare,  typename Hash >
typename hashmap<Key,T,Compare,Hash>::const_iterator hashmap<Key,T,Compare,Hash>::begin() const
{
    return begin();
}
template <typename  Key, typename T, typename Compare,  typename Hash >
typename hashmap<Key,T,Compare,Hash>::const_iterator hashmap<Key,T,Compare,Hash>::end() const
{
    return end();
}
template <typename  Key, typename T, typename Compare,  typename Hash >
typename hashmap<Key,T,Compare,Hash>::const_iterator hashmap<Key,T,Compare,Hash>::cbegin() const
{
    return begin();
}
template <typename  Key, typename T, typename Compare,  typename Hash >
typename hashmap<Key,T,Compare,Hash>::const_iterator hashmap<Key,T,Compare,Hash>::cend() const
{
    return end();
}

//关联容器 方法
template <typename  Key, typename T, typename Compare,  typename Hash >
template <typename InputIterator>
hashmap<Key,T,Compare,Hash>::hashmap(InputIterator first,InputIterator last,
		const Compare & comp, const Hash& hash
		)throw(invalid_argument)
		:mSize(0),mComp(comp), mHash(hash)
		{
			if(mHash.getBucketNum() <=0)
			{
				throw invalid_argument("Number of buckets must >0");
			}
			mElems = new vector<ListType>(mHash.getBucketNum());
			insert(first,last);
		}
//C++11 
template <typename  Key, typename T, typename Compare,  typename Hash >
hashmap<Key,T,Compare,Hash>::hashmap(initializer_list<value_type> il,const Compare & comp,
		const Hash & hash
		)throw(invalid_argument)
		:mSize(0),mComp(comp), mHash(hash)
		{
			if(mHash.getBucketNum() <=0)
			{
				throw invalid_argument("Number of buckets must >0");
			}
			mElems = new vector<ListType>(mHash.getBucketNum());
			insert(il.begin(),il.end());
		}
		template <typename  Key, typename T, typename Compare,  typename Hash >
  hashmap<Key,T,Compare,Hash> & hashmap<Key,T,Compare,Hash>::operator=(initializer_list<value_type> il)
		throw(invalid_argument)
		{
			clear();
			insert(il.begin(),il.end());
			return *this;
		}	
//insert()
template <typename  Key, typename T, typename Compare,  typename Hash >
pair<typename hashmap<Key,T,Compare,Hash>::iterator,bool> hashmap<Key,T,Compare,Hash>::insert(const value_type & x)
{
	size_t bucket;
    auto it = findElement(x.first,bucket);
    if(it != (*mElems)[bucket].end())
    {
        return  make_pair(HashIterator<Key,T,Compare,Hash>(bucket,it,this),false);
        
        
    }
    //执行插入操作
    mSize++;
    //vector 中的元素已在创建时初始化
	// list insert() 方法只 返回插入的元素的迭代器，而不是 pair 类型
    auto res= (*mElems)[bucket].insert(it,x);
	return make_pair(HashIterator<Key,T,Compare,Hash>(bucket,res,this),true);
	
    
}
template <typename  Key, typename T, typename Compare,  typename Hash >
typename hashmap<Key,T,Compare,Hash>::iterator hashmap<Key,T,Compare,Hash>::insert(iterator position,const value_type & x)
{
	//忽略 position 
	return  insert(x).first;
}
//跨容器插入
template <typename  Key, typename T, typename Compare,  typename Hash >
template <typename InputIterator>
void hashmap<Key,T,Compare,Hash>::insert(InputIterator first,InputIterator last)
{
	insert_iterator<hashmap<Key,T,Compare,Hash> > insert_begin(*this,begin());
	//调用 insert() 方法
	copy(first,last,insert_begin);
	
}
template <typename  Key, typename T, typename Compare,  typename Hash >
void hashmap<Key,T,Compare,Hash>::insert(initializer_list<value_type> il)
{
	//调用 insert(InputIterator,InputIterator) 重载形式
	insert(il.begin(),il.end());
}

//c++11 emplace()
template <typename  Key, typename T, typename Compare,  typename Hash >
pair<typename hashmap<Key,T,Compare,Hash>::iterator,bool> hashmap<Key,T,Compare,Hash>::emplace(value_type && x)
{
	size_t bucket;
    auto it = findElement(x.first,bucket);
	if(it !=(*mElems)[bucket].end())
	{
		return make_pair(HashIterator<Key,T,Compare,Hash>(bucket,it,this),false);
	}
	++mSize;
	auto res =(*mElems)[bucket].emplace(x);
	return make_pair(HashIterator<Key,T,Compare,Hash>(bucket,res.first,this),res.second);
}
template <typename  Key, typename T, typename Compare,  typename Hash >
typename hashmap<Key,T,Compare,Hash>::iterator hashmap<Key,T,Compare,Hash>::emplace_hint(typename hashmap<Key,T,Compare,Hash>::iterator hint,value_type && x)
{
	//forward<T>(x)  当参数变量为右值引用时，则仍返回该右值引用类型， 若直接将 变量 传入到函数中，则会视为 左值
	//因此可以通过 该函数保持 右值引用的类型
	emplace(forward<value_type>(x)); //忽略 hint 参数
}
//元素删除
template <typename  Key, typename T, typename Compare,  typename Hash >
void hashmap<Key,T,Compare,Hash>::erase(typename hashmap<Key,T,Compare,Hash>::iterator position)
{
	((*mElems)[position.mBucket]).erase(position.mIt);
}
template <typename  Key, typename T, typename Compare,  typename Hash >
typename hashmap<Key,T,Compare,Hash>::size_type hashmap<Key,T,Compare,Hash>::erase(const key_type & x)
{
	size_t bucket;
    auto it = findElement(x,bucket);
	if(it == (*mElems)[bucket].end())
	{
		return 0;
	}
	mSize--;
	(*mElems)[bucket].erase(it);
	return 1;
	
}
template <typename  Key, typename T, typename Compare,  typename Hash >
void hashmap<Key,T,Compare,Hash>::erase(typename hashmap<Key,T,Compare,Hash>::iterator first,
			typename hashmap<Key,T,Compare,Hash>::iterator last)
			{
				/*
				(*mElems)[first.mBucket].erase(first.mIt,(*mElems)[first.mBucket].end());
				for(int i=first.mBucket+1;i<last.mBucket; ++i)
				{
					(*mElems)[i].clear();
				}
				for(auto it= (*mElems)[last.mBucket].begin(); it!=last.mIt;++it)
				{
					
				}
				*/
				typename hashmap<Key,T,Compare,Hash>::iterator cur, next;
				for(next=first; next!=last;)
				{
					//在删除前 定位到下个元素位置
					cur=next++;
					erase(cur);
				}
			}
template <typename  Key, typename T, typename Compare,  typename Hash >			
void hashmap<Key,T,Compare,Hash>::clear()
{
	for_each(mElems->begin(),mElems->end(),[](ListType& e){e.clear();});
	mSize=0;
}	
//stl 标准方法
template <typename  Key, typename T, typename Compare,  typename Hash >	
typename hashmap<Key,T,Compare,Hash>::key_compare hashmap<Key,T,Compare,Hash>::key_comp() const
{
	return mComp;
}
template <typename  Key, typename T, typename Compare,  typename Hash >	
typename hashmap<Key,T,Compare,Hash>::value_compare hashmap<Key,T,Compare,Hash>::value_comp() const
{
	return value_compare(mComp);
}
template <typename  Key, typename T, typename Compare,  typename Hash >	
typename hashmap<Key,T,Compare,Hash>::iterator hashmap<Key,T,Compare,Hash>::find(const key_type & x)
{
	size_t bucket;
    auto it = findElement(x,bucket);
	if(it == (*mElems)[bucket].end())
	{
		return end();
	}
   return  HashIterator<Key,T,Compare,Hash>(bucket,it,this);
        
}
template <typename  Key, typename T, typename Compare,  typename Hash >	
typename hashmap<Key,T,Compare,Hash>::const_iterator hashmap<Key,T,Compare,Hash>::find(const key_type & x) const
{
	return find(x);
}
template <typename  Key, typename T, typename Compare,  typename Hash >	
typename hashmap<Key,T,Compare,Hash>::size_type hashmap<Key,T,Compare,Hash>::count(const key_type & x) const
{
	/*
	size_t bucket;
    auto it = findElement(x,bucket);
	if(it == (*mElems)[bucket].end())
	{
		return 0;
	}
	size_t count =0;
	for_each((*mElems)[bucket].begin(),(*mElems)[bucket].end(),[&x,&count](ListType& e){if(e.first ==x){count++;}});
	return count;
	*/
	//不能插入相同元素 因此返回 0 或1
	if(find(x) ==end())
	{
		return 0;
	}
	else{return 1;}
}	
template <typename  Key, typename T, typename Compare,  typename Hash >
T & hashmap<Key,T,Compare,Hash>::operator[](const key_type & x)
{
	/*
	auto it=find(x);
	if(it==end())
	{
		it =insert(it,make_pair(x,T()));
	}
	return it->second;
	*/
	return ((insert(make_pair(x,T()))).first)->second;
}
#include "HashIterator.h"
#endif