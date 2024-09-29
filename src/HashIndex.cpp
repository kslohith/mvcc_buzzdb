#include "HashIndex.h"

HashIndex::HashIndex() {
    // Initialize all entries as non-existing by default
    for (size_t i = 0; i < capacity; ++i) {
        hashTable[i] = HashEntry();
    }
}

size_t HashIndex::hashFunction(int key) const {
    return key % capacity; // Simple modulo hash function
}

void HashIndex::insertOrUpdate(int key, int value) {
    size_t index = hashFunction(key);
    size_t originalIndex = index;
    bool inserted = false;
    int i = 0; // Attempt counter

    do {
        if (!hashTable[index].exists) {
            hashTable[index] = HashEntry(key, value, index);
            inserted = true;
            break;
        } else if (hashTable[index].key == key) {
            hashTable[index].value += value;
            inserted = true;
            break;
        }
        i++;
        index = (originalIndex + i * i) % capacity; // Quadratic probing
    } while (index != originalIndex && !inserted);

    if (!inserted) {
        std::cerr << "HashTable is full or cannot insert key: " << key << std::endl;
    }
}

int HashIndex::getValue(int key) const {
    size_t index = hashFunction(key);
    size_t originalIndex = index;

    do {
        if (hashTable[index].exists && hashTable[index].key == key) {
            return hashTable[index].value;
        }
        if (!hashTable[index].exists) {
            break; // Stop if we find a slot that has never been used
        }
        index = (index + 1) % capacity;
    } while (index != originalIndex);

    return -1; // Key not found
}

std::vector<int> HashIndex::rangeQuery(int lowerBound, int upperBound) const {
    std::vector<int> values;
    for (size_t i = 0; i < capacity; ++i) {
        if (hashTable[i].exists && hashTable[i].key >= lowerBound && hashTable[i].key <= upperBound) {
            std::cout << "Key: " << hashTable[i].key << ", Value: " << hashTable[i].value << std::endl;
            values.push_back(hashTable[i].value);
        }
    }
    return values;
}

void HashIndex::print() const {
    for (size_t i = 0; i < capacity; ++i) {
        if (hashTable[i].exists) {
            std::cout << "Position: " << hashTable[i].position 
                      << ", Key: " << hashTable[i].key 
                      << ", Value: " << hashTable[i].value << std::endl;
        }
    }
}