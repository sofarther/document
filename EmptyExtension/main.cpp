#include <phpcpp.h>
#include "test.h"
/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {
    
    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module() 
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("test", "1.0");
        
        // @todo    add your own functions, classes, namespaces to the extension
       	extension.add<my_func>("my_func");
	extension.add<out>("out");
        extension.add<notice>("notice");
        extension.add<out_params>("out_params",{
                Php::ByVal("a",Php::Type::Numeric),
                    Php::ByVal("b",Php::Type::String),
                    Php::ByVal("c",Php::Type::Array),
                    Php::ByVal("d",Php::Type::Object)
            }); 
        // return the extension
        return extension;
    }
}
