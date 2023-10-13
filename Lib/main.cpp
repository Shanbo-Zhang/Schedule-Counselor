#include <iostream>
#include "schcon/task.h"
#include "schcon/avl_tree.h"

int main() {
    AVLTree<int, int> tr;
    tr.Insert(4, 5);
    tr.Insert(6, 7);
    tr.Insert(8, 9);
    std::cout << tr.Count();
}
