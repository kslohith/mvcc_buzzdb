#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <fstream>
#include <memory>
#include <iostream>
#include <limits>
#include "SlottedPage.h"

const std::string DATABASE_FILENAME = "buzzdb.dat";
using PageID = uint16_t;
//static constexpr size_t PAGE_SIZE = 4096;

class SlottedPage;
class TupleManager;
class StorageManager {
public:
    std::fstream fileStream;
    size_t num_pages = 0;

    StorageManager();
    ~StorageManager();

    std::unique_ptr<SlottedPage> load(PageID page_id);
    void flush(PageID page_id, const std::unique_ptr<SlottedPage>& page);
    void extend();
};

#endif // STORAGEMANAGER_H