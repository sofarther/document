#!/bin/bash
#description 执行sql 语句

MYSQL=$(which mysql)
mysql_cmd="MYSQL remote -u root"
#echo $MYSQL
#-B 表示批处理模式， -s 禁止输出列标题和格式化符号
$MYSQL remote -u root -Bse 'show tables'

sql="select * from access_list  where access_id >666 limit 3;
	select * from access_list where access_id <10 limit 2 	
"
#-X 表示返回XML 格式数据
<< XML

<resultset statement="select * from access_list where access_id &lt;10 limit 2
" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <row>
        <field name="access_id">6</field>
        <field name="access_ip">127.0.0.1</field>
        <field name="access_userid">0</field>
        <field name="access_url">/index.php/Login/</field>
        <field name="access_time">1480304562</field>
        <field name="access_os">0</field>
        <field name="access_isproxy">0</field>
        <field name="access_domain">0</field>
  </row>

  <row>
        <field name="access_id">7</field>
        <field name="access_ip">127.0.0.1</field>
        <field name="access_userid">0</field>
        <field name="access_url">admine.juwairen.net/index.php/IPList/accessIPList</field>
        <field name="access_time">1480305266</field>
        <field name="access_os">0</field>
        <field name="access_isproxy">0</field>
        <field name="access_domain">0</field>
  </row>
</resultset>
XML

$MYSQL remote -u root -X -Bse "$sql"
#$mysql_cmd  -e "insert into access_list values(null,'182.168.1.101',100,'http://wd.net',158374833,0,0,0)"
#-u 和用户，-p和密码 之间必须没有空格 ，否则会进行交互式提示输入密码 
# mystest 指定连接的数据库名
$MYSQL -h 192.168.1.127 mystest -usofar -psofar -e "insert into admine values(null,'test','passwd','love email','qq','15334324','192.168.1.102','4212121',1)"
