#include "BookManager.hpp"
#include "database/Database.hpp"

NS_USE

BookManager* BookManager::create() {
    BookManager* book = new BookManager();
    return book;
}

bool BookManager::init() {
    return true;
}
