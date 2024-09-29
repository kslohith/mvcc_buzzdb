#include "SlottedPage.h"

SlottedPage::SlottedPage() : page_data(std::make_unique<char[]>(PAGE_SIZE)), metadata_size(sizeof(Slot) * MAX_SLOTS) {
    Slot* slot_array = reinterpret_cast<Slot*>(page_data.get());
    for (size_t slot_itr = 0; slot_itr < MAX_SLOTS; slot_itr++) {
        slot_array[slot_itr].empty = true;
        slot_array[slot_itr].offset = INVALID_VALUE;
        slot_array[slot_itr].length = INVALID_VALUE;
    }
}

bool SlottedPage::addTuple(std::unique_ptr<Tuple> tuple, int pageId, TupleManager& tupleManager) {
    auto serializedTuple = tuple->serialize();
    size_t tuple_size = serializedTuple.size();

    size_t slot_itr = 0;
    Slot* slot_array = reinterpret_cast<Slot*>(page_data.get());
    for (; slot_itr < MAX_SLOTS; slot_itr++) {
        if (slot_array[slot_itr].empty == true && 
            slot_array[slot_itr].length >= tuple_size) {
            break;
        }
    }
    
    if (slot_itr == MAX_SLOTS) {
        return false;
    }

    slot_array[slot_itr].empty = false;
    size_t offset = INVALID_VALUE;
    
    if (slot_array[slot_itr].offset == INVALID_VALUE) {
        if (slot_itr != 0) {
            auto prev_slot_offset = slot_array[slot_itr - 1].offset;
            auto prev_slot_length = slot_array[slot_itr - 1].length;
            offset = prev_slot_offset + prev_slot_length;
        } else {
            offset = metadata_size;
        }
        slot_array[slot_itr].offset = offset;
    } else {
        offset = slot_array[slot_itr].offset;
    }

    if (offset + tuple_size >= PAGE_SIZE) {
        slot_array[slot_itr].empty = true;
        slot_array[slot_itr].offset = INVALID_VALUE;
        return false;
    }

    if (slot_array[slot_itr].length == INVALID_VALUE) {
        slot_array[slot_itr].length = tuple_size;
    }

    std::memcpy(page_data.get() + offset, serializedTuple.c_str(), tuple_size);
    
    tupleManager.addTuple(tuple->version_id, pageId, offset, tuple_size, tuple->begin_ts);

    return true;
}

void SlottedPage::deleteTuple(size_t index) {
    Slot* slot_array = reinterpret_cast<Slot*>(page_data.get());
    
    if (index < MAX_SLOTS && !slot_array[index].empty) {
        slot_array[index].empty = true;
        // Reset other fields if needed
        // For example:
        // slot_array[index].offset = INVALID_VALUE;
        // slot_array[index].length = INVALID_VALUE;
    }
}

void SlottedPage::updateTuple(size_t index, std::unique_ptr<Tuple> tuple) {
    deleteTuple(index);
    addTuple(std::move(tuple), index);
}

void SlottedPage::print() const {
    Slot* slot_array = reinterpret_cast<Slot*>(page_data.get());
    
    for (size_t slot_itr = 0; slot_itr < MAX_SLOTS; ++slot_itr) {
        if (!slot_array[slot_itr].empty) {
            const char* tuple_data = page_data.get() + slot_array[slot_itr].offset;
            std::istringstream iss(std::string(tuple_data, slot_array[slot_itr].length));
            auto loadedTuple = Tuple::deserialize(iss);
            
            std::cout << "Slot " << slot_itr << " : [";
            std::cout << static_cast<uint16_t>(slot_array[slot_itr].offset) << "] :: ";
            loadedTuple->print();
        }
    }
}