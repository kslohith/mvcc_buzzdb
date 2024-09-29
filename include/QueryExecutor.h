#ifndef QUERYEXECUTOR_H
#define QUERYEXECUTOR_H

#include <iostream>
#include <vector>
#include <optional>
#include "QueryParser.h"
#include "BufferManager.h"
#include "Operator.h"

class QueryExecutor {
public:
    static void prettyPrint(const QueryComponents& components);
    static void executeQuery(const QueryComponents& components, BufferManager& buffer_manager);
};

#endif // QUERYEXECUTOR_H