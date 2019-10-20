#include <signal.h>

#include "httplib.h"
#include "db.hpp"

MYSQL *mysql = blog_system::MySQLInit();

int main() {
  using namespace httplib;
  using namespace blog_system;
  
  // 1. 先和数据库建立好连接
  signal(SIGINT, [](int ) {
      MySQLRelease(mysql);
      exit(0);
  });

  // 2. 创建相关数据库处理对象
  BlogTable blog_table(mysql);
  TagTable tag_table(mysql);

  // 3. 创建服务器, 创建并设置路由
  Server server;
  //// 新增博客
  server.Post("/blog", [&blog_table](const Request& req, Response& resp) {
    printf("新增博客\n");
    // 1. 获取请求中的 body 并解析成 json
    Json::Reader reader;
    Json::FastWriter writer;
    Json::Value req_json;
    Json::Value resp_json;
    bool ret = reader.parse(req.body, req_json);
    // 解析出错
    if (!ret) {
      printf("解析请求失败！%s\n", req.body.c_str());
      resp_json["ok"] = false;
      resp_json["msg"] = "input data parse failed!";
      resp.status = 400;
      resp.set_content(writer.write(resp_json), "application/json");
      return;
    }
    // 2. 对参数进行校验
    if (req_json["title"].empty()
        || req_json["content"].empty() 
        || req_json["tag_id"].empty()
        || req_json["create_time"].empty()) {
      printf("请求数据格式有错！%s\n", req.body.c_str());
      resp_json["ok"] = false;
      resp_json["msg"] = "input data format error!";
      resp.status = 400;
      resp.set_content(writer.write(resp_json), "application/json");
      return;
    }
    // 3. 进行操作
    ret = blog_table.Insert(req_json);
    if (!ret) {
      printf("插入博客失败！\n");
      resp_json["ok"] = false;
      resp_json["msg"] = "blog insert failed!";
      resp.status = 500;
      resp.set_content(writer.write(resp_json), "application/json");
      return;
    }
    // 4. 构造一个正确的相应
    printf("插入成功！\n");
    resp_json["ok"] = true;
    resp_json["msg"] = "insert ok!";
    resp.set_content(writer.write(resp_json), "application/json");
    return;
  });
  //// 查看所有博客列表
  server.Get("/blog", [&blog_table](const Request& req, Response& resp) {
    printf("查看所有博客\n");
    // 1. 尝试获取 tag_id, 如果不存在，返回空字符串
    const std::string& tag_id = req.get_param_value("tag_id");
    // 就不需要解析请求, 不需要合法性判定
    // 2. 调用数据库操作，获取所有博客结果
    Json::Value resp_json;
    Json::FastWriter writer;
    bool ret = blog_table.SelectAll(&resp_json, tag_id);
    if (!ret) {
      resp_json["ok"] = false;
      resp_json["msg"] = "select all faild";
      resp.status = 500;
      resp.set_content(writer.write(resp_json), "application/json");
      return;
    }
    // 3. 构造响应结果
    resp.set_content(writer.write(resp_json), "application/json");
    return;
  });
  //// 查看某个博客
  server.Get(R"(/blog/(\d+))", [&blog_table](const Request& req, Response& resp) {
    // 1. 获取 blog_id
    // printf("%s\n%s\n", req.matches[0].str().c_str(), req.matches[1].str().c_str());
    int32_t blog_id = std::stoi(req.matches[1]);
    printf("查看的博客id为%d\n", blog_id);
    Json::Value resp_json;
    Json::FastWriter writer;
    bool ret = blog_table.SelectOne(blog_id, &resp_json);
    if (!ret) {
      resp_json["ok"] = false;
      resp_json["msg"] = "查看指定博客失败: " + std::to_string(blog_id);
      resp.status = 404;
      resp.set_content(writer.write(resp_json), "application/json");
      return;
    }
    // 3. 执行正确的相应结果
    resp.set_content(writer.write(resp_json), "application/json");
    return;
  });
  //// 修改某博客
  server.Put(R"(/blog/(\d+))", [&blog_table](const Request& req, Response& resp) {
      int32_t blog_id = std::stoi(req.matches[1].str());
      printf("修改 id 为%d 的博客\n", blog_id);
      // 2. 获取请求并解析
      Json::Reader reader;
      Json::FastWriter writer;
      Json::Value req_json;
      Json::Value resp_json;
      bool ret = reader.parse(req.body, req_json);
      if (!ret) {
        resp_json["ok"] = false;
        resp_json["msg"] = "update blog parse request failed";
        resp.status = 400;
        resp.set_content(writer.write(resp_json), "application/json");
        return;
      }
      // 3. 校验参数是否符合预期
      if (req_json["title"].empty() 
          || req_json["content"].empty() 
          || req_json["tag_id"].empty()) {
        resp_json["ok"] = false;
        resp_json["msg"] = "update blog request format error";
        resp.status = 400;
        resp.set_content(writer.write(resp_json), "application/json");
        return;
      }
      // 4. 调用数据库操作来完成更新博客
      req_json["blog_id"] = blog_id;  // 从 path 中得到的 id 设置到 json 对象中

      printf("here 1\n");
      ret = blog_table.Update(req_json);
      if (!ret) {
        resp_json["ok"] = false;
        resp_json["msg"] = "update blog db failed";
        resp.status = 400;
        resp.set_content(writer.write(resp_json), "application/json");
        return;
      }
      // 5. 构造一个正确的返回结果
      resp_json["ok"] = true;
      resp.set_content(writer.write(resp_json), "application/json");
  });
  //// 删除博客
  server.Delete(R"(/blog/(\d+))", [&blog_table](const Request& req, Response& resp) {
      // 1. 获取 blog_id 
      int32_t blog_id = std::stoi(req.matches[1].str());
      printf("删除id为 %d的博客\n", blog_id);
      // 2. 调用数据库操作
      bool ret = blog_table.Delete(blog_id);
      Json::FastWriter writer;
      Json::Value resp_json;
      if (!ret) {
        resp_json["ok"] = false;
        resp_json["msg"] = "delete blog failed";
        resp.status = 500;
        resp.set_content(writer.write(resp_json), "application/json");
        return;
      }
      // 3. 构造一个正确的相应
      resp_json["ok"] = true;
      resp.set_content(writer.write(resp_json), "application/json");
      return;
  });

  //// 新增标签
  server.Post("/tag", [&tag_table](const Request& req, Response& resp) {
      // 1. 解析请求
      Json::FastWriter writer;
      Json::Reader reader;
      Json::Value req_json, resp_json;
      bool ret = reader.parse(req.body, req_json);
      if (!ret) {
        printf("插入标签失败\n");
        resp_json["ok"] = false;
        resp_json["msg"] = "insert tag failed, req parse faile";
        resp.status = 400;
        resp.set_content(writer.write(resp_json), "application/json");
        return;
      }
      // 2. 队请求进行校验
      if (req_json["tag_name"].empty()) {
        printf("插入标签失败\n");
        resp_json["ok"] = false;
        resp_json["msg"] = "insert tag req faild";
        resp.status = 400;
        resp.set_content(writer.write(resp_json), "application/json");
        return;
      }
      // 3. 调用数据库操作
      ret = tag_table.Insert(req_json);
      if (!ret) {
        printf("插入标签失败\n");
        resp_json["ok"] = false;
        resp_json["msg"] = "insert tag db faild";
        resp.status = 400;
        resp.set_content(writer.write(resp_json), "application/json");
        return;
      }
      // 4. 返回正确的结果
      resp_json["ok"] = true;
      resp.set_content(writer.write(resp_json), "application/json");
      return;
  });
  //// 删除标签
  server.Delete(R"(/tag/(\d+))", [&tag_table](const Request& req, Response& resp) {
      Json::FastWriter writer;
      Json::Reader reader;
      Json::Value req_json, resp_json;
      // 1. 解析 tag_id 
      int32_t tag_id = std::stoi(req.matches[1].str());
      printf("删除 id 为%d 的标签\n", tag_id);
      // 2. 执行数据库操作
      bool ret = tag_table.Delete(tag_id);
      if (!ret) {
        printf("删除失败\n");
        resp_json["ok"] = false;
        resp_json["msg"] = "insert tag db faild";
        resp.status = 400;
        resp.set_content(writer.write(resp_json), "application/json");
        return;
      }
      // 3. 构造正确的相应
      resp_json["ok"] = true;
      resp.set_content(writer.write(resp_json), "application/json");
      return;
  });
  //// 查看所有标签
  server.Get("/tag", [&tag_table](const Request& req, Response& resp) {
      Json::FastWriter writer;
      Json::Value resp_json;
      // 1. 不需要解析参数, 直接数据库操作
      bool ret = tag_table.SelectAll(&resp_json);
      if (!ret) {
        printf("获取所有标签失败\n");
        resp_json["ok"] = false;
        resp_json["msg"] = "get tag all faild";
        resp.status = 500;
        resp.set_content(writer.write(resp_json), "application/json");
        return;
      }
      // 3. 构造正确的相应
      resp.set_content(writer.write(resp_json), "application/json");
      return;
  });

  server.set_base_dir("./wwwroot");
  server.listen("0.0.0.0", 9093);

  return 0;
}
