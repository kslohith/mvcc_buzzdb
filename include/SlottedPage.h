#ifndef SLOTTEDPAGE_H
#define SLOTTEDPAGE_H

#include <iostream>
#include <memory>
#include <sstream>
#include <cstring>
#include "Tuple.h"
#include "Policy.h"
#include "VersionManager.h"

static constexpr size_t PAGE_SIZE = 4096;  // Fixed page size
static constexpr size_t MAX_SLOTS = 512;   // Fixed number of slots
//const uint16_t INVALID_VALUE = std::numeric_limits<uint16_t>::max(); // Sentinel value

struct Slot {
    bool empty = true;                 // Is the slot empty?    
    uint16_t offset = INVALID_VALUE;    // Offset of the slot within the page
    uint16_t length = INVALID_VALUE;    // Length of the slot
};

class SlottedPage {
public:
    std::unique_ptr<char[]> page_data;
    size_t metadata_size;
    int64_t current_page_id;
    VersionManager& version_manager;

    SlottedPage(int64_t page_id, VersionManager& version_manager);

    bool addTuple(std::unique_ptr<Tuple> tuple);
    void deleteTuple(size_t index);
    void updateTuple(size_t index, std::unique_ptr<Tuple> tuple);
    void print() const;
};

#endif // SLOTTEDPAGE_H