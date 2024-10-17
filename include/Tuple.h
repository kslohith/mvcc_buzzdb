#ifndef TUPLE_H
#define TUPLE_H

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Field.h"

class Tuple {
public:
    std::vector<std::unique_ptr<Field>> fields;
    /// Ts at which the tuple was created
    int64_t creation_ts;
    /// Ts till which the tuple is valid
    int64_t expiration_ts;
    /// Ts of the transaction that created this tuple
    int64_t tuple_id;
    /// PageNumber of the previous version of the tuple
    int64_t prev_page_number;
    /// slot number of the previous version of the tuple
    int64_t prev_slot_number;
    /// Check if this is the latest version of the tuple
    bool is_latest_version;
    /// Current pagenumber of the tuple
    int64_t page_number;
    /// Current slot number of the tuple
    int64_t slot_number;
    
  
    Tuple(int64_t tuple_id, int64_t creation_ts);

    void addField(std::unique_ptr<Field> field);
    size_t getSize() const;

    std::string serialize();
    void serialize(std::ofstream& out);

    static std::unique_ptr<Tuple> deserialize(std::istream& in);

    std::unique_ptr<Tuple> clone() const;
    void print() const;
};

#endif // TUPLE_H