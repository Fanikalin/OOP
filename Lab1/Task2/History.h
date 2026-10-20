#ifndef HISTORY_H
#define HISTORY_H

#include "Tree.h"
#include <string>
#include <vector>

class Version {
    private:
        int id;
        std::string text;
    public:
        Version(int id, const std::string& text);

        int getId() const;
        const std::string& getText() const;
};

class History {
    private:
        Tree<Version> tree;
        int nextId;

        Tree<Version>* get(int id);
    public:
        History(const std::string& text);

        int add(int baseId, const std::string& text);
        const Version* find(int id);
        const Version* find(const std::string& text);
        std::vector<Version> getChildren(int id);
        std::vector<Version> getAllChildren(int id);
        std::vector<Version> getPath(int id);
        Version getCommonParent(int firstId, int secondId);
        void remove(int id);
};

#endif
