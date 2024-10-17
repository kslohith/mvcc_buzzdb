#ifndef OPERATORS_H
#define OPERATORS_H

#include <iostream>
#include <vector>
#include <memory>
#include "BufferManager.h"
#include "Field.h"
#include "Tuple.h"
#include "Predicate.h" // Assuming predicates are defined here

class Operator {
public:
    virtual ~Operator() = default;
    virtual void open() = 0;
    virtual bool next() = 0;
    virtual void close() = 0;
    virtual std::vector<std::unique_ptr<Field>> getOutput() = 0;
};

class UnaryOperator : public Operator {
protected:
    Operator* input;

public:
    explicit UnaryOperator(Operator& input);
    ~UnaryOperator() override = default;
};

class BinaryOperator : public Operator {
protected:
    Operator* input_left;
    Operator* input_right;

public:
    explicit BinaryOperator(Operator& input_left, Operator& input_right);
    ~BinaryOperator() override = default;
};

class ScanOperator : public Operator {
private:
    BufferManager& bufferManager;
    size_t currentPageIndex = 0;
    size_t currentSlotIndex = 0;
    std::unique_ptr<Tuple> currentTuple;
    size_t tuple_count = 0;

public:
    ScanOperator(BufferManager& manager);
    void open() override;
    bool next() override;
    void close() override;
    std::vector<std::unique_ptr<Field>> getOutput() override;
    std::unique_ptr<Tuple> getCurrentTuple();

private:
    void loadNextTuple();
};

class SelectOperator : public UnaryOperator {
private:
    std::unique_ptr<IPredicate> predicate;
    bool has_next;
    std::vector<std::unique_ptr<Field>> currentOutput;

public:
    SelectOperator(Operator& input, std::unique_ptr<IPredicate> predicate);
    void open() override;
    bool next() override;
    void close() override;
    std::vector<std::unique_ptr<Field>> getOutput() override;
};

enum class AggrFuncType { COUNT, MAX, MIN, SUM };

struct AggrFunc {
    AggrFuncType func;
    size_t attr_index; // Index of the attribute to aggregate
};

class HashAggregationOperator : public UnaryOperator {
private:
    std::vector<size_t> group_by_attrs;
    std::vector<AggrFunc> aggr_funcs;
    std::vector<Tuple> output_tuples; // Use your Tuple class for output
    size_t output_tuples_index = 0;

    struct FieldVectorHasher {
        std::size_t operator()(const std::vector<Field>& fields) const;
    };

public:
    HashAggregationOperator(Operator& input, std::vector<size_t> group_by_attrs, std::vector<AggrFunc> aggr_funcs);

    void open() override;
    bool next() override;
    void close() override;
    std::vector<std::unique_ptr<Field>> getOutput() override;

private:
    Field updateAggregate(const AggrFunc& aggrFunc, const Field& currentAggr, const Field& newValue);
};

class InsertOperator : public Operator {
private:
    BufferManager& bufferManager;
    std::unique_ptr<Tuple> tupleToInsert;

public:
    InsertOperator(BufferManager& manager);

    void setTupleToInsert(std::unique_ptr<Tuple> tuple);
    void open() override;
    bool next() override;
    void close() override;
    std::vector<std::unique_ptr<Field>> getOutput() override;
};

class DeleteOperator : public Operator {
private:
    BufferManager& bufferManager;
    size_t pageId;
    size_t tupleId;

public:
    DeleteOperator(BufferManager& manager, size_t pageId, size_t tupleId);

    void open() override;
    bool next() override;
    void close() override;
    std::vector<std::unique_ptr<Field>> getOutput() override;
};

#endif // OPERATORS_H