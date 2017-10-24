#include <iostream>
#include <map>
#include <utility>
#include <string>

using namespace std;

class Data
{
public:
  Data(int val=0){mVal=val;}
  int getVal() const {return mVal;}
  void setVal(int val) {mVal=val;}
  friend ostream & operator<<(ostream & os, const Data & m)
  {
    os << m.getVal();
      return os;
    }
protected:
  int mVal;
};
template <typename T, typename V>
void showMap(const map<T,V> &);

void test(const map<string,Data> & ,string key);
int main()
{
  map<string,Data> dataMap;
  //insert
  pair<map<string,Data>::iterator,bool>ret =dataMap.insert(make_pair("sofar",Data(22)));

  cout <<(ret.first)->first<<", "<< (ret.first)->second << endl;
  cout <<ret.second << endl;
  ret = dataMap.insert(make_pair("zhss",Data(33)));
  //cout <<*(ret.first) << endl;
  cout <<(ret.first)->first<<", "<< (ret.first)->second << endl;

  cout <<ret.second << endl;
  ret =dataMap.insert(make_pair("sofar",Data(26)));
  cout <<(ret.first)->first<<", "<< (ret.first)->second << endl;

  //  cout <<*(ret.first) << endl;
  cout <<ret.second << endl;

  dataMap["zss"]=Data(45);
  showMap(dataMap);
  cout << dataMap["zhss"] << endl;
    test(dataMap,"zhss");
    dataMap["zss"]=Data(32);

    showMap(dataMap);
    //when key not exist, will call the  default construtor of value and
    //insert to map
    Data d=dataMap["zgg"];
    cout << d <<endl; //0
    showMap(dataMap);
	//find() 返回 iterator, 但iterator 可以转为const_iterator
	//但不能将 const_iterator 转为 iterator
	map<string,Data>::iterator ret2 = dataMap.find("zgg");
	dataMap.erase(ret.first, ret2);
	showMap(dataMap);

  return 0;
}

template <typename T, typename V>
void showMap(const map<T,V> & m)
{
  typename map<T,V>::const_iterator it;
  for( it=m.begin();it!=m.end(); ++it)
    {
      cout << it->first << ", " << it->second << endl;
    }
}
void test(const map<string,Data> &m ,string key)
{
  // operator[] not used const , so param m which is const, can not use []
  //cout <<m[key] << endl;
  map<string,Data>::const_iterator it =m.find(key);
  if(it !=m.end())
    {
      cout << it->second << endl;
    }

}
