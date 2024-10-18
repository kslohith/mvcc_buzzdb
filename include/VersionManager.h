#include <unordered_map>
#include <memory>
#include "Tuple.h"

class VersionManager {
public:
    void addOrUpdateTuple(int key, std::vector<int64_t> tupleMetadata);

    std::vector<int64_t> getLatestVersion(int key);

    std::vector<std::vector<int64_t>> getAllLatestVersions();

private:
    std::unordered_map<int64_t, std::vector<int64_t>> latestVersions;
};