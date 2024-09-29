#ifndef TUPLEMANAGER_H
#define TUPLEMANAGER_H

#include <unordered_map>
#include <deque>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include "BufferManager.h"
#include "Tuple.h"
#include "SlottedPage.h"

class BufferManager;

class TupleManager {
private:
    std::unordered_map<int, std::deque<std::vector<int>>> tupleManager;
    BufferManager& bufferManager;

public:
    TupleManager(BufferManager& bufferMgr) : bufferManager(bufferMgr) {}    
    void addTuple(int key, int pageId, int offset, int tuple_size, int txnId);
    std::vector<std::unique_ptr<Field>> findTupleForReading(int key, int txnId);
};

#endif // TUPLEMANAGER_H