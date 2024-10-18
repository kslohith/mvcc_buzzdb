#include "BufferManager.h"

BufferManager::BufferManager(VersionManager& version_manager) : policy(std::make_unique<LruPolicy>(MAX_PAGES_IN_MEMORY)), storage_manager(version_manager) {}

std::unique_ptr<SlottedPage>& BufferManager::getPage(int page_id) {
    auto it = pageMap.find(page_id);
    if (it != pageMap.end()) {
        policy->touch(page_id);
        return it->second;
    }

    if (pageMap.size() >= MAX_PAGES_IN_MEMORY) {
        auto evictedPageId = policy->evict();
        if (evictedPageId != INVALID_VALUE) {
            std::cout << "Evicting page " << evictedPageId << "\n";
            storage_manager.flush(evictedPageId, pageMap[evictedPageId]);
            pageMap.erase(evictedPageId);
        }
    }

    auto page = storage_manager.load(page_id);
    policy->touch(page_id);
    std::cout << "Loading page: " << page_id << "\n";
    pageMap[page_id] = std::move(page);
    return pageMap[page_id];
}

void BufferManager::flushPage(int page_id) {
    storage_manager.flush(page_id, pageMap[page_id]);
}

void BufferManager::extend() {
    storage_manager.extend();
}

size_t BufferManager::getNumPages() {
    return storage_manager.num_pages;
}