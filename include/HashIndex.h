#ifndef HASHINDEX_H
#define HASHINDEX_H

#include <iostream>
#include <vector>

class HashIndex {
private:
    struct HashEntry {
        int key;
        int value;
        int position; // Final position within the array
        bool exists;  // Flag to check if entry exists

        // Default constructor
        HashEntry() : key(0), value(0), position(-1), exists(false) {}

        // Constructor for initializing with key, value, and exists flag
        HashEntry(int k, int v, int pos) : key(k), value(v), position(pos), exists(true) {}
    };

    static const size_t capacity = 100; // Hard-coded capacity
    HashEntry hashTable[capacity];      // Static-sized array

    size_t hashFunction(int key) const;

public:
    HashIndex();

    void insertOrUpdate(int key, int value);
    int getValue(int key) const;
    std::vector<int> rangeQuery(int lowerBound, int upperBound) const;
    void print() const;
};

#endif // HASHINDEX_H