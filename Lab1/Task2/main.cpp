#include "History.h"
#include <iostream>

std::string ids(const std::vector<Version>& versions) {
    std::string result = "";
    for (int i = 0; i < versions.size(); i++) {
        if (i > 0) result += " ";
        result += std::to_string(versions[i].getId());
    }
    return result;
}

void print(History& history, int id = 0, int level = 0) {
    for (int i = 0; i < level; i++) std::cout << "    ";
    std::cout << "v" << id << ": " << history.find(id)->getText() << std::endl;

    for (Version child : history.getChildren(id)) print(history, child.getId(), level + 1);
}

void fill(History& history) {
    int v1 = history.add(0, "Введение");

    int v2 = history.add(v1, "Раздел 1");
    history.add(v2, "Исправлены опечатки");

    int v4 = history.add(v1, "Другая структура");
    history.add(v4, "Добавлены рисунки");

    int v6 = history.add(0, "Альтернативный черновик");
    history.add(v6, "Сокращённая версия");

    history.add(v2, "Раздел 2");
}

void testTree() {
    std::cout << "tree" << std::endl;
    Tree<int> root(1);
    Tree<int>* two = root.add(2);
    Tree<int>* three = root.add(3);
    Tree<int>* four = two->add(4);

    for (Tree<int>* node : root.getAllChildren()) std::cout << node->getData() << " ";
    std::cout << std::endl;
    for (Tree<int>* node : four->getPath()) std::cout << node->getData() << " ";
    std::cout << std::endl;
    std::cout << four->getCommonParent(three)->getData() << std::endl;
}

void testAdd() {
    std::cout << "add" << std::endl;
    History history("Начальный текст");
    fill(history);
    print(history);
}

void testFind() {
    std::cout << "find" << std::endl;
    History history("Начальный текст");
    fill(history);
    std::cout << history.find(0)->getText() << std::endl;
    std::cout << history.find(5)->getText() << std::endl;
    std::cout << history.find("Раздел 1")->getId() << std::endl;
}

void testGetChildren() {
    std::cout << "getChildren" << std::endl;
    History history("Начальный текст");
    fill(history);
    std::cout << ids(history.getChildren(0)) << std::endl;
    std::cout << ids(history.getChildren(2)) << std::endl;
}

void testGetAllChildren() {
    std::cout << "getAllChildren" << std::endl;
    History history("Начальный текст");
    fill(history);
    std::cout << ids(history.getAllChildren(1)) << std::endl;
    std::cout << ids(history.getAllChildren(6)) << std::endl;
}

void testGetPath() {
    std::cout << "getPath" << std::endl;
    History history("Начальный текст");
    fill(history);
    std::cout << ids(history.getPath(8)) << std::endl;
    std::cout << ids(history.getPath(7)) << std::endl;
}

void testGetCommonParent() {
    std::cout << "getCommonParent" << std::endl;
    History history("Начальный текст");
    fill(history);
    std::cout << history.getCommonParent(3, 8).getId() << std::endl;
    std::cout << history.getCommonParent(3, 5).getId() << std::endl;
    std::cout << history.getCommonParent(5, 7).getId() << std::endl;
}

void testRemove() {
    std::cout << "remove" << std::endl;
    History history("Начальный текст");
    fill(history);
    history.remove(7);
    history.remove(3);
    print(history);
}

int main() {
}
