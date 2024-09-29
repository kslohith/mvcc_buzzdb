#ifndef POLICY_H
#define POLICY_H

#include <unordered_map>
#include <list>
#include <iostream>
#include <string>
#include <limits>

using PageID = uint16_t;
constexpr PageID INVALID_VALUE = std::numeric_limits<PageID>::max();

class Policy {
public:
    virtual bool touch(PageID page_id) = 0;
    virtual PageID evict() = 0;
    virtual ~Policy() = default;
};

class LruPolicy : public Policy {
private:
    std::list<PageID> lruList;
    std::unordered_map<PageID, std::list<PageID>::iterator> map;
    size_t cacheSize;

public:
    LruPolicy(size_t cacheSize);

    bool touch(PageID page_id) override;
    PageID evict() override;

    static void printList(const std::string& list_name, const std::list<PageID>& myList);
};

#endif // POLICY_H