#ifndef HASH_MAP_ITERATOR_H
#define HASH_MAP_ITERATOR_H
#include <iostream>
#include <iterator>
#include <vector>
#include <list>
#include <utility>

#include "DefaultHash.h"
using namespace std;


//继承 iterator 模板， 并部分具体化参数类型
template<typename Key,typename T, typename Compare, typename Hash>
    class HashIterator : public std::iterator<bidirectional_iterator_tag,
    pair<const Key,T> >
{
public:
    HashIterator();
    HashIterator(size_t bucket,typename list<pair<const Key,T> >::iterator listIt, const hashmap<Key,T,Compare,Hash> * inHashmap);

    pair<const Key,T>& operator*() const;
    pair<const Key,T>* operator->() const;
    //前缀++
    HashIterator<Key,T,Compare,Hash>& operator++();
    //后缀++
    const HashIterator<Key,T,Compare,Hash> operator++(int);
    HashIterator<Key,T,Compare,Hash>& operator--();
    const HashIterator<Key,T,Compare,Hash> operator--(int);
    bool operator==(const HashIterator& rhs) const;
    bool operator!=(const HashIterator& rhs) const;
protected:
    size_t mBucket;
    typename list<pair<const Key,T> >::iterator mIt;
    const hashmap<Key,T,Compare,Hash>* mHashmap;
    //辅助方法
    void increment();
    void decrement();
};
template<typename Key,typename T, typename Compare, typename Hash>
    HashIterator<Key,T,Compare,Hash>::HashIterator()
{
    mBucket=0;
    mHashmap=nullptr;
    mIt= list<pair<const Key, T> >::iterator();
}
template<typename Key,typename T, typename Compare, typename Hash>
    HashIterator<Key,T,Compare,Hash>::HashIterator(size_t bucket,typename list<pair<const Key,T> >::iterator listIt, const hashmap<Key,T,Compare,Hash> * inHashmap)
{
    mBucket=bucket;
    mIt=listIt;
    mHashmap=inHashmap;
}
template<typename Key,typename T, typename Compare, typename Hash>
    pair<const Key,T>& HashIterator<Key,T,Compare,Hash>::operator*() const
{
    return *mIt;
}
template<typename Key,typename T, typename Compare, typename Hash>
    pair<const Key,T>* HashIterator<Key,T,Compare,Hash>::operator->() const
{
    return &(*mIt);
}
template<typename Key,typename T, typename Compare, typename Hash>
    HashIterator<Key,T,Compare,Hash>& HashIterator<Key,T,Compare,Hash>::operator++()
{
    increment();
    return *this;
}
    //后缀++
template<typename Key,typename T, typename Compare, typename Hash>
    const HashIterator<Key,T,Compare,Hash> HashIterator<Key,T,Compare,Hash>::operator++(int)
{
    auto oldIt =*this;
    increment();
    return oldIt;
}
template<typename Key,typename T, typename Compare, typename Hash>
    HashIterator<Key,T,Compare,Hash>& HashIterator<Key,T,Compare,Hash>::operator--()
{
    decrement();
    return *this;
}
template<typename Key,typename T, typename Compare, typename Hash>
    const HashIterator<Key,T,Compare,Hash> HashIterator<Key,T,Compare,Hash>::operator--(int)
{
    auto oldIt =*this;
    decrement();
    return oldIt;
}
template<typename Key,typename T, typename Compare, typename Hash>
    bool HashIterator<Key,T,Compare,Hash>::operator==(const HashIterator& rhs) const
{
    if(mBucket !=rhs.mBucket)
    {
        return false;
    }
    return mIt ==rhs.mIt && mHashmap == rhs.mHashmap ;
}
template<typename Key,typename T, typename Compare, typename Hash>
    bool HashIterator<Key,T,Compare,Hash>::operator!=(const HashIterator& rhs) const
{
    /*
    if(mBucket !=rhs.mBucket)
    {
        return true;
    }
    return mIt != rhs.mIt;
    */
    return !(*this == rhs);
}

template<typename Key,typename T, typename Compare, typename Hash>
 void HashIterator<Key,T,Compare,Hash>::increment()
{
    ++mIt;
    //不支持 + 运算
    if(mIt == (*mHashmap->mElems)[mBucket].end())
    {

        size_t size =(*mHashmap->mElems).size();
        /*
        while(true)
        {
            
            if(mBucket+1==size)
            {
                mIt=(*mHashmap->mElems)[mBucket].end();
                return ;
            }
            ++mBucket;
            if((*mHashmap->mElems)[mBucket].empty())
            {
                continue;
            }
            mIt=(*mHashmap->mElems)[mBucket].begin();
            break;
        }
        */
        for(size_t i=mBucket+1; i<size; ++i)
        {
            if(!(*mHashmap->mElems)[i].empty())
            {
                mIt=(*mHashmap->mElems)[i].begin();
                mBucket=i;
                return;
            }
        }
        mBucket=(*mHashmap->mElems).size()-1;
        mIt=(*mHashmap->mElems)[mBucket].end();
    }
    
}
template<typename Key,typename T, typename Compare, typename Hash>
    void HashIterator<Key,T,Compare,Hash>::decrement()
    {
    if(mIt == (*mHashmap)[mBucket].begin())
    {
        /* 
        while(true)
        {
            --mBucket;
            if(mBucket==0)
            {
                mIt=(*mHashmap)[mBucket].begin();
                return ;
            }
        
            if((*mHashmap)[mBucket].empty())
            {
                continue;
            }
            mIt=--(*mHashmap)[mBucket].end();
            break;
        }
        */
        for(size_t i=mBucket-1;i>=0;--i)
        {
            if(!(*mHashmap->mElems)[i].empty())
            {
                mIt=(*mHashmap->mElems)[i].end();
                --mIt;
                mBucket=i;
                return ;
            }
        }
        mIt=(*mHashmap->mElems)[0].begin();
        --mIt;
        mBucket=0;
    }else{
        --mIt;
    }
        
    }

#endif