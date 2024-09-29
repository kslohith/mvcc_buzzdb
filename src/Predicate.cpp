#include "Predicate.h"

void printTuple(const std::vector<std::unique_ptr<Field>>& tupleFields) {
    std::cout << "Tuple: [";
    for (const auto& field : tupleFields) {
        field->print(); // Assuming `print()` is a method that prints field content
        std::cout << " ";
    }
    std::cout << "]";
}

SimplePredicate::Operand::Operand(std::unique_ptr<Field> value)
    : directValue(std::move(value)), type(DIRECT) {}

SimplePredicate::Operand::Operand(size_t idx)
    : index(idx), type(INDIRECT) {}

SimplePredicate::SimplePredicate(Operand left, Operand right, ComparisonOperator op)
    : left_operand(std::move(left)), right_operand(std::move(right)), comparison_operator(op) {}

bool SimplePredicate::check(const std::vector<std::unique_ptr<Field>>& tupleFields) const {
    const Field* leftField = nullptr;
    const Field* rightField = nullptr;

    if (left_operand.type == DIRECT) {
        leftField = left_operand.directValue.get();
    } else if (left_operand.type == INDIRECT) {
        leftField = tupleFields[left_operand.index].get();
    }

    if (right_operand.type == DIRECT) {
        rightField = right_operand.directValue.get();
    } else if (right_operand.type == INDIRECT) {
        rightField = tupleFields[right_operand.index].get();
    }

    if (leftField == nullptr || rightField == nullptr) {
        std::cerr << "Error: Invalid field reference.\n";
        return false;
    }

    if (leftField->getType() != rightField->getType()) {
        std::cerr << "Error: Comparing fields of different types.\n";
        return false;
    }

    switch (leftField->getType()) {
        case FieldType::INT: {
            int left_val = leftField->asInt();
            int right_val = rightField->asInt();
            return compare(left_val, right_val);
        }
        case FieldType::FLOAT: {
            float left_val = leftField->asFloat();
            float right_val = rightField->asFloat();
            return compare(left_val, right_val);
        }
        case FieldType::STRING: {
            std::string left_val = leftField->asString();
            std::string right_val = rightField->asString();
            return compare(left_val, right_val);
        }
        default:
            std::cerr << "Invalid field type\n";
            return false;
    }
}

template<typename T>
bool SimplePredicate::compare(const T& left_val, const T& right_val) const {
    switch (comparison_operator) {
        case ComparisonOperator::EQ: return left_val == right_val;
        case ComparisonOperator::NE: return left_val != right_val;
        case ComparisonOperator::GT: return left_val > right_val;
        case ComparisonOperator::GE: return left_val >= right_val;
        case ComparisonOperator::LT: return left_val < right_val;
        case ComparisonOperator::LE: return left_val <= right_val;
        default: 
            std::cerr << "Invalid predicate type\n"; 
            return false;
    }
}

ComplexPredicate::ComplexPredicate(LogicOperator op)
    : logic_operator(op) {}

void ComplexPredicate::addPredicate(std::unique_ptr<IPredicate> predicate) {
    predicates.push_back(std::move(predicate));
}

bool ComplexPredicate::check(const std::vector<std::unique_ptr<Field>>& tupleFields) const {
    
    if (logic_operator == AND) {
        for (const auto& pred : predicates) {
            if (!pred->check(tupleFields)) {
                return false; // If any predicate fails, the AND condition fails
            }
        }
        return true; // All predicates passed
    } else if (logic_operator == OR) {
        for (const auto& pred : predicates) {
            if (pred->check(tupleFields)) {
                return true; // If any predicate passes, the OR condition passes
            }
        }
        return false; // No predicates passed
    }
    
    return false; // Default to false if no logic operator matches
}