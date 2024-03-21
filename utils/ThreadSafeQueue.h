#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
 
template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> q;
    std::mutex mut;
    std::condition_variable cond;
    int max_size;
 
public:
    ThreadSafeQueue(int max_size) : max_size(max_size) {};

    ~ThreadSafeQueue() {
        std::lock_guard<std::mutex> lock(mut);
    }

    void push(T new_value) {
        std::lock_guard<std::mutex> lock(mut);
        q.push(new_value);
        if(q.size() > max_size) q.pop();
        cond.notify_one();
    }
 
    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(mut);
        cond.wait(lock, [&] { return !q.empty(); });
        value = q.front();
        q.pop();
    }
 
    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mut);
        if (q.empty()) return false;
        value = q.front();
        q.pop();
        return true;
    }

    bool try_pop() {
        std::lock_guard<std::mutex> lock(mut);
        if (q.empty()) return false;
        q.pop();
        return true;
    }

    int size() {
        std::lock_guard<std::mutex> lock(mut);
        return q.size();
    }
};