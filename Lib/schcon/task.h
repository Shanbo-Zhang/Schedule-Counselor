//
// Created by Escap on 9/28/2023.
//

#ifndef SCLIB_TASK_H
#define SCLIB_TASK_H

#include "base.h"
#include "date.h"
#include "time.h"
#include "calendar.h"
#include <cinttypes>
#include <utility>

//enum class TaskFlag {
//
//};

/*
 * Manifests what a certain task is.
 */
enum class TaskType : unsigned short {
    /**
     * For this kind of tasks, users can edit the begin date,
     * and the due date. It also contains a duration.
     */
    DeadlineTask = 0x0001,

    /**
     * This kind of tasks contains a list of begin time, end time, and duration.
     */
    ScheduledTask = 0x0002,

    /**
     * This kind of tasks is more complicated. It contains several modes.
     */
    PeriodicTask = 0x0004,
};

/**
 * Implementation of Bitmap.
 * For now, I used [unsigned short] to represent the id of tasks.
 * It contains 8 bytes. To ensure the id of every task is unique,
 * I created this to check if the id has existed.
 * When I'm writing this, I intend to use AVL to store all tasks.
 */
class {
public:
    void Mark(unsigned short id) noexcept {
        bitmap_[id >> 5] |= (0x1 << ((id << 27) >> 27));
    }

    void Unmark(unsigned short id) {
        bitmap_[id >> 5] ^= (0x1 << ((id << 27) >> 27));
    }

    bool HasMark(unsigned short id) const noexcept {
        return bitmap_[id >> 5] & (0x1 << ((id << 27) >> 27));
    }

private:
    UINT32 bitmap_[65536 / 32] = {0};
} g_TaskIDBitmap;

class AbstractTask {
public:
    AbstractTask() noexcept = delete; // To create a task, it must point out related information.

    /**
     * Creates a task instance based on specific type, name, and location.
     * The name cannot be empty.
     * @param type the type of the task, which cannot change once created.
     * @param name the name of the task, which cannot \b be \b empty.
     * @param location the location, can be empty.
     */
    AbstractTask(TaskType type, const Char *name, const Char *location) noexcept
            : type_(type), id_(RandomID()), name_(name), location_(location) {
        assert(!name_.IsEmpty()); // make sure the name is not empty.
    }

    virtual ~AbstractTask() noexcept {} // To ensure we cannot create it.

    TaskType Type() const noexcept {
        return type_;
    }

    unsigned short ID() const noexcept {
        return id_;
    }

    const String &Name() const noexcept {
        return name_;
    }

    const String &Location() const noexcept {
        return location_;
    }

protected:
    TaskType type_;
    unsigned short id_;
    String name_;
    String location_;
    // String location_;

    static unsigned short RandomID() noexcept {
        ::srand(::time(nullptr));
        unsigned short value = (::rand() << 16) >> 16;
        while (g_TaskIDBitmap.HasMark(value)) {
            value = (::rand() << 16) >> 16;
        }
        g_TaskIDBitmap.Mark(value);
        return value;
    }
};

class DeadlineTask : public AbstractTask {
public:
    enum class DeadlineFlag : unsigned int {
        ReleasedDate = 0x0001,
        ReleasedTime = 0x0002,
        Duration = 0x0004
    };

    /**
     * Creates a DeadlineTask instance from given information.
     * @param name the name of the task.
     * @param location the location of the task, can be empty.
     * @param release_date when the task is/was released, can be invalid.
     * @param release_time when the task is/was released, can be invalid.
     * @param deadline_date the date the task due, \b cannot be invalid.
     * @param deadline_time the time the task due, can be invalid (defaultly 23:59)
     * @param duration how long the task lasts, can be empty.
     * @warning \p deadline_date \b must be valid. Other parameters can be invalid.
     */
    DeadlineTask(const Char *name, const Char *location,
                 const Date &release_date, const Time &release_time,
                 const Date &deadline_date, const Time &deadline_time,
                 unsigned long duration)
            : AbstractTask(TaskType::DeadlineTask, name, location), flag_(),
              released_date_(release_date), released_time_(release_time),
              deadline_date_(deadline_date), deadline_time_(deadline_time),
              duration_(duration) {
        assert(deadline_date_.IsValid()); // How can a task with a certain deadline do not have a due date?
        if (!deadline_time_.IsValid()) { // if the time is invalid, it is default set to 23:59:59.
            deadline_time_.SetHour(23).SetMinute(59).SetSecond(59);
        }

        // Flags.
        if (released_date_.IsValid()) {
            flag_.AddFlag(DeadlineFlag::ReleasedDate);
        }
        if (released_time_.IsValid()) {
            flag_.AddFlag(DeadlineFlag::ReleasedTime);
        }
        if (duration_ > 0) {
            flag_.AddFlag(DeadlineFlag::Duration);
        }
    }

    DeadlineTask(const DeadlineTask &other) noexcept = default;

    ~DeadlineTask() noexcept override = default;

    Date &ReleasedDate() noexcept {
        return released_date_;
    }

    const Date &ConstReleasedDate() const noexcept {
        return released_date_;
    }

    DeadlineTask &SetReleasedDate(const int &year, const int &month, const int &day) {

    }

    const Time &ReleasedTime() const noexcept {
        return released_time_;
    }

    const Date &DeadlineDate() const noexcept {
        return deadline_date_;
    }

    const Time &DeadlineTime() const noexcept {
        return deadline_time_;
    }

    unsigned long Duration() const noexcept {
        return duration_;
    }


protected:
    Flag<DeadlineFlag> flag_;
    Date released_date_;
    Time released_time_;
    Date deadline_date_;
    Time deadline_time_;
    unsigned long duration_;
};

#endif //SCLIB_TASK_H
