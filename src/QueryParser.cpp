#include "QueryParser.h"

QueryComponents parseQuery(const std::string& query) {
    QueryComponents components;

    // Parse selected attributes
    std::regex selectRegex("\\{(\\d+)\\}(, \\{(\\d+)\\})?");
    std::smatch selectMatches;
    std::string::const_iterator queryStart(query.cbegin());
    while (std::regex_search(queryStart, query.cend(), selectMatches, selectRegex)) {
        for (size_t i = 1; i < selectMatches.size(); i += 2) {
            if (!selectMatches[i].str().empty()) {
                components.selectAttributes.push_back(std::stoi(selectMatches[i]) - 1);
            }
        }
        queryStart = selectMatches.suffix().first;
    }

    // Check for SUM operation
    std::regex sumRegex("SUM\\{(\\d+)\\}");
    std::smatch sumMatches;
    if (std::regex_search(query, sumMatches, sumRegex)) {
        components.sumOperation = true;
        components.sumAttributeIndex = std::stoi(sumMatches[1]) - 1;
    }

    // Check for GROUP BY clause
    std::regex groupByRegex("GROUP BY \\{(\\d+)\\}");
    std::smatch groupByMatches;
    if (std::regex_search(query, groupByMatches, groupByRegex)) {
        components.groupBy = true;
        components.groupByAttributeIndex = std::stoi(groupByMatches[1]) - 1;
    }

    // Extract WHERE conditions more accurately
    std::regex whereRegex("\\{(\\d+)\\} > (\\d+) and \\{(\\d+)\\} < (\\d+)");
    std::smatch whereMatches;
    if (std::regex_search(query, whereMatches, whereRegex)) {
        components.whereCondition = true;
        // Correctly identify the attribute index for the WHERE condition
        components.whereAttributeIndex = std::stoi(whereMatches[1]) - 1;
        components.lowerBound = std::stoi(whereMatches[2]);
        // Ensure the same attribute is used for both conditions
        if (std::stoi(whereMatches[3]) - 1 == components.whereAttributeIndex) {
            components.upperBound = std::stoi(whereMatches[4]);
        } else {
            std::cerr << "Error: WHERE clause conditions apply to different attributes." << std::endl;
            // Handle error or set components.whereCondition = false;
        }
    }

    return components;
}