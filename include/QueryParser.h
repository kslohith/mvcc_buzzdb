#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include <vector>
#include <string>
#include <regex>
#include <limits>
#include <iostream>

struct QueryComponents {
    std::vector<int> selectAttributes;
    bool sumOperation = false;
    int sumAttributeIndex = -1;
    bool groupBy = false;
    int groupByAttributeIndex = -1;
    bool whereCondition = false;
    int whereAttributeIndex = -1;
    int lowerBound = std::numeric_limits<int>::min();
    int upperBound = std::numeric_limits<int>::max();
};

QueryComponents parseQuery(const std::string& query);

#endif // QUERYPARSER_H