#ifndef PREDICATE_H
#define PREDICATE_H

#include <iostream>
#include <vector>
#include <memory>
#include "Field.h" // Assuming Field class is defined in this header

class IPredicate {
public:
    virtual ~IPredicate() = default;
    virtual bool check(const std::vector<std::unique_ptr<Field>>& tupleFields) const = 0;
};

void printTuple(const std::vector<std::unique_ptr<Field>>& tupleFields);

class SimplePredicate : public IPredicate {
public:
    enum OperandType { DIRECT, INDIRECT };
    enum ComparisonOperator { EQ, NE, GT, GE, LT, LE };

    struct Operand {
        std::unique_ptr<Field> directValue;
        size_t index;
        OperandType type;

        Operand(std::unique_ptr<Field> value);
        Operand(size_t idx);
    };

private:
    Operand left_operand;
    Operand right_operand;
    ComparisonOperator comparison_operator;

public:
    SimplePredicate(Operand left, Operand right, ComparisonOperator op);

    bool check(const std::vector<std::unique_ptr<Field>>& tupleFields) const override;

private:
    template<typename T>
    bool compare(const T& left_val, const T& right_val) const;
};

class ComplexPredicate : public IPredicate {
public:
    enum LogicOperator { AND, OR };

private:
    std::vector<std::unique_ptr<IPredicate>> predicates;
    LogicOperator logic_operator;

public:
    ComplexPredicate(LogicOperator op);

    void addPredicate(std::unique_ptr<IPredicate> predicate);
    bool check(const std::vector<std::unique_ptr<Field>>& tupleFields) const override;
};

#endif // PREDICATE_H