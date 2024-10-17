#include "Tuple.h"

/// Initialize the expiration_ts to INFINITY and prev version details to -1 during the creation of the tuple
Tuple::Tuple(int64_t tuple_id, int64_t creation_ts) : tuple_id(tuple_id), creation_ts(creation_ts) {
    expiration_ts = INFINITY;
    prev_page_number = -1;
    prev_slot_number = -1;
    page_number = -1;
    slot_number = -1;
    is_latest_version = false;
}

void Tuple::addField(std::unique_ptr<Field> field) {
    fields.push_back(std::move(field));
}

size_t Tuple::getSize() const {
    size_t size = 0;
    size += sizeof(int64_t) * 7; // creation_ts, expiration_ts, tuple_id, prev page id, prev slot id, page number, slot number
    size += sizeof(bool); // is_latest_version
    for (const auto& field : fields) {
        size += field->data_length;
    }
    return size;
}

std::string Tuple::serialize() {
    std::stringstream buffer;
    buffer << creation_ts << ' ' << expiration_ts << ' ' << tuple_id << ' ';
    buffer << prev_page_number << ' ' << prev_slot_number << ' ' << page_number << ' ' << slot_number << ' ';
    buffer << is_latest_version << ' ';
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
    size_t fieldCount;
    int64_t creation_ts, expiration_ts, tuple_id, prev_page_number, prev_slot_number, page_number, slot_number;
    bool is_latest_version;
    in >> creation_ts >> expiration_ts >> tuple_id >> prev_page_number >> prev_slot_number >> page_number >> slot_number >> is_latest_version;
    in >> fieldCount;
    auto tuple = std::make_unique<Tuple>(tuple_id, creation_ts);
    tuple->expiration_ts = expiration_ts;
    tuple->prev_page_number = prev_page_number;
    tuple->prev_slot_number = prev_slot_number;
    tuple->is_latest_version = is_latest_version;
    tuple->page_number = page_number;
    tuple->slot_number = slot_number;
    for (size_t i = 0; i < fieldCount; ++i) {
        tuple->addField(Field::deserialize(in));
    }
    return tuple;
}

std::unique_ptr<Tuple> Tuple::clone() const {
    auto clonedTuple = std::make_unique<Tuple>(tuple_id, creation_ts);
    clonedTuple->creation_ts = creation_ts;
    clonedTuple->expiration_ts = expiration_ts;
    clonedTuple->tuple_id = tuple_id;
    clonedTuple->prev_page_number = prev_page_number;
    clonedTuple->prev_slot_number = prev_slot_number;
    clonedTuple->is_latest_version = is_latest_version;
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