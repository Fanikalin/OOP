#include "FileSystem.h"
#include <iostream>

std::vector<std::string> split(std::string s, char sep = '/') {
    std::vector<std::string> parts;
    std::string current;

    for (int i = 0; i < s.length(); i++) {
        if (s[i] == sep) {
            parts.push_back(current);
            current.clear();
        } else {
            current += s[i];
        }
    }
    parts.push_back(current);
    return parts;
}

void checkName(std::string name) {
    if (name == "" || name == "." || name == "..") throw std::runtime_error("недопустимое имя");
    for (int i = 0; i < name.length(); i++) {
        if (name[i] == '/') throw std::runtime_error("недопустимое имя");
    }
}

Path::Path(const std::string& path) {
    if (path.length() > 0 && path[0] == '/') parts.push_back("/");
    for (std::string part : split(path)) {
        if (part != "") parts.push_back(part);
    }
}

const std::vector<std::string>& Path::getParts() const {
    return parts;
}

std::string Path::getName() const {
    if (parts.empty()) throw std::runtime_error("пустой путь");
    return parts.back();
}

Path Path::getParent() const {
    if (parts.empty()) throw std::runtime_error("нет родителя");
    Path result = *this;
    result.parts.pop_back();
    return result;
}

Path Path::operator/(const Path& other) const {
    if (!other.parts.empty() && other.parts[0] == "/") throw std::runtime_error("нельзя присоединить абсолютный путь");
    Path result = *this;
    for (std::string part : other.parts) {
        result.parts.push_back(part);
    }
    return result;
}

Path Path::operator/(const std::string& part) const {
    return *this / Path(part);
}

bool Path::operator==(const Path& other) const {
    return parts == other.parts;
}

bool Path::operator!=(const Path& other) const {
    return !(*this == other);
}

Item::Item(std::string name) {
    this->name = name;
}

const std::string& Item::getName() const {
    return name;
}

void Item::setName(const std::string& name) {
    checkName(name);
    this->name = name;
}

File::File(const std::string& name, std::string content) : Item(name) {
    checkName(name);
    this->content = content;
}

const std::string& File::cat() const {
    return content;
}

void File::tee(std::string content) {
    this->content = content;
}

unsigned File::getSize() const {
    return content.length();
}

std::string File::toString() const {
    return getName() + " (" + std::to_string(getSize()) + " B)";
}

Item* File::copy() const {
    return new File(getName(), content);
}

Directory::Directory(const std::string& name) : Item(name) {
    checkName(name);
}

Directory::Directory() : Item("") {
}

Directory::~Directory() {
    for (int i = 0; i < items.size(); i++) delete items[i];
}

void Directory::add(Item* item) {
    checkName(item->getName());
    if (get(item->getName()) != nullptr) throw std::runtime_error("уже есть элемент с таким именем");

    if (isDir(item)) {
        Directory* dir = dynamic_cast<Directory*>(item);
        if (dir->encloses(*this)) throw std::runtime_error("нельзя поместить каталог внутрь самого себя");
        dir->parent = this;
    }

    items.push_back(item);
}

Directory* Directory::getParent() const {
    return parent;
}

Path Directory::getPath() const {
    if (parent == nullptr) return Path("/");
    return parent->getPath() / getName();
}

bool Directory::encloses(const Directory& other) const {
    const Directory* dir = &other;
    while (dir != nullptr) {
        if (dir == this) return true;
        dir = dir->parent;
    }
    return false;
}

Item* Directory::get(const std::string& path) {
    return get(Path(path));
}

Item* Directory::get(const Path& path) {
    if (path.getParts().empty()) return this;

    std::string name = path.getName();
    if (name == "/" || name == "." || name == "..") return cd(path);

    Directory* dir = cd(path.getParent());
    for (int i = 0; i < dir->items.size(); i++) {
        if (dir->items[i]->getName() == name) return dir->items[i];
    }
    return nullptr;
}

Directory* Directory::cd(const std::string& path){
    return cd(Path(path));
}

Directory* Directory::cd(const Path& path){
    Directory* result = this;

    for (std::string part : path.getParts()) {
        if (part == "/") {
            result = getRoot();
        } else if (part == ".") {

        } else if (part == "..") {
            if (result->getParent() != nullptr) result = result->getParent();
        } else {
            Item* next = result->get(part);
            if (next == nullptr || !isDir(next)) throw std::runtime_error("такой директории не существует");
            result = dynamic_cast<Directory*>(next);
        }
    }

    return result;
}

std::vector<Path> Directory::ls() {
    std::vector<Path> result;

    for (int i = 0; i < items.size(); i++) result.push_back(getPath() / items[i]->getName());

    return result;
}

std::vector<Path> Directory::ls(const std::string& path) {
    return ls(Path(path));
}

std::vector<Path> Directory::ls(const Path& path) {
    Item* item = get(path);

    if (item == nullptr) throw std::runtime_error("директория или файл не найден");
    if (isDir(item)) return dynamic_cast<Directory*>(item)->ls();

    std::vector<Path> result;
    result.push_back(cd(path.getParent())->getPath() / item->getName());
    return result;
}

Directory* Directory::touch(const std::string& path) {
    return touch(Path(path));
}

Directory* Directory::touch(const Path& path) {
    Directory* dir = cd(path.getParent());
    std::string name = path.getName();

    if (dir->get(name) != nullptr) throw std::runtime_error("такой элемент уже существует");
    File* file = new File(name);
    dir->add(file);
    getRoot()->selectedFile = file;

    return this;
}

Directory* Directory::tee(const std::string& path, const std::string& content) {
    return tee(Path(path), content);
}

Directory* Directory::tee(const Path& path, const std::string& content) {
    Directory* dir = cd(path.getParent());
    std::string name = path.getName();
    Item* item = dir->get(name);

    if (item == nullptr) {
        File* file = new File(name);
        dir->add(file);
        getRoot()->selectedFile = file;
    } else if (dir->isFile(item)) {
        getRoot()->selectedFile = dynamic_cast<File*>(item);
    } else {
        throw std::runtime_error("это не файл");
    }

    return tee(content);
}

Directory* Directory::tee(const std::string& content) {
    File* file = getRoot()->selectedFile;
    if (file == nullptr) throw std::runtime_error("не выбран файл");
    file->tee(content);

    return this;
}

std::string Directory::cat(const std::string& path) {
    return cat(Path(path));
}

std::string Directory::cat(const Path& path) {
    Item* item = get(path);

    if (item == nullptr || !isFile(item)) throw std::runtime_error("файл не найден");
    getRoot()->selectedFile = dynamic_cast<File*>(item);

    return cat();
}

std::string Directory::cat() {
    File* file = getRoot()->selectedFile;
    if (file == nullptr) throw std::runtime_error("файл не найден");
    return file->cat();
}

Directory* Directory::mkdir(const std::string& path) {
    return mkdir(Path(path));
}

Directory* Directory::mkdir(const Path& path) {
    Directory* dir = cd(path.getParent());
    std::string name = path.getName();

    if (dir->get(name) != nullptr) throw std::runtime_error("такой элемент уже существует");
    Directory* result = new Directory(name);
    dir->add(result);

    return result;
}

Directory* Directory::mv(const std::string& src, const std::string& dest) {
    return mv(Path(src), Path(dest));
}

Directory* Directory::mv(const Path& src, const Path& dest) {
    Item* item = get(src);
    if (item == nullptr) throw std::runtime_error("элемент не найден");
    if (item == getRoot()) throw std::runtime_error("нельзя переместить корень");

    Directory* from;
    if (isDir(item)) from = dynamic_cast<Directory*>(item)->getParent();
    else from = cd(src.getParent());

    Directory* destDir;
    std::string newName = item->getName();
    Item* existing = get(dest);
    if (existing != nullptr && isDir(existing)) {
        destDir = dynamic_cast<Directory*>(existing);
    } else {
        destDir = cd(dest.getParent());
        newName = dest.getName();
    }

    if (destDir->get(newName) != nullptr) throw std::runtime_error("в директории назначения уже есть элемент с таким именем");
    if (isDir(item) && dynamic_cast<Directory*>(item)->encloses(*destDir)) {
        throw std::runtime_error("нельзя переместить каталог внутрь самого себя");
    }

    item->setName(newName);
    for (int i = 0; i < from->items.size(); i++) {
        if (from->items[i] == item) from->items.erase(from->items.begin() + i);
    }
    destDir->add(item);

    return this;
}

Directory* Directory::cp(const std::string& src, const std::string& dest) {
    return cp(Path(src), Path(dest));
}

Directory* Directory::cp(const Path& src, const Path& dest) {
    Item* item = get(src);
    if (item == nullptr) throw std::runtime_error("элемент не найден");
    if (item == getRoot()) throw std::runtime_error("нельзя скопировать корень");

    Directory* destDir;
    std::string newName = item->getName();
    Item* existing = get(dest);
    if (existing != nullptr && isDir(existing)) {
        destDir = dynamic_cast<Directory*>(existing);
    } else {
        destDir = cd(dest.getParent());
        newName = dest.getName();
    }

    if (destDir->get(newName) != nullptr) throw std::runtime_error("в директории назначения уже есть элемент с таким именем");

    Item* copy = item->copy();
    copy->setName(newName);
    destDir->add(copy);

    return this;
}

Directory* Directory::rm(const std::string& path) {
    return rm(Path(path));
}

Directory* Directory::rm(const Path& path) {
    Item* item = get(path);
    if (item == nullptr) throw std::runtime_error("элемент не найден");
    if (item == getRoot()) throw std::runtime_error("нельзя удалить корень");
    if (isDir(item) && dynamic_cast<Directory*>(item)->encloses(*this)) {
        throw std::runtime_error("нельзя удалить каталог, в котором находишься");
    }

    Directory* from;
    if (isDir(item)) from = dynamic_cast<Directory*>(item)->getParent();
    else from = cd(path.getParent());

    getRoot()->selectedFile = nullptr;
    for (int i = 0; i < from->items.size(); i++) {
        if (from->items[i] == item) from->items.erase(from->items.begin() + i);
    }
    delete item;

    return this;
}

unsigned Directory::size(const std::string& path) {
    return size(Path(path));
}

unsigned Directory::size(const Path& path) {
    Item* item = get(path);
    if (item == nullptr) throw std::runtime_error("элемент не найден");
    return item->getSize();
}

unsigned Directory::getSize() const {
    unsigned result = 0;
    for (int i = 0; i < items.size(); i++) result += items[i]->getSize();
    return result;
}

std::string Directory::toString() const {
    return getName() + "/ (" + std::to_string(getSize()) + " B)";
}

Item* Directory::copy() const {
    Directory* result = new Directory(getName());
    for (int i = 0; i < items.size(); i++) result->add(items[i]->copy());
    return result;
}

void Directory::print(int level) const {
    int depth = getPath().getParts().size() - 1;

    for (int i = 0; i < depth; i++) std::cout << "    ";
    std::cout << toString() << std::endl;

    if (level == 0) return;

    for (int i = 0; i < items.size(); i++) {
        if (isDir(items[i])) {
            dynamic_cast<Directory*>(items[i])->print(level - 1);
        } else {
            for (int j = 0; j < depth + 1; j++) std::cout << "    ";
            std::cout << items[i]->toString() << std::endl;
        }
    }
}

bool Directory::isFile(const Item* item) const {
    return dynamic_cast<const File*>(item) != nullptr;
}

bool Directory::isDir(const Item* item) const {
    return dynamic_cast<const Directory*>(item) != nullptr;
}

Directory* Directory::getRoot() {
    Directory* dir = this;
    while (dir->getParent() != nullptr) dir = dir->getParent();
    return dir;
}
