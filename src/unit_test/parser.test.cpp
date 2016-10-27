#include <iostream>
#include "parser/driver.h"
#include "parser/parser_context.h"

int main(int argc, char *argv[]) {

  const char *sql[] = {
      "SELECT name,country FROM Websites",
      "SELECT name,country FROM Websites WHERE country='CN'",
      "SELECT name,country FROM Websites WHERE country='CN' AND alexa > 50;",
      "SELECT name,country FROM Websites WHERE country='USA' OR country='CN';",
      "SELECT name,country FROM Websites WHERE alexa > 15 AND (country='CN' OR country='USA');",
      "SELECT name,country FROM Websites ORDER BY alexa;",
      "SELECT name,country FROM Websites ORDER BY country,alexa;",
      "INSERT INTO Websites (name, url, alexa, country) VALUES ('百度','https://www.baidu.com/','4','CN');",
      "INSERT INTO Websites (name, url, country) VALUES ('stackoverflow', 'http://stackoverflow.com/', 'IND');",
      "UPDATE Websites SET alexa='5000', country='USA' WHERE name='菜鸟教程';",
      "UPDATE Websites SET alexa='5000', country='USA'",
      "DELETE FROM Websites WHERE name='百度' AND country='CN';",
      "SELECT Websites.id, Websites.name, access_log.count, access_log.date FROM Websites INNER JOIN access_log ON Websites.id=access_log.site_id;",
      "SELECT Websites.name, access_log.count, access_log.date FROM Websites LEFT JOIN access_log ON Websites.id=access_log.site_id ORDER BY access_log.count DESC;",
      "SELECT Websites.name, access_log.count, access_log.date FROM access_log RIGHT JOIN Websites ON access_log.site_id=Websites.id ORDER BY access_log.count DESC;",
      "CREATE TABLE Person ( PersonID int,LastName varchar(255), FirstName varchar(255), Address varchar(255), City varchar(255));"
  };
  for (int i = 0; i < sizeof(sql) / sizeof(const char *); i++) {
    std::string sql_str = sql[i];
    ParserContext ctx;
    bool result = ctx.driver_.parse_string(ctx, sql_str, "input");
    if (result) {
      std::cout << "parse SQL:[" << sql[i] << "] success:" << std::endl;
      ctx.root_->trace(1);
    }
  }
}
