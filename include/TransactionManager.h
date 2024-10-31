#include <unordered_map>
#include <cstdint>
#include <vector>

class TransactionManager {
    /// A way to hold the commited transactions along with the updates made in that transaction.
    std::unordered_map<int64_t, std::vector<int64_t>> commitedTransactions;
public:
    void addCommittedTransaction(int64_t commit_ts, std::vector<std::vector<int64_t>> tupleMetadata);
    std::unordered_map<int64_t, std::vector<int64_t>> getAllCommittedTransaction();
};