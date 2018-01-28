#include "test.h"
#include <iostream>
#include <phpcpp.h>
#include <stdlib.h>
#include <typeinfo>
void my_func()
{
  Php::out << "this is test.so " << std::endl;
}
void out()
{
  Php::out <<"output"<<std::flush;
  Php::echo("Echo function\n");
}
void notice()
{
  Php::notice<<"this is a notice" << std::flush;
  Php::warning<<"this is a warning" << std::flush;
  Php::deprecated<<"this method is deprecated" << std::flush;
  Php::error<<"fatal error" << std::flush;
}

Php::Value out_params(Php::Parameters &params)
{
  Php::out<<"1 params: type:"<<  typeid(params[0]).name()
	  <<", value: " << params[0]
	  <<std::endl;
  Php::out<<"2 param: type:" << typeid(params[1]).name()
	  <<", value: " << params[1]<<std::flush;
  Php::out<<"3 param: type: " << typeid(params[2]).name()
	  <<", value: " << params[2] << std::flush;
  Php::out << "4 param: type: "<< typeid(params[3]).name()
	   <<", value: " << params[3]<<std::flush;

  Php::Value keys=Php::array_keys(params[2]);
  for(auto & key:keys)
    {
      Php::out << "key: " << key << std::endl;
    }
  Php::Object time("DateTime","now");
  Php::out << time.call("format","Y-m-d H:i:s") << std::endl;
  Php::Array time_format({time,"format"});
  Php::out <<time_format("Y-m-d k:i") << std::endl;
  if(params[0] >20){
    return 123;
  }else{
    return "abc";
}
}
