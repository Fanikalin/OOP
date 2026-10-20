#ifndef TREE_H
#define TREE_H

#include <stdexcept>
#include <vector>

template <typename T>
class Tree {
    private:
        T data;
        Tree<T>* parent;
        std::vector<Tree<T>*> children;
    public:
        Tree(const T& data) : data(data), parent(nullptr) {}

        ~Tree() {
            for (int i = 0; i < children.size(); i++) delete children[i];
        }

        const T& getData() const {
            return data;
        }

        Tree<T>* getParent() const {
            return parent;
        }

        const std::vector<Tree<T>*>& getChildren() const {
            return children;
        }

        Tree<T>* add(const T& data) {
            Tree<T>* child = new Tree<T>(data);
            child->parent = this;
            children.push_back(child);
            return child;
        }

        void remove(Tree<T>* child) {
            for (int i = 0; i < children.size(); i++) {
                if (children[i] == child) {
                    if (!child->children.empty()) throw std::runtime_error("нельзя удалить узел, у которого есть потомки");
                    children.erase(children.begin() + i);
                    delete child;
                    return;
                }
            }
            throw std::runtime_error("узел не является потомком");
        }

        template <typename Func>
        Tree<T>* find(Func func) {
            if (func(data)) return this;

            for (int i = 0; i < children.size(); i++) {
                Tree<T>* result = children[i]->find(func);
                if (result != nullptr) return result;
            }
            return nullptr;
        }

        std::vector<Tree<T>*> getAllChildren() {
            std::vector<Tree<T>*> result;
            for (int i = 0; i < children.size(); i++) {
                result.push_back(children[i]);
                std::vector<Tree<T>*> next = children[i]->getAllChildren();
                for (int j = 0; j < next.size(); j++) result.push_back(next[j]);
            }
            return result;
        }

        std::vector<Tree<T>*> getPath() {
            std::vector<Tree<T>*> result;
            for (Tree<T>* node = this; node != nullptr; node = node->parent) {
                result.insert(result.begin(), node);
            }
            return result;
        }

        Tree<T>* getCommonParent(Tree<T>* other) {
            std::vector<Tree<T>*> first = getPath();
            std::vector<Tree<T>*> second = other->getPath();

            Tree<T>* result = nullptr;
            for (int i = 0; i < first.size() && i < second.size(); i++) {
                if (first[i] != second[i]) break;
                result = first[i];
            }
            return result;
        }
};

#endif
