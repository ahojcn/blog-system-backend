#include <cstdio>
#include <cstdlib>
#include <mysql/mysql.h>

// select

int main() {
  // init and get one tool
  MYSQL* connect_fd = mysql_init(NULL);

  // connect
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

  // char set = utf8
  mysql_set_character_set(connect_fd, "utf8");

  // get a sql query
  char sql[1024 * 4] = "select * from blog_table";

  // exec
  int ret = mysql_query(connect_fd, sql);
  if (ret < 0) {
    printf("error > %s\n", mysql_error(connect_fd));
    mysql_close(connect_fd);
    return 1;
  }

  // 遍历结果集合
  MYSQL_RES* result = mysql_store_result(connect_fd);
  //    获取结果集合中的行数和列数
  int rows = mysql_num_rows(result);
  int fields = mysql_num_fields(result);
  //    根据行和列遍历结果
  for (int i = 0; i < rows; ++i) {
    // 一次获取一行数据
    MYSQL_ROW row = mysql_fetch_row(result);
    for (int j = 0; j < fields; ++j) {
      printf("%s\t", row[j]);
    }
    printf("\n");
  }

  // 释放结果集合
  mysql_free_result(result);

  // close connect
  mysql_close(connect_fd);
  printf("connect closed.\n");
  return 0;
}
