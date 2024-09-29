#include "TupleManager.h"

void TupleManager::addTuple(int key, int pageId, int offset, int tuple_size, int txnId) {
    if (tupleManager.find(key) == tupleManager.end()) {
        std::vector<int> pageMetadata = {pageId, offset, tuple_size};
        std::deque<std::vector<int>> pageQueue;
        pageQueue.push_back(pageMetadata);
        tupleManager[key] = pageQueue;
        std::cout << "Added tuple to tuple manager\n";
    } else {
        std::deque<std::vector<int>>& pageQueue = tupleManager[key];
        std::vector<int>& pageMetadata = pageQueue.back();
        int currentPageId = pageMetadata[0];
        int currentOffset = pageMetadata[1];
        int currentTupleSize = pageMetadata[2];

        auto& currentPage = bufferManager.getPage(currentPageId);
        char* page_buffer = currentPage->page_data.get();
        const char* tuple_data = page_buffer + currentOffset;
        std::istringstream iss(std::string(tuple_data, currentTupleSize));
        std::unique_ptr<Tuple> currentTuple = Tuple::deserialize(iss);
        currentTuple->end_ts = txnId;
        bufferManager.flushPage(currentPageId);

        pageQueue.push_back({pageId, offset, tuple_size});
        std::cout << "Added tuple to tuple manager\n";
    }
}

std::vector<std::unique_ptr<Field>> TupleManager::findTupleForReading(int key, int txnId) {
    if (tupleManager.find(key) == tupleManager.end()) {
        return {};
    }

    const std::deque<std::vector<int>>& pageQueue = tupleManager[key];
    for (auto it = pageQueue.rbegin(); it != pageQueue.rend(); ++it) {
        const std::vector<int>& pageMetadata = *it;
        int pageId = pageMetadata[0];
        int offset = pageMetadata[1];
        int tuple_size = pageMetadata[2];

        auto& currentPage = bufferManager.getPage(pageId);
        char* page_buffer = currentPage->page_data.get();
        const char* tuple_data = page_buffer + offset;
        std::istringstream iss(std::string(tuple_data, tuple_size));
        std::unique_ptr<Tuple> currentTuple = Tuple::deserialize(iss);

        if (currentTuple->begin_ts <= txnId && currentTuple->end_ts > txnId && currentTuple->read_ts <= txnId) {
            currentTuple->read_ts = txnId;
            return std::move(currentTuple->fields);
        }
    }

    return {};
}