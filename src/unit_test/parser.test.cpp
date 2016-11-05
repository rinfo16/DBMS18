#include <iostream>
#include "parser/driver.h"
#include "parser/parser_service_interface.h"
#include "parser/ast_base.h"

int main(int argc, char *argv[]) {

  const char *test_sql[] = {
      "SELECT name,country FROM Websites;",
      "SELECT name,country FROM Websites WHERE country='CN';",
      "CREATE TABLE Person ( PersonID integer,LastName varchar(255), FirstName varchar(255), Address varchar(255), City varchar(255));",
      "COPY TBL1 FROM '/tmp/data.csv';",
      "COPY TBL1(C1, C2, C3) FROM '/tmp/data.csv';",
      /*"SELECT name,country FROM Websites WHERE country='CN' AND alexa > 50;",
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
      "SELECT Websites.name, access_log.count, access_log.date FROM access_log RIGHT JOIN Websites ON access_log.site_id=Websites.id ORDER BY access_log.count DESC;"

    */
  };



  for (int i = 0; i < sizeof(test_sql) / sizeof(const char *); i++) {
    std::string sql = test_sql[i];
    std::cout << sql << std::endl;
    std::cout << "SQL:[" <<  sql << "]" << std::endl;
    parser::SQLParserInterface *parser = parser::ParserServiceInterface::Instance()->CreateSQLParser(sql);
    ASTBase *ast = parser->Parse();
    if (ast) {
      std::cout << "parse TREE : " << std::endl;
      std::cout << "---------------------------------------------" << std::endl;
      std::cout << ast->ToJSON() << std::endl;
      std::cout << "---------------------------------------------" << std::endl;
    }
  }
}
