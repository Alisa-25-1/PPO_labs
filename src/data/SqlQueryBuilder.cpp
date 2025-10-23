#include "SqlQueryBuilder.hpp"
#include <algorithm>

SqlQueryBuilder::SqlQueryBuilder() 
    : limit_(0), offset_(0), hasLimit_(false), hasOffset_(false) {
}

SqlQueryBuilder& SqlQueryBuilder::select(const std::vector<std::string>& columns) {
    queryType_ = "SELECT";
    selectColumns_ = columns;
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::from(const std::string& table) {
    fromTable_ = table;
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::where(const std::string& condition) {
    whereConditions_.clear();
    whereConditions_.push_back(condition);
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::andWhere(const std::string& condition) {
    whereConditions_.push_back("AND " + condition);
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::orWhere(const std::string& condition) {
    whereConditions_.push_back("OR " + condition);
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::orderBy(const std::string& column, bool ascending) {
    orderByClauses_.push_back(column + (ascending ? " ASC" : " DESC"));
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::limit(int count) {
    limit_ = count;
    hasLimit_ = true;
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::offset(int count) {
    offset_ = count;
    hasOffset_ = true;
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::groupBy(const std::string& column) {
    groupByClauses_.push_back(column);
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::join(JoinType type, const std::string& table, const std::string& condition) {
    joinClauses_.push_back(joinTypeToString(type) + " JOIN " + table + " ON " + condition);
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::innerJoin(const std::string& table, const std::string& condition) {
    return join(JoinType::INNER, table, condition);
}

SqlQueryBuilder& SqlQueryBuilder::leftJoin(const std::string& table, const std::string& condition) {
    return join(JoinType::LEFT, table, condition);
}

SqlQueryBuilder& SqlQueryBuilder::insertInto(const std::string& table) {
    queryType_ = "INSERT";
    fromTable_ = table;
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::values(const std::map<std::string, std::string>& values) {
    insertValues_ = values;
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::update(const std::string& table) {
    queryType_ = "UPDATE";
    fromTable_ = table;
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::set(const std::map<std::string, std::string>& values) {
    updateValues_ = values;
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::deleteFrom(const std::string& table) {
    queryType_ = "DELETE";
    fromTable_ = table;
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::truncate(const std::string& table) {
    queryType_ = "TRUNCATE";
    truncateTables_.clear();
    truncateTables_.push_back(table);
    return *this;
}

SqlQueryBuilder& SqlQueryBuilder::truncate(const std::vector<std::string>& tables) {
    queryType_ = "TRUNCATE";
    truncateTables_ = tables;
    return *this;
}

std::string SqlQueryBuilder::buildTruncate() const {
    std::ostringstream query;
    query << "TRUNCATE TABLE ";
    
    for (size_t i = 0; i < truncateTables_.size(); ++i) {
        if (i > 0) query << ", ";
        query << truncateTables_[i];
    }
    
    query << " CASCADE";
    return query.str();
}


std::string SqlQueryBuilder::build() const {
    if (queryType_ == "SELECT") {
        return buildSelect();
    } else if (queryType_ == "INSERT") {
        return buildInsert();
    } else if (queryType_ == "UPDATE") {
        return buildUpdate();
    } else if (queryType_ == "DELETE") {
        return buildDelete();
    } else if (queryType_ == "TRUNCATE") {
        return buildTruncate();
    }
    return "";
}

std::string SqlQueryBuilder::buildSelect() const {
    std::ostringstream query;
    
    query << "SELECT ";
    if (selectColumns_.empty() || (selectColumns_.size() == 1 && selectColumns_[0] == "*")) {
        query << "*";
    } else {
        for (size_t i = 0; i < selectColumns_.size(); ++i) {
            if (i > 0) query << ", ";
            query << selectColumns_[i];
        }
    }
    
    if (!fromTable_.empty()) {
        query << " FROM " << fromTable_;
    }
    
    for (const auto& join : joinClauses_) {
        query << " " << join;
    }
    
    if (!whereConditions_.empty()) {
        query << " WHERE ";
        for (size_t i = 0; i < whereConditions_.size(); ++i) {
            if (i > 0) query << " ";
            query << whereConditions_[i];
        }
    }
    
    if (!groupByClauses_.empty()) {
        query << " GROUP BY ";
        for (size_t i = 0; i < groupByClauses_.size(); ++i) {
            if (i > 0) query << ", ";
            query << groupByClauses_[i];
        }
    }
    
    if (!orderByClauses_.empty()) {
        query << " ORDER BY ";
        for (size_t i = 0; i < orderByClauses_.size(); ++i) {
            if (i > 0) query << ", ";
            query << orderByClauses_[i];
        }
    }
    
    if (hasLimit_) {
        query << " LIMIT " << limit_;
    }
    
    if (hasOffset_) {
        query << " OFFSET " << offset_;
    }
    
    return query.str();
}

std::string SqlQueryBuilder::buildInsert() const {
    std::ostringstream query;
    
    query << "INSERT INTO " << fromTable_ << " (";
    
    size_t i = 0;
    for (const auto& pair : insertValues_) {
        if (i++ > 0) query << ", ";
        query << pair.first;
    }
    
    query << ") VALUES (";
    
    i = 0;
    for (const auto& pair : insertValues_) {
        if (i++ > 0) query << ", ";
        query << pair.second;
    }
    
    query << ")";
    
    return query.str();
}

std::string SqlQueryBuilder::buildUpdate() const {
    std::ostringstream query;
    
    query << "UPDATE " << fromTable_ << " SET ";
    
    size_t i = 0;
    for (const auto& pair : updateValues_) {
        if (i++ > 0) query << ", ";
        query << pair.first << " = " << pair.second;
    }
    
    if (!whereConditions_.empty()) {
        query << " WHERE ";
        for (size_t j = 0; j < whereConditions_.size(); ++j) {
            if (j > 0) query << " ";
            query << whereConditions_[j];
        }
    }
    
    return query.str();
}

std::string SqlQueryBuilder::buildDelete() const {
    std::ostringstream query;
    
    query << "DELETE FROM " << fromTable_;
    
    if (!whereConditions_.empty()) {
        query << " WHERE ";
        for (size_t i = 0; i < whereConditions_.size(); ++i) {
            if (i > 0) query << " ";
            query << whereConditions_[i];
        }
    }
    
    return query.str();
}

void SqlQueryBuilder::reset() {
    queryType_.clear();
    selectColumns_.clear();
    fromTable_.clear();
    whereConditions_.clear();
    joinClauses_.clear();
    orderByClauses_.clear();
    groupByClauses_.clear();
    insertValues_.clear();
    updateValues_.clear();
    limit_ = 0;
    offset_ = 0;
    hasLimit_ = false;
    hasOffset_ = false;
}

std::string SqlQueryBuilder::joinTypeToString(JoinType type) {
    switch (type) {
        case JoinType::INNER: return "INNER";
        case JoinType::LEFT: return "LEFT";
        case JoinType::RIGHT: return "RIGHT";
        default: return "INNER";
    }
}