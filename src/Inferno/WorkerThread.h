#pragma once

#include <thread>
#include <mutex>
#include <spdlog/spdlog.h>

// A long running worker thread that waits for notifications to start processing
class WorkerThread {
    std::mutex _notifyLock;
    std::condition_variable _workAvailable;
    std::thread _worker;
    std::atomic<bool> _hasWork;
    std::atomic<bool> _alive;
public:
    WorkerThread() {}

    virtual ~WorkerThread() {
        Stop();
    }

    void Start() {
        _alive = true;
        _worker = std::thread(&WorkerThread::Worker, this);
    }

    void Stop() {
        _alive = false;
        _workAvailable.notify_all();
        if (_worker.joinable())
            _worker.join();
    }

    WorkerThread(const WorkerThread&) = delete;
    WorkerThread(WorkerThread&&) = default;
    WorkerThread& operator=(const WorkerThread&) = delete;
    WorkerThread& operator=(WorkerThread&&) = default;

    // Wake up the worker
    void Notify() {
        _hasWork = true;
        _workAvailable.notify_one();
    }

protected:
    virtual void Work() = 0;
    bool HasWork() { return _hasWork; }

private:
    void Worker() {
        SPDLOG_INFO("Starting worker");
        while (_alive) {
            _hasWork = false;
            try {
                Work();
            }
            catch (const std::exception& e) {
                SPDLOG_ERROR(e.what());
            }

            // New work could be requested while work is being done
            if (!_hasWork) {
                std::unique_lock lock(_notifyLock);
                _workAvailable.wait(lock); // sleep until work requested
            }
        }
        SPDLOG_INFO("Stopping worker");
    }
};
