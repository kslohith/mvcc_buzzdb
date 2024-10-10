#include "QueryExecutor.h"

void QueryExecutor::prettyPrint(const QueryComponents& components) {
    std::cout << "Query Components:\n";
    std::cout << "  Selected Attributes: ";
    for (auto attr : components.selectAttributes) {
        std::cout << "{" << attr + 1 << "} "; // Convert back to 1-based indexing for display
    }
    std::cout << "\n  SUM Operation: " << (components.sumOperation ? "Yes" : "No");
    if (components.sumOperation) {
        std::cout << " on {" << components.sumAttributeIndex + 1 << "}";
    }
    std::cout << "\n  GROUP BY: " << (components.groupBy ? "Yes" : "No");
    if (components.groupBy) {
        std::cout << " on {" << components.groupByAttributeIndex + 1 << "}";
    }
    std::cout << "\n  WHERE Condition: " << (components.whereCondition ? "Yes" : "No");
    if (components.whereCondition) {
        std::cout << " on {" << components.whereAttributeIndex + 1 << "} > "
                  << components.lowerBound << " and < " << components.upperBound;
    }
    std::cout << std::endl;
}

void QueryExecutor::executeQuery(const QueryComponents& components, BufferManager& buffer_manager) { 
    // Stack allocation of ScanOperator
    ScanOperator scanOp(buffer_manager);

    // Using a pointer to Operator to handle polymorphism
    Operator* rootOp = &scanOp;

    // Buffer for optional operators to ensure lifetime
    std::optional<SelectOperator> selectOpBuffer;
    std::optional<HashAggregationOperator> hashAggOpBuffer;

    // Apply WHERE conditions
    if (components.whereAttributeIndex != -1) {
        // Create simple predicates with comparison operators
        auto predicate1 = std::make_unique<SimplePredicate>(
            SimplePredicate::Operand(components.whereAttributeIndex),
            SimplePredicate::Operand(std::make_unique<Field>(components.lowerBound)),
            SimplePredicate::ComparisonOperator::GT
        );

        auto predicate2 = std::make_unique<SimplePredicate>(
            SimplePredicate::Operand(components.whereAttributeIndex),
            SimplePredicate::Operand(std::make_unique<Field>(components.upperBound)),
            SimplePredicate::ComparisonOperator::LT
        );

        // Combine simple predicates into a complex predicate with logical AND operator
        auto complexPredicate = std::make_unique<ComplexPredicate>(ComplexPredicate::LogicOperator::AND);
        complexPredicate->addPredicate(std::move(predicate1));
        complexPredicate->addPredicate(std::move(predicate2));

        // Using std::optional to manage the lifetime of SelectOperator
        selectOpBuffer.emplace(*rootOp, std::move(complexPredicate));
        rootOp = &*selectOpBuffer;
    }

    // Apply SUM or GROUP BY operation
    if (components.sumOperation || components.groupBy) {
        std::vector<size_t> groupByAttrs;
        if (components.groupBy) {
            groupByAttrs.push_back(static_cast<size_t>(components.groupByAttributeIndex));
        }
        std::vector<AggrFunc> aggrFuncs{
            {AggrFuncType::SUM, static_cast<size_t>(components.sumAttributeIndex)}
        };

        // Using std::optional to manage the lifetime of HashAggregationOperator
        hashAggOpBuffer.emplace(*rootOp, groupByAttrs, aggrFuncs);
        rootOp = &*hashAggOpBuffer;
    }

    // Execute the Root Operator
    rootOp->open();
    while (rootOp->next()) {
        // Retrieve and print the current tuple
        const auto& output = rootOp->getOutput();
        for (const auto& field : output) {
            field->print();
            std::cout << " ";
        }
        std::cout << std::endl;
    }
    rootOp->close();
}