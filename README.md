# UDF_HTTP
use mysql udf for restful http request in windows
- http_get
- http_post
- http_put
- http_delete

## 目录
- [制作udf_http.dll文件](#1)
- [MySQL使用制作udf_http.dll文件](#2)
- [参考资料](#3)


## <span id = "1">制作udf_http.dll文件</span>
环境：VS2015+libcurl  
注意: 如MySQL是64位，需要制作64的dll。
### 1.下载编译好的libcurl
64位：
- .h: github中include文件夹
- .lib: github中lib文件夹
- .dll: github中bin文件夹

### 2.VS C++配置libcurl
#### 2.1. include .h文件
- 附加包含目录---》添加工程的头文件目录  
主要包括MySQL的头文件和libcurl的头文件  
![image](https://note.youdao.com/yws/public/resource/6f8422bd32256e4ba1b69c962e0a91b1/xmlnote/6A889F447F2649698120CC87413B84CD/10931)
```
#include <mysql.h>
#include <curl/curl.h>
```

#### 2.2. 添加预编译宏
- CURL_STATICLIB  
- HTTP_ONLY

![image](https://note.youdao.com/yws/public/resource/6f8422bd32256e4ba1b69c962e0a91b1/xmlnote/8C01A686152C468490BC070F04478ABE/10929)


#### 2.3. 配置lib
- 附加库目录---添加文件引用的lib静态库路径；
![image](https://note.youdao.com/yws/public/resource/6f8422bd32256e4ba1b69c962e0a91b1/xmlnote/087A084A94D5427D8BC9AC6BB36571B1/10932)
- 附加依赖项---添加工程引用的lib文件名；
![image](https://note.youdao.com/yws/public/resource/6f8422bd32256e4ba1b69c962e0a91b1/xmlnote/6092CDD6DC7341DFB2A1F2EBDC281D49/10930)
- Ws2_32.lib  
- Wldap32.lib  
- libcurl.lib    

### 3.代码
代码详见github，感谢(参考资料,1)中的大神。

## <span id = "2">MySQL使用制作udf_http.dll文件</span>
### 1. 引入dll
- libcurl.dll 放入...\MySQL\MySQL Server 5.7\bin
- 制作的udf_http.dll 放入...\MySQL\MySQL Server 5.7\lib\plugin  

### 2. MySQL中创建UDF函数
建立：
```
mysql> CREATE FUNCTION http_get RETURNS STRING SONAME "udf_http.dll";
mysql> CREATE FUNCTION http_post RETURNS STRING SONAME "udf_http.dll";
mysql> CREATE FUNCTION http_put RETURNS STRING SONAME "udf_http.dll";
mysql> CREATE FUNCTION http_delete RETURNS STRING SONAME "udf_http.dll";
```
删除：
```
mysql> drop function http_get;
mysql> drop function http_post;
mysql> drop function http_put;
mysql> drop function http_delete;
```

### 3. 触发器调用UDF函数

Navicat中定义的insert after触发器：
```
BEGIN
     SET @id = (select max(id) from tb_pipe_warning_info);
     SET @result = (select http_get(concat("http://localhost:59898/Home/UpdataWarningInfo/", @id)));
END
```
功能：当表中数据插入时，利用get请求向restful api接口发送最新插入的id。

## <span id = "3">参考资料</span>
1. https://github.com/y-ken/mysql-udf-http
2. http://blog.chinaunix.net/uid-233938-id-3920063.html  
3. https://bugs.mysql.com/bug.php?id=62449
4. http://blog.csdn.net/u012043391/article/details/54972127
5. http://blog.csdn.net/socho/article/details/52292064
6. http://blog.csdn.net/swotcoder/article/details/18524

### 注：集日月之精华，采网友之智慧
