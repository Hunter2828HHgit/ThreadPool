#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

class ThreadPool{
public:
    ThreadPool(int thread_num) :stop(false){
        for(int i = 0;i<thread_num;i++){
            workers.emplace_back([this]{
                while(true){
                    unique_lock<mutex> locker(mu);
                    cv.wait(locker, [this]{return stop or !tasks.empty();});
                    if(stop and tasks.empty()){
                        return;
                    }
                    auto task = move(tasks.front());
                    tasks.pop();
                    locker.unlock();
                    task(); 
                }
            });
        }
    }

    template<class T>
    void enqueue(T&& task){
        unique_lock<mutex> locker(mu);
        tasks.emplace(forward<T>(task));
        locker.unlock();
        cv.notify_one();
    }

    ~ThreadPool(){
        unique_lock<mutex> locker(mu);
        stop = 1;
        locker.unlock();
        cv.notify_all();
        for(auto& worker: workers){
            worker.join();
        }
    }

private:
    bool stop;
    mutex mu;
    queue<function<void()>> tasks;
    vector<thread> workers;
    condition_variable cv;
};

signed main(){
    int tasksNum, threadNum;
    cin>>tasksNum>>threadNum;
    ThreadPool pool(threadNum);
    for(int i = 0;i<tasksNum;i++){
        pool.enqueue([i]{
            printf("Task %d is executed by thread\n", i);
            this_thread::sleep_for(chrono::seconds(1));
        });
    }
    return 0;
}
