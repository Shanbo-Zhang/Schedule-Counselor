#include <iostream>
#include "schcon/task.h"


enum class ScheduleFlag {
    HasBeginDate = 0x00000001,
    HasBeginTime = 0x00000002,
    HasDuration = 0x00000004
};

int main() {
    DeadlineTask t(L"ECE 110: Lab #7 Report", nullptr, Date(), Time(), Date(2023, 10, 9), Time(13,30,0),1800);
    std::cout << g_TaskIDBitmap.HasMark(t.ID());
}
