#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdexcept>
#include <string>
#include <vector>

class Path {
    private:
        std::vector<std::string> parts;
    public:
        Path(const std::string& path);

        const std::vector<std::string>& getParts() const;
        std::string getName() const;
        Path getParent() const;

        Path operator/(const Path& other) const;
        Path operator/(const std::string& part) const;
        bool operator==(const Path& other) const;
        bool operator!=(const Path& other) const;
};

class Item {
    private:
        std::string name;
    public:
        Item(std::string name);
        virtual ~Item() {}

        const std::string& getName() const;
        void setName(const std::string& name);

        virtual unsigned getSize() const = 0;
        virtual std::string toString() const = 0;
        virtual Item* copy() const = 0;
};

class File : public Item {
    private:
        std::string content;
    public:
        File(const std::string& name, std::string content = "");

        void tee(std::string content);
        const std::string& cat() const;

        unsigned getSize() const;
        std::string toString() const;
        Item* copy() const;
};

class Directory : public Item {
    private:
        Directory* parent = nullptr;
        std::vector<Item*> items;
        File* selectedFile = nullptr;
    public:
        Directory();
        Directory(const std::string& name);
        ~Directory();

        void add(Item* item);
        Directory* getParent() const;
        Path getPath() const;
        bool encloses(const Directory& other) const;
        Item* get(const std::string& path);
        Item* get(const Path& path);

        Directory* cd(const std::string& path);
        Directory* cd(const Path& path);
        std::vector<Path> ls();
        std::vector<Path> ls(const std::string& path);
        std::vector<Path> ls(const Path& path);
        Directory* touch(const std::string& path);
        Directory* touch(const Path& path);
        Directory* tee(const std::string& path, const std::string& content);
        Directory* tee(const Path& path, const std::string& content);
        Directory* tee(const std::string& content);
        std::string cat(const std::string& path);
        std::string cat(const Path& path);
        std::string cat();
        Directory* mkdir(const std::string& path);
        Directory* mkdir(const Path& path);
        Directory* mv(const std::string& src, const std::string& dest);
        Directory* mv(const Path& src, const Path& dest);
        Directory* cp(const std::string& src, const std::string& dest);
        Directory* cp(const Path& src, const Path& dest);
        Directory* rm(const std::string& path);
        Directory* rm(const Path& path);
        unsigned size(const std::string& path);
        unsigned size(const Path& path);

        unsigned getSize() const;
        std::string toString() const;
        Item* copy() const;
        void print(int level = -1) const;

        bool isFile(const Item* item) const;
        bool isDir(const Item* item) const;
        Directory* getRoot();
};

#endif
