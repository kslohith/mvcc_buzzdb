#include "Policy.h"

LruPolicy::LruPolicy(size_t cacheSize) : cacheSize(cacheSize) {}

bool LruPolicy::touch(PageID page_id) {
    bool found = false;

    if (map.find(page_id) != map.end()) {
        found = true;
        lruList.erase(map[page_id]);
        map.erase(page_id);
    }

    if (lruList.size() == cacheSize) {
        evict();
    }

    if (lruList.size() < cacheSize) {
        lruList.emplace_front(page_id);
        map[page_id] = lruList.begin();
    }

    return found;
}

PageID LruPolicy::evict() {
    PageID evictedPageId = INVALID_VALUE;
    
    if (!lruList.empty()) {
        evictedPageId = lruList.back();
        map.erase(evictedPageId);
        lruList.pop_back();
    }
    
    return evictedPageId;
}

void LruPolicy::printList(const std::string& list_name, const std::list<PageID>& myList) {
    std::cout << list_name << " :: ";
    
    for (const PageID& value : myList) {
        std::cout << value << ' ';
    }
    
    std::cout << '\n';
}