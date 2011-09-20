#ifndef __ZLQTTIME_H__
#define __ZLQTTIME_H__

#include <map>

#include <QtCore/QObject>
#include <QtCore/QTimerEvent>
#include <QtCore/QMetaType>

#include <ZLTimeManager.h>

class ZLQtTimeManager : public QObject, public ZLTimeManager {
    Q_OBJECT
public:

        static void createInstance() {

            ourInstance = new ZLQtTimeManager();

            qRegisterMetaType< shared_ptr<ZLRunnable> >("shared_ptr<ZLRunnable>");
            connect((ZLQtTimeManager*)ourInstance, SIGNAL(removeTaskInternalSignal(shared_ptr<ZLRunnable>)),
                    (ZLQtTimeManager*)ourInstance, SLOT(removeTaskInternalSlot(shared_ptr<ZLRunnable>)));

            connect((ZLQtTimeManager*)ourInstance, SIGNAL(addTaskSignal(shared_ptr<ZLRunnable>,int)),
                    (ZLQtTimeManager*)ourInstance, SLOT(addTaskSlot(shared_ptr<ZLRunnable>, int)));

        }

	void addTask(shared_ptr<ZLRunnable> task, int interval);
	void removeTaskInternal(shared_ptr<ZLRunnable> task);

private:
		ZLTime currentTime() const;
		short hoursBySeconds(long seconds) const;
		short minutesBySeconds(long seconds) const;

		short yearBySeconds(long seconds) const;
		short monthBySeconds(long seconds) const;
		short dayOfMonthBySeconds(long seconds) const;

signals:
        void addTaskSignal(shared_ptr<ZLRunnable> task, int interval);
        void removeTaskInternalSignal(shared_ptr<ZLRunnable> task);
private slots:
        void addTaskSlot(shared_ptr<ZLRunnable> task, int interval);
        void removeTaskInternalSlot(shared_ptr<ZLRunnable> task);

private:
	void timerEvent(QTimerEvent *event);

private:
	std::map<shared_ptr<ZLRunnable>,int> myTimers;
	std::map<int,shared_ptr<ZLRunnable> > myTasks;
};

#endif /* __ZLQTTIME_H__ */
