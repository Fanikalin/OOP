#include "FileSystem.h"
#include <iostream>

void fill(Directory& root) {
    root.mkdir("/home")
        ->mkdir("docs")
            ->tee("report.txt", "чето")
            ->tee("notes.txt", "ещё чето")
            ->mkdir("old")
                ->tee("2023.txt", "2023");
    root.mkdir("/home/pics")
        ->touch("cat.png")->tee("картинка");
    root.mkdir("/tmp");
}

void testPath() {
    std::cout << "path" << std::endl;
    Path home("home");
    Path docs = home / "docs";
    Path report = docs / "report.txt";

    std::cout << (home == Path("home")) << std::endl;
    std::cout << (home / Path("docs/report.txt") == report) << std::endl;
    std::cout << (Path("/home") != Path("home")) << std::endl;
    std::cout << report.getName() << std::endl;
    std::cout << (report.getParent() == docs) << std::endl;
}

void testMkdir() {
    std::cout << "mkdir" << std::endl;
    Directory root;
    root.mkdir("/a")->mkdir("b")->mkdir("c");
    root.mkdir("/a/d");
    root.print();
}

void testTouch() {
    std::cout << "touch" << std::endl;
    Directory root;
    root.mkdir("/a")->touch("f");
    root.touch("/g");
    root.print();
}

void testTee() {
    std::cout << "tee" << std::endl;
    Directory root;
    fill(root);
    root.tee("/home/docs/report.txt", "изменено");
    std::cout << root.cat("/home/docs/report.txt") << std::endl;
    root.tee("/tmp/new.txt", "created");
    std::cout << root.cat("/tmp/new.txt") << std::endl;
    root.tee("changed");
    std::cout << root.cat("/tmp/new.txt") << std::endl;
}

void testCat() {
    std::cout << "cat" << std::endl;
    Directory root;
    fill(root);
    std::cout << root.cat("/home/docs/report.txt") << std::endl;
    std::cout << root.cat("/home/docs/old/2023.txt") << std::endl;
    std::cout << root.cat() << std::endl;
}

void testCd() {
    std::cout << "cd" << std::endl;
    Directory root;
    fill(root);
    std::cout << root.cd("/home/docs/old")->getName() << std::endl;
    std::cout << root.cd("./home/docs/../pics")->getName() << std::endl;
    std::cout << root.cd("/home/docs")->cd("/tmp")->getName() << std::endl;
}

void testLs() {
    std::cout << "ls" << std::endl;
    Directory root;
    fill(root);
    for (Path path : root.ls()) std::cout << path.getName() << " ";
    std::cout << std::endl;
    for (Path path : root.ls("/home/docs")) std::cout << path.getName() << " ";
    std::cout << std::endl;
}

void testGet() {
    std::cout << "get" << std::endl;
    Directory root;
    fill(root);
    std::cout << root.get("home")->toString() << std::endl;
    std::cout << root.get("/home/docs/old/2023.txt")->toString() << std::endl;
    std::cout << root.cd("/home")->get("docs/notes.txt")->toString() << std::endl;
}

void testSize() {
    std::cout << "size" << std::endl;
    Directory root;
    fill(root);
    std::cout << root.size("/home/docs/report.txt") << std::endl;
    std::cout << root.size("/home/docs") << std::endl;
    std::cout << root.size("/") << std::endl;
}

void testPrint() {
    std::cout << "print" << std::endl;
    Directory root;
    fill(root);
    root.print();
    root.print(1);
}

void testItem() {
    std::cout << "item" << std::endl;
    Directory root;
    fill(root);
    Item* items[] = {root.get("/home/docs"), root.get("/home/docs/notes.txt")};
    for (Item* item : items) std::cout << item->toString() << std::endl;
}

void testMv() {
    std::cout << "mv" << std::endl;
    Directory root;
    fill(root);
    root.mv("/home/docs/notes.txt", "/tmp");
    root.mv("/home/pics", "/tmp");
    root.mv("/tmp/notes.txt", "/tmp/todo.txt");
    root.mv("/home/docs/old", "/home/archive");
    root.print();
}

void testCp() {
    std::cout << "cp" << std::endl;
    Directory root;
    fill(root);
    root.cp("/home/docs/notes.txt", "/tmp");
    root.cp("/home/docs", "/tmp/backup");
    root.tee("/home/docs/report.txt", "Changed");
    root.print();
}

void testRm() {
    std::cout << "rm" << std::endl;
    Directory root;
    fill(root);
    root.rm("/home/docs/notes.txt");
    root.rm("/home/pics");
    root.print();
}

int main() {
    testPath();
    testMkdir();
    testTouch();
    testTee();
    testCat();
    testCd();
    testLs();
    testGet();
    testSize();
    testPrint();
    testItem();
    testMv();
    testCp();
    testRm();
    
}
