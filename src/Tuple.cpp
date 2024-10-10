#include "Tuple.h"

Tuple::Tuple() = default;

void Tuple::addField(std::unique_ptr<Field> field) {
    fields.push_back(std::move(field));
}

size_t Tuple::getSize() const {
    size_t size = 0;
    size += sizeof(pageNumber);
    size += sizeof(slotId);
    for (const auto& field : fields) {
        size += field->data_length;
    }
    return size;
}

std::string Tuple::serialize() {
    std::stringstream buffer;
    buffer << pageNumber << ' ';
    buffer << slotId << ' ';
    buffer << fields.size() << ' ';
    for (const auto& field : fields) {
        buffer << field->serialize();
    }
    return buffer.str();
}

void Tuple::serialize(std::ofstream& out) {
    std::string serializedData = this->serialize();
    out << serializedData;
}

std::unique_ptr<Tuple> Tuple::deserialize(std::istream& in) {
    auto tuple = std::make_unique<Tuple>();
    int pageNumber, slotId;
    size_t fieldCount;
    in >> pageNumber >> slotId >> fieldCount;
    tuple->pageNumber = pageNumber;
    tuple->slotId = slotId;
    for (size_t i = 0; i < fieldCount; ++i) {
        tuple->addField(Field::deserialize(in));
    }
    return tuple;
}

std::unique_ptr<Tuple> Tuple::clone() const {
    auto clonedTuple = std::make_unique<Tuple>();
    for (const auto& field : fields) {
        clonedTuple->addField(field->clone());
    }
    return clonedTuple;
}

void Tuple::print() const {
              
    for (const auto& field : fields) {
        field->print();
        std::cout << " ";
    }
    
    std::cout << "\n";
}
