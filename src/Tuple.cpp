#include "Tuple.h"

Tuple::Tuple() = default;

Tuple::Tuple(int versionId) : version_id(versionId), begin_ts(versionId), end_ts(1000000000), read_ts(versionId) {}

void Tuple::addField(std::unique_ptr<Field> field) {
    fields.push_back(std::move(field));
}

size_t Tuple::getSize() const {
    size_t size = sizeof(version_id) + sizeof(begin_ts) + sizeof(end_ts) + sizeof(read_ts);
    for (const auto& field : fields) {
        size += field->data_length;
    }
    return size;
}

std::string Tuple::serialize() {
    std::stringstream buffer;
    buffer << fields.size() << ' ';
    buffer << version_id << ' ' << begin_ts << ' ' << end_ts << ' ' << read_ts << ' ';
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
    size_t fieldCount;
    
    in >> fieldCount >> tuple->version_id >> tuple->begin_ts >> tuple->end_ts >> tuple->read_ts;
    
    for (size_t i = 0; i < fieldCount; ++i) {
        tuple->addField(Field::deserialize(in));
    }
    
    return tuple;
}

std::unique_ptr<Tuple> Tuple::clone() const {
    auto clonedTuple = std::make_unique<Tuple>(version_id);
    
    for (const auto& field : fields) {
        clonedTuple->addField(field->clone());
    }
    
    return clonedTuple;
}

void Tuple::print() const {
    std::cout << "Version ID: " << version_id 
              << ", Begin TS: " << begin_ts 
              << ", End TS: " << end_ts 
              << ", Read TS: " << read_ts 
              << "\nFields: ";
              
    for (const auto& field : fields) {
        field->print();
        std::cout << " ";
    }
    
    std::cout << "\n";
}