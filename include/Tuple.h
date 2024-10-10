#ifndef TUPLE_H
#define TUPLE_H

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Field.h"
#include "Lock.h"

class Tuple {
public:
    std::vector<std::unique_ptr<Field>> fields;
    int pageNumber;
    int slotId;

    Tuple();

    void addField(std::unique_ptr<Field> field);
    size_t getSize() const;

    std::string serialize();
    void serialize(std::ofstream& out);

    static std::unique_ptr<Tuple> deserialize(std::istream& in);

    std::unique_ptr<Tuple> clone() const;
    void print() const;

};

#endif