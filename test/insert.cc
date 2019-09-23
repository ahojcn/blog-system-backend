// 通过这个程序，使用 MySQL API 实现数据插入
//

#include <cstdio>
#include <cstdlib>
#include <mysql/mysql.h>  // 编译器默认从 /usr/include 中查找头文件


int main() {
  // 1. 创建一个数据库连接句柄
  MYSQL* connect_fd = mysql_init(NULL);

  // 2. 和数据库建立连接(和TCP区分开，这是在应用层建立连接)
  /*
   *MYSQL *mysql_real_connect(
   *  MYSQL *mysql,  // 句柄
   *  const char *host,
   *  const char *user, 
   *  const char *passwd, 
   *  const char *db,  // 数据库名 
   *  unsigned int port,
   *  const char *unix_socket,  // unix 套接字，NULL
   *  unsigned long client_flag  // 0
   *)
   */
  if (mysql_real_connect(
        connect_fd, 
        "127.0.0.1", 
        "root", "123456", "blog_system", 
        3306, NULL, 0
      ) == NULL) {
    printf("connect sql server faild.\n%s\n", mysql_error(connect_fd));
    return 1;
  }
  
  printf("connect success.\n");

  // 3. 设置编码方式
  //    mysql server 安装的时候设置了 utf8
  //    所以客户端也得设置成 utf8
  mysql_set_character_set(connect_fd, "utf8");

  // 4. 拼接 SQL 语句
  char sql[1024 * 4] = {0};
  char title[] = "first blog";
  char content[] = "30w+ offer";
  int tag_id = 1;
  char date[] = "2019/09/25";
  sprintf(sql, "insert into blog_table values(null, '%s', '%s', %d, '%s')", 
      title, content, tag_id, date);

  printf("sql > %s\n", sql);

  // 5. 执行 sql 语句
  int ret = mysql_query(connect_fd, sql);
  if (ret < 0) {
    printf("error > %s\n", mysql_error(connect_fd));
    mysql_close(connect_fd);
    return 1;
  }

  // 断开连接
  mysql_close(connect_fd);
  printf("connect closed.\n");
  
  return 0;
}
