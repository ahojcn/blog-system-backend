////////////////////////////////////////
// 数据库相关操作的封装类
////////////////////////////////////////

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>

namespace blog_system {

  // 初始化一个 MySQL 句柄并建立连接
  static MYSQL* MySQLInit() {
    // 1. 创建一个句柄
    MYSQL *connect_fd = mysql_init(NULL);
     
    // 2. 建立连接
    if (mysql_real_connect(
          connect_fd, 
          "127.0.0.1", 
          "root", "123456", "blog_system", 
          3306, NULL, 0
        ) == NULL) {
      printf("connect sql server faild.\n%s\n", mysql_error(connect_fd));
      return NULL;
    }
    printf("connect success.\n");

    // 3. 设定字符编码
    mysql_set_character_set(connect_fd, "utf8");

    return connect_fd;
  }

  // 释放句柄并断开连接
  static void MySQLRelease(MYSQL *connect_fd) {
    mysql_close(connect_fd);
  }

  // 创建一个类，用于操作博客表的类
  class BlogTable {
  public:
    BlogTable(MYSQL *connect_fd) : _mysql(connect_fd){
    }

    bool Insert(const Json::Value& blog) {
      // 对正文部分进行转义，防止出现 SQL 注入
      const std::string &content = blog["content"].asString();
      // char *to = new char[content.size() * 2 + 1];  // 文档要求 to 的长度为 from.size * 2 + 1
      std::unique_ptr<char> to(new char[content.size() * 2 + 1]);
      
      mysql_real_escape_string(_mysql, to.get(), content.c_str(), content.size());

      std::unique_ptr<char> sql(new char[content.size() * 2 + 4096]);
      sprintf(sql.get()
          , "insert into blog_table values(null, '%s', '%s', %d, '%s')"
          , blog["title"].asCString()
          , to.get()
          , blog["tag_id"].asInt()
          , blog["create_time"].asCString()
      );

      int ret = mysql_query(_mysql, sql.get());
      if (ret != 0) {
        printf("insert error, %s\n", mysql_error(_mysql));
        return false;
      }

      return true;
    }

    bool SelectAll(Json::Value *blogs, const std::string &tag_id = "") {
      char sql[1024 * 4] = {0};

      if (tag_id == "") {
        sprintf(sql
            , "select blog_id, title, tag_id, create_time from blog_table");
      } else {
        sprintf(sql
            , "select blog_id, title, tag_id, create_time from blog_table where tag_id=%d"
            , std::stoi(tag_id));
      }

      int ret = mysql_query(_mysql, sql);
      if (ret != 0) {
        printf("select all blog error, %s\n", mysql_error(_mysql));
      }

      MYSQL_RES *result = mysql_store_result(_mysql);
      int rows = mysql_num_rows(result);
      for (int i = 0; i < rows; ++i) {
        MYSQL_ROW row = mysql_fetch_row(result);

        Json::Value blog;
        blog["blog_id"] = atoi(row[0]);
        blog["title"] = row[1];
        blog["tag_id"] = atoi(row[2]);
        blog["create_time"] = row[3];

        blogs->append(blog);
      }
      mysql_free_result(result);
      printf("select all success, 共查找到 %d 条记录\n", rows);

      return true;
    }

    bool SelectOne(int32_t blog_id, Json::Value *blog) {
      
      return true;
    }


    bool Update(const Json::Value &blog) {

      return true;
    }

    bool Delete(int32_t blog_id) {

      return true;
    }

  private:
    MYSQL *_mysql;
  };


  class TagTable {
  public:
    TagTable() {}

    bool Insert(const Json::Value &tag) {
      return true;
    }

    bool Delete(int32_t tag_id) {
      return true;
    }

    bool SelectAll(Json::Value *tags) {
      return true;
    }

  private:
  };

}  // end of namespace blog_system
