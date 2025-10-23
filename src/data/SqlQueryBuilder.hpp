#ifndef SQLQUERYBUILDER_HPP
#define SQLQUERYBUILDER_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <memory>

class SqlQueryBuilder {
public:
    enum class JoinType {
        INNER,
        LEFT,
        RIGHT
    };

    SqlQueryBuilder();
    
    // SELECT операции
    SqlQueryBuilder& select(const std::vector<std::string>& columns = {"*"});
    SqlQueryBuilder& from(const std::string& table);
    SqlQueryBuilder& where(const std::string& condition);
    SqlQueryBuilder& andWhere(const std::string& condition);
    SqlQueryBuilder& orWhere(const std::string& condition);
    SqlQueryBuilder& orderBy(const std::string& column, bool ascending = true);
    SqlQueryBuilder& limit(int count);
    SqlQueryBuilder& offset(int count);
    SqlQueryBuilder& groupBy(const std::string& column);
    
    // JOIN операции
    SqlQueryBuilder& join(JoinType type, const std::string& table, const std::string& condition);
    SqlQueryBuilder& innerJoin(const std::string& table, const std::string& condition);
    SqlQueryBuilder& leftJoin(const std::string& table, const std::string& condition);
    
    // INSERT операции
    SqlQueryBuilder& insertInto(const std::string& table);
    SqlQueryBuilder& values(const std::map<std::string, std::string>& values);
    
    // UPDATE операции
    SqlQueryBuilder& update(const std::string& table);
    SqlQueryBuilder& set(const std::map<std::string, std::string>& values);
    
    // DELETE операции
    SqlQueryBuilder& deleteFrom(const std::string& table);

    // Truncate операции
    SqlQueryBuilder& truncate(const std::string& table);
    SqlQueryBuilder& truncate(const std::vector<std::string>& tables);
    
    // Получение скомпилированного запроса
    std::string build() const;
    
    // Очистка билдера для нового запроса
    void reset();

private:
    std::string queryType_;
    std::vector<std::string> selectColumns_;
    std::string fromTable_;
    std::vector<std::string> whereConditions_;
    std::vector<std::string> joinClauses_;
    std::vector<std::string> orderByClauses_;
    std::vector<std::string> groupByClauses_;
    std::map<std::string, std::string> insertValues_;
    std::map<std::string, std::string> updateValues_;
    std::vector<std::string> truncateTables_;
    int limit_;
    int offset_;
    bool hasLimit_;
    bool hasOffset_;
    
    std::string buildSelect() const;
    std::string buildInsert() const;
    std::string buildUpdate() const;
    std::string buildDelete() const;
    std::string buildTruncate() const;
    
    static std::string joinTypeToString(JoinType type);
};

#endif // SQLQUERYBUILDER_HPP 