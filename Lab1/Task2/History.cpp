#include "History.h"

class HasId {
    private:
        int id;
    public:
        HasId(int id) {
            this->id = id;
        }

        bool operator()(const Version& version) const {
            return version.getId() == id;
        }
};

class HasText {
    private:
        std::string text;
    public:
        HasText(const std::string& text) {
            this->text = text;
        }

        bool operator()(const Version& version) const {
            return version.getText() == text;
        }
};

Version::Version(int id, const std::string& text) {
    this->id = id;
    this->text = text;
}

int Version::getId() const {
    return id;
}

const std::string& Version::getText() const {
    return text;
}

History::History(const std::string& text) : tree(Version(0, text)) {
    nextId = 1;
}

Tree<Version>* History::get(int id) {
    Tree<Version>* node = tree.find(HasId(id));
    if (node == nullptr) throw std::runtime_error("версия не найдена");
    return node;
}

std::vector<Version> toList(const std::vector<Tree<Version>*>& nodes) {
    std::vector<Version> result;
    for (Tree<Version>* node : nodes) result.push_back(node->getData());
    return result;
}

int History::add(int baseId, const std::string& text) {
    Tree<Version>* base = get(baseId);

    int id = nextId;
    base->add(Version(id, text));
    nextId++;

    return id;
}

const Version* History::find(int id) {
    Tree<Version>* node = tree.find(HasId(id));
    if (node == nullptr) return nullptr;
    return &node->getData();
}

const Version* History::find(const std::string& text) {
    Tree<Version>* node = tree.find(HasText(text));
    if (node == nullptr) return nullptr;
    return &node->getData();
}

std::vector<Version> History::getChildren(int id) {
    return toList(get(id)->getChildren());
}

std::vector<Version> History::getAllChildren(int id) {
    return toList(get(id)->getAllChildren());
}

std::vector<Version> History::getPath(int id) {
    return toList(get(id)->getPath());
}

Version History::getCommonParent(int firstId, int secondId) {
    return get(firstId)->getCommonParent(get(secondId))->getData();
}

void History::remove(int id) {
    Tree<Version>* node = get(id);
    if (node->getParent() == nullptr) throw std::runtime_error("нельзя удалить начальную версию");

    node->getParent()->remove(node);
}
