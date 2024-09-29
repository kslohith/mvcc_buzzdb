#ifndef FIELD_H
#define FIELD_H

#include <iostream>
#include <memory>
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>

enum FieldType { INT, FLOAT, STRING };

class Field {
public:
    FieldType type;
    size_t data_length;
    std::unique_ptr<char[]> data;

    Field(int i);
    Field(float f);
    Field(const std::string& s);
    Field(const Field& other);
    Field(Field&& other) noexcept;
    Field& operator=(const Field& other);
    ~Field() = default;

    FieldType getType() const;
    int asInt() const;
    float asFloat() const;
    std::string asString() const;

    std::string serialize();
    void serialize(std::ofstream& out);
    static std::unique_ptr<Field> deserialize(std::istream& in);

    std::unique_ptr<Field> clone() const;
    void print() const;

    bool operator==(const Field& other) const;
};

#endif // FIELD_H