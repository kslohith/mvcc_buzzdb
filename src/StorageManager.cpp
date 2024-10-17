#include "StorageManager.h"

StorageManager::StorageManager() {
    fileStream.open(DATABASE_FILENAME, std::ios::in | std::ios::out);
    if (!fileStream) {
        // If file does not exist, create it
        fileStream.clear(); // Reset the state
        fileStream.open(DATABASE_FILENAME, std::ios::out);
    }
    fileStream.close(); 
    fileStream.open(DATABASE_FILENAME, std::ios::in | std::ios::out); 

    fileStream.seekg(0, std::ios::end);
    num_pages = fileStream.tellg() / PAGE_SIZE;

    std::cout << "Storage Manager :: Num pages: " << num_pages << "\n";        
    if(num_pages == 0){
        extend();
    }
}

StorageManager::~StorageManager() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

std::unique_ptr<SlottedPage> StorageManager::load(PageID page_id) {
    fileStream.seekg(page_id * PAGE_SIZE, std::ios::beg);
    auto page = std::make_unique<SlottedPage>(page_id);
    
    // Read the content of the file into the page
    if(fileStream.read(page->page_data.get(), PAGE_SIZE)){
        //std::cout << "Page read successfully from file." << std::endl;
    } else {
        std::cerr << "Error: Unable to read data from the file. \n";
        exit(-1);
    }
    
    return page;
}

void StorageManager::flush(PageID page_id, const std::unique_ptr<SlottedPage>& page) {
    size_t page_offset = page_id * PAGE_SIZE;        

    // Move the write pointer
    fileStream.seekp(page_offset, std::ios::beg);
    fileStream.write(page->page_data.get(), PAGE_SIZE);        
    fileStream.flush();
}

void StorageManager::extend() {
    std::cout << "Extending database file \n";

    // Create a slotted page
    auto empty_slotted_page = std::make_unique<SlottedPage>(num_pages);

    // Move the write pointer
    fileStream.seekp(0, std::ios::end);

    // Write the page to the file, extending it
    fileStream.write(empty_slotted_page->page_data.get(), PAGE_SIZE);
    fileStream.flush();

    // Update number of pages
    num_pages += 1;
}