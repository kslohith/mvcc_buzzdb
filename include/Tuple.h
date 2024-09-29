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
    int version_id;
    int begin_ts;
    int end_ts;
    int read_ts;

    Tuple();
    Tuple(int versionId);

    void addField(std::unique_ptr<Field> field);
    size_t getSize() const;

    std::string serialize();
    void serialize(std::ofstream& out);

    static std::unique_ptr<Tuple> deserialize(std::istream& in);

    std::unique_ptr<Tuple> clone() const;
    void print() const;
};

#endif // TUPLE_H