#include "VersionManager.h"

//To Do: Flushing VersionManager to disk
void VersionManager::addOrUpdateTuple(int key, std::vector<int64_t> tupleMetadata) {
    latestVersions[key] = tupleMetadata;
}

std::vector<int64_t> VersionManager::getLatestVersion(int key) {
    if(latestVersions.find(key) != latestVersions.end()) {
        std::vector<int64_t> tupleMetadata = latestVersions[key];
        return tupleMetadata;
    }
    return {};
}

std::vector<std::vector<int64_t>> VersionManager::getAllLatestVersions() {
    std::vector<std::vector<int64_t>> latestVersionsList;
    for(const auto& pair : latestVersions) {
        latestVersionsList.push_back(pair.second);
    }
    return latestVersionsList;
}