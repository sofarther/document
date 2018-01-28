<?php

my_func();
$arr=[ "name"=>"sofar","age"=>34,"gender"=>'male'];
$person= new stdClass();
$person->address="aakkkd";
$res=out_params(24,"hello",$arr,$person);
/*this is test.so 
 * 1 params: type:N3Php5ValueE, value: 24
 * 2 param: type:N3Php5ValueE, value: hellokey: 0,name
 * key: 1,age
 * key: 2,gender
 * 2018-01-28 19:08:47
 * 2018-01-28 k:08
 * int(123)
* /
var_dump($res);
?>
