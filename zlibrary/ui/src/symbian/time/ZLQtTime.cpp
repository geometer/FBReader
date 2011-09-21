#include <QtCore/QDateTime>

#include <time.h>

#include <ZLTime.h>
#include "ZLQtTime.h"

ZLTime ZLQtTimeManager::currentTime() const {
    QDateTime dateTime = QDateTime::currentDateTime();
    return ZLTime( (long)dateTime.toTime_t(), (long)dateTime.time().msec() );
}


void ZLQtTimeManager::addTask(shared_ptr<ZLRunnable> task, int interval) {
        emit addTaskSignal(task,interval);
}


void ZLQtTimeManager::addTaskSlot(shared_ptr<ZLRunnable> task, int interval) {
    removeTask(task);
    if ((interval > 0) && !task.isNull()) {
            int id = startTimer(interval);
            myTimers[task] = id;
            myTasks[id] = task;
    }
}

void ZLQtTimeManager::removeTaskInternal(shared_ptr<ZLRunnable> task) {
        emit removeTaskInternalSignal(task);
}

void ZLQtTimeManager::removeTaskInternalSlot(shared_ptr<ZLRunnable> task) {
    std::map<shared_ptr<ZLRunnable>,int>::iterator it = myTimers.find(task);
    if (it != myTimers.end()) {
            killTimer(it->second);
            myTasks.erase(myTasks.find(it->second));
            myTimers.erase(it);
    }
}

void ZLQtTimeManager::timerEvent(QTimerEvent *event) {
	myTasks[event->timerId()]->run();
}


short ZLQtTimeManager::hoursBySeconds(long seconds) const {
	time_t seconds_time = seconds;
	return localtime(&seconds_time)->tm_hour;
}

short ZLQtTimeManager::minutesBySeconds(long seconds) const {
	time_t seconds_time = seconds;
	return localtime(&seconds_time)->tm_min;
}

short ZLQtTimeManager::yearBySeconds(long seconds) const {
	time_t seconds_time = seconds;
	return localtime(&seconds_time)->tm_year + 1900;
}

short ZLQtTimeManager::monthBySeconds(long seconds) const {
	time_t seconds_time = seconds;
	return localtime(&seconds_time)->tm_mon + 1;
}

short ZLQtTimeManager::dayOfMonthBySeconds(long seconds) const {
	time_t seconds_time = seconds;
	return localtime(&seconds_time)->tm_mday;
}
