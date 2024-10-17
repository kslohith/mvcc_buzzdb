#ifndef BUZZDB_H
#define BUZZDB_H

#include <iostream>
#include <vector>
#include <memory>
#include "HashIndex.h"
#include "BufferManager.h"
#include "Operator.h"
#include "Transaction.h"

class BuzzDB {
public:
    HashIndex hash_index;
    BufferManager buffer_manager;

    size_t max_number_of_tuples = 5000;
    size_t tuple_insertion_attempt_counter = 0;

    BuzzDB();

    void insert(int key, int value, std::unique_ptr<Transaction>& t);
    void printTuples();
    void deleteTuples(int index);
    void executeQueries();
};

#endif // BUZZDB_H