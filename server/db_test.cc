#include "db.hpp"

// unit test ! ...  单元测试
// 测试db.hpp代码
void testBlogTable() {
  Json::StyledWriter writer;
  bool ret = 0;
  MYSQL *mysql = blog_system::MySQLInit();
  blog_system::BlogTable blog_table(mysql);

  // test insert
  Json::Value blog;
//  blog["title"] = "111";
//  blog["content"] = "30w offer";
//  blog["tag_id"] = 1;
//  blog["create_time"] = "2019/07/28"; 
//  bool ret = blog_table.Insert(blog);
//  printf("insert: %d\n", ret);
  
  // test selectall
  // Json::Value blogs;
  // bool ret = blog_table.SelectAll(&blogs);
  // printf("select all: %d\n", ret);

  // Json::StyledWriter writer;
  // printf("blogs: %s\n", writer.write(blogs).c_str());

  // test select one
//  bool ret = blog_table.SelectOne(1, &blog);
//  printf("select one: %d\n", ret);
//  printf("blog: %s\n", writer.write(blog).c_str());
//

  // update blog
  //
  //blog["blog_id"] = 1;
  //blog["title"] = "update blog test title";
  //blog["content"] = "1.1..1.1..1.1.1..1.1.1..12.2.2.2.";
  //ret = blog_table.Update(blog);
  //
  
  // test delete
  ret = blog_table.Delete(1);

  printf("ret: %d\n", ret);

  blog_system::MySQLRelease(mysql);
}

void testTagTable() {
  MYSQL *mysql = blog_system::MySQLInit();

  blog_system::TagTable tag_table(mysql);
  bool ret = 0;
  Json::Value tag;
  Json::StyledWriter writer;

  // test insert
  //tag["tag_name"] = "C/C++";
  //ret = tag_table.Insert(tag);

  // test select
  // ret = tag_table.SelectAll(&tag);
  // printf("tags: %s\n", writer.write(tag).c_str());

  ret = tag_table.Delete(1);
  printf("ret: %d\n", ret);

  blog_system::MySQLRelease(mysql);
}

int main() {
  // testBlogTable();
  //
  
  testTagTable();

  return 0;
}
