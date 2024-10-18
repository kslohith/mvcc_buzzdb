#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <unordered_map>
#include <memory>
#include <iostream>
#include "StorageManager.h"
#include "SlottedPage.h"
#include "Policy.h"  // Assuming Policy and LruPolicy are defined here

constexpr size_t MAX_PAGES_IN_MEMORY = 10;

class BufferManager {
private:
    using PageMap = std::unordered_map<PageID, std::unique_ptr<SlottedPage>>;

    StorageManager storage_manager;
    PageMap pageMap;
    std::unique_ptr<Policy> policy;

public:
    BufferManager(VersionManager& version_manager);

    std::unique_ptr<SlottedPage>& getPage(int page_id);
    void flushPage(int page_id);
    void extend();
    size_t getNumPages();
};

#endif // BUFFERMANAGER_H