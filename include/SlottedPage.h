#ifndef SLOTTEDPAGE_H
#define SLOTTEDPAGE_H

#include <iostream>
#include <memory>
#include <sstream>
#include <cstring>
#include "Tuple.h"
#include "Policy.h"
#include "LockManager.h"

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
    std::unique_ptr<char[]> page_data = std::make_unique<char[]>(PAGE_SIZE);
    size_t metadata_size = sizeof(Slot) * MAX_SLOTS;

    SlottedPage() {
        // Empty page -> initialize slot array inside page
        Slot* slot_array = reinterpret_cast<Slot*>(page_data.get());
        for (size_t slot_itr = 0; slot_itr < MAX_SLOTS; slot_itr++) {
            slot_array[slot_itr].empty = true;
            slot_array[slot_itr].offset = INVALID_VALUE;
            slot_array[slot_itr].length = INVALID_VALUE;
        }
    }; 

    bool addTuple(std::unique_ptr<Tuple> tuple, int pageId, LockManager &lock_manager);
    void deleteTuple(size_t index);
    void updateTuple(int pageId, size_t index, std::unique_ptr<Tuple> tuple, LockManager &lock_manager);
    void print() const;
};

#endif // SLOTTEDPAGE_H