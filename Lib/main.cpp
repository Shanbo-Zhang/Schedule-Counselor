#include <iostream>
#include "schcon/task.h"
#include "schcon/avl_tree.h"

int main() {
    g_TaskIDBitmap.Mark(300);
    std::cout<<g_TaskIDBitmap.HasMark(300);
}
