#include "Operators.h"

// UnaryOperator implementation
UnaryOperator::UnaryOperator(Operator& input) : input(&input) {}

// BinaryOperator implementation
BinaryOperator::BinaryOperator(Operator& input_left, Operator& input_right)
        : input_left(&input_left), input_right(&input_right) {}

// ScanOperator implementation
ScanOperator::ScanOperator(BufferManager& manager, TupleManager& tupleManager)
        : bufferManager(manager), tupleManager(tupleManager) {}

void ScanOperator::open() {
    currentPageIndex = 0;
    currentSlotIndex = 0;
    currentTuple.reset();
    loadNextTuple();
}

bool ScanOperator::next() {
    if (!currentTuple) return false; // No more tuples available
    loadNextTuple();
    return currentTuple != nullptr;
}

void ScanOperator::close() {
    std::cout << "Scan Operator tuple_count: " << tuple_count << "\n";
    currentPageIndex = 0;
    currentSlotIndex = 0;
    currentTuple.reset();
}

std::vector<std::unique_ptr<Field>> ScanOperator::getOutput() {
    if (currentTuple) {
        return std::move(currentTuple->fields);
    }
    return {}; // Return an empty vector if no tuple is available
}

void ScanOperator::loadNextTuple() {
   while (currentPageIndex < bufferManager.getNumPages()) {
        auto& currentPage = bufferManager.getPage(currentPageIndex, tupleManager);
        if (!currentPage || currentSlotIndex >= MAX_SLOTS) {
            currentSlotIndex = 0; // Reset slot index when moving to a new page
            ++currentPageIndex;   // Increment page index after exhausting current page
            continue;             // Continue to the next page
        }
        
        char* page_buffer = currentPage->page_data.get();
        Slot* slot_array = reinterpret_cast<Slot*>(page_buffer);

        while (currentSlotIndex < MAX_SLOTS) {
            if (!slot_array[currentSlotIndex].empty) {
                const char* tuple_data = page_buffer + slot_array[currentSlotIndex].offset;
                std::istringstream iss(std::string(tuple_data, slot_array[currentSlotIndex].length));
                currentTuple = Tuple::deserialize(iss);
                ++currentSlotIndex; // Move to the next slot for the next call
                ++tuple_count;      // Increment tuple count
                return;             // Tuple loaded successfully
            }
            ++currentSlotIndex; // Move to the next slot if the current one is empty
        }
   }
   currentTuple.reset(); // No more tuples are available
}

// SelectOperator implementation
SelectOperator::SelectOperator(Operator& input, std::unique_ptr<IPredicate> predicate)
        : UnaryOperator(input), predicate(std::move(predicate)), has_next(false) {}

void SelectOperator::open() {
   input->open();
   has_next = false;
   currentOutput.clear(); // Ensure currentOutput is cleared at the beginning
}

bool SelectOperator::next() {
   while (input->next()) {
       const auto& output = input->getOutput(); // Temporarily hold the output
       if (predicate->check(output)) {          // If the predicate is satisfied, store the output in the member variable
           currentOutput.clear();               // Clear previous output

           for (const auto& field : output) {   // Assuming Field class has a clone method or copy constructor to duplicate fields
               currentOutput.push_back(field->clone());
           }
           has_next = true;
           return true;
       }
   }
   has_next = false;
   currentOutput.clear(); // Clear output if no more tuples satisfy the predicate
   return false;
}

void SelectOperator::close() {
   input->close();
   currentOutput.clear(); // Ensure currentOutput is cleared at the end
}

std::vector<std::unique_ptr<Field>> SelectOperator::getOutput() {
   if (has_next) { 
       std::vector<std::unique_ptr<Field>> outputCopy; 
       for (const auto& field : currentOutput) { 
           outputCopy.push_back(field->clone()); 
       }
       return outputCopy; 
   } else { 
       return {}; 
   } 
}

std::size_t HashAggregationOperator::FieldVectorHasher::operator()(const std::vector<Field>& fields) const {
    std::size_t hash = 0;
    for (const auto& field : fields) {
        std::hash<std::string> hasher;
        std::size_t fieldHash = 0;

        switch (field.getType()) {
            case FieldType::INT: {
                int value = field.asInt();
                fieldHash = hasher(std::to_string(value));
                break;
            }
            case FieldType::FLOAT: {
                float value = field.asFloat();
                fieldHash = hasher(std::to_string(value));
                break;
            }
            case FieldType::STRING: {
                std::string value = field.asString();
                fieldHash = hasher(value);
                break;
            }
            default:
                throw std::runtime_error("Unsupported field type for hashing.");
        }

        hash ^= fieldHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}

HashAggregationOperator::HashAggregationOperator(Operator& input, std::vector<size_t> group_by_attrs, std::vector<AggrFunc> aggr_funcs)
    : UnaryOperator(input), group_by_attrs(group_by_attrs), aggr_funcs(aggr_funcs) {}

void HashAggregationOperator::open() {
    input->open();
    output_tuples_index = 0;
    output_tuples.clear();

    std::unordered_map<std::vector<Field>, std::vector<Field>, FieldVectorHasher> hash_table;

    while (input->next()) {
        const auto& tuple = input->getOutput();

        std::vector<Field> group_keys;
        for (auto& index : group_by_attrs) {
            group_keys.push_back(*tuple[index]);
        }

        if (!hash_table.count(group_keys)) {
            std::vector<Field> aggr_values(aggr_funcs.size(), Field(0));
            hash_table[group_keys] = aggr_values;
        }

        auto& aggr_values = hash_table[group_keys];
        for (size_t i = 0; i < aggr_funcs.size(); ++i) {
            aggr_values[i] = updateAggregate(aggr_funcs[i], aggr_values[i], *tuple[aggr_funcs[i].attr_index]);
        }
    }

    for (const auto& entry : hash_table) {
        const auto& group_keys = entry.first;
        const auto& aggr_values = entry.second;

        Tuple output_tuple;
        for (const auto& key : group_keys) {
            output_tuple.addField(std::make_unique<Field>(key));
        }

        for (const auto& value : aggr_values) {
            output_tuple.addField(std::make_unique<Field>(value));
        }

        output_tuples.push_back(std::move(output_tuple));
    }
}

bool HashAggregationOperator::next() {
    if (output_tuples_index < output_tuples.size()) {
        ++output_tuples_index;
        return true;
    }
    return false;
}

void HashAggregationOperator::close() {
    input->close();
}

std::vector<std::unique_ptr<Field>> HashAggregationOperator::getOutput() {
    if (output_tuples_index == 0 || output_tuples_index > output_tuples.size()) {
        return {};
    }

    const auto& currentTuple = output_tuples[output_tuples_index - 1];
    std::vector<std::unique_ptr<Field>> outputCopy;

    for (const auto& field : currentTuple.fields) {
        outputCopy.push_back(field->clone());
    }

    return outputCopy;
}

Field HashAggregationOperator::updateAggregate(const AggrFunc& aggrFunc, const Field& currentAggr, const Field& newValue) {
    if (currentAggr.getType() != newValue.getType()) {
        throw std::runtime_error("Mismatched Field types in aggregation.");
    }

    switch (aggrFunc.func) {
        case AggrFuncType::COUNT: {
            if (currentAggr.getType() == FieldType::INT) {
                int count = currentAggr.asInt() + 1;
                return Field(count);
            }
            break;
        }
        case AggrFuncType::SUM: {
            if (currentAggr.getType() == FieldType::INT) {
                int sum = currentAggr.asInt() + newValue.asInt();
                return Field(sum);
            } else if (currentAggr.getType() == FieldType::FLOAT) {
                float sum = currentAggr.asFloat() + newValue.asFloat();
                return Field(sum);
            }
            break;
        }
        case AggrFuncType::MAX: {
            if (currentAggr.getType() == FieldType::INT) {
                int max = std::max(currentAggr.asInt(), newValue.asInt());
                return Field(max);
            } else if (currentAggr.getType() == FieldType::FLOAT) {
                float max = std::max(currentAggr.asFloat(), newValue.asFloat());
                return Field(max);
            }
            break;
        }
        case AggrFuncType::MIN: {
            if (currentAggr.getType() == FieldType::INT) {
                int min = std::min(currentAggr.asInt(), newValue.asInt());
                return Field(min);
            } else if (currentAggr.getType() == FieldType::FLOAT) {
                float min = std::min(currentAggr.asFloat(), newValue.asFloat());
                return Field(min);
            }
            break;
        }
        default:
            throw std::runtime_error("Unsupported aggregation function.");
    }

    throw std::runtime_error("Invalid operation or unsupported Field type.");
}

InsertOperator::InsertOperator(BufferManager& manager, TupleManager& tupleManager)
    : bufferManager(manager), tupleManager(tupleManager) {}

void InsertOperator::setTupleToInsert(std::unique_ptr<Tuple> tuple) {
    tupleToInsert = std::move(tuple);
}

void InsertOperator::open() {
    // Not used in this context
}

bool InsertOperator::next() {
    if (!tupleToInsert) return false; // No tuple to insert

    for (size_t pageId = 0; pageId < bufferManager.getNumPages(); ++pageId) {
        auto& page = bufferManager.getPage(pageId, tupleManager);
        // Attempt to insert the tuple
        if (page->addTuple(tupleToInsert->clone(), pageId)) { 
            // Flush the page to disk after insertion
            bufferManager.flushPage(pageId); 
            return true; // Insertion successful
        }
    }

    // If insertion failed in all existing pages, extend the database and try again
    bufferManager.extend();
    auto& newPage = bufferManager.getPage(bufferManager.getNumPages() - 1, tupleManager);
    if (newPage->addTuple(tupleToInsert->clone())) {
        bufferManager.flushPage(bufferManager.getNumPages() - 1);
        return true; // Insertion successful after extending the database
    }

    return false; // Insertion failed even after extending the database
}

void InsertOperator::close() {
    // Not used in this context
}

std::vector<std::unique_ptr<Field>> InsertOperator::getOutput() {
    return {}; // Return empty vector
}

DeleteOperator::DeleteOperator(BufferManager& manager, size_t pageId, size_t tupleId, TupleManager& tupleManager)
    : bufferManager(manager), pageId(pageId), tupleId(tupleId), tupleManager(tupleManager) {}

void DeleteOperator::open() {
    // Not used in this context
}

bool DeleteOperator::next() {
    auto& page = bufferManager.getPage(pageId, tupleManager);
    if (!page) {
        std::cerr << "Page not found." << std::endl;
        return false;
    }

    page->deleteTuple(tupleId); // Perform deletion
    bufferManager.flushPage(pageId); // Flush the page to disk after deletion
    return true;
}

void DeleteOperator::close() {
    // Not used in this context
}

std::vector<std::unique_ptr<Field>> DeleteOperator::getOutput() {
    return {}; // Return empty vector
}

UpdateOperator::UpdateOperator(BufferManager& manager, TupleManager& tupleManager)
    : bufferManager(manager), tupleManager(tupleManager) {}

void UpdateOperator::setTupleToUpdate(std::unique_ptr<Tuple> tuple) {
    tupleToUpdate = std::move(tuple);
}

void UpdateOperator::open() {
    // Not used in this context
}

bool UpdateOperator::next() {
    if (!tupleToUpdate) return false; // No tuple to update

    int currentPageIndex = 0;
    int currentSlotIndex = 0;

    while (currentPageIndex < bufferManager.getNumPages()) {
        auto& currentPage = bufferManager.getPage(currentPageIndex, tupleManager);
        if (!currentPage || currentSlotIndex >= MAX_SLOTS) {
            currentSlotIndex = 0; // Reset slot index when moving to a new page
        }

        char* page_buffer = currentPage->page_data.get();
        Slot* slot_array = reinterpret_cast<Slot*>(page_buffer);

        while (currentSlotIndex < MAX_SLOTS) {
            if (!slot_array[currentSlotIndex].empty) {
                const char* tuple_data = page_buffer + slot_array[currentSlotIndex].offset;
                std::istringstream iss(std::string(tuple_data, slot_array[currentSlotIndex].length));
                currentTuple = Tuple::deserialize(iss);

                if (currentTuple->fields[0]->asInt() == tupleToUpdate->fields[0]->asInt()) {
                    // Update the tuple
                    currentPage->deleteTuple(currentSlotIndex); // Delete the old tuple
                    currentPage->addTuple(tupleToUpdate->clone()); // Insert the updated tuple
                    bufferManager.flushPage(currentPageIndex); // Flush the page to disk after update
                    return true; // Update successful
                }
                currentSlotIndex++; // Move to the next slot for the next call
            }
            currentSlotIndex++;
        }

        // Increment page index after exhausting current page
        currentPageIndex++;
    }

    // Update failed
    return false;
}

void UpdateOperator::close() {
    // Not used in this context
}

std::vector<std::unique_ptr<Field>> UpdateOperator::getOutput() {
    return {}; // Return empty vector
}