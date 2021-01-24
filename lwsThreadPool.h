#define LWS_TEST 1
#ifdef LWS_TEST
#include <stdio.h>
#endif


#ifndef LWS__THREAD_POOL
#define LWS__THREAD_POOL

#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>
//只有一个FUNCTION虚函数，继承类必须实现，来满足线程的调度
struct lws_task
{
    virtual void task_function(){return;};
};

class lws_thread_pool
{
public:
    lws_thread_pool(int pthread_num, int tasks_num);
    lws_thread_pool();
    ~lws_thread_pool();
    int add_task(lws_task *task);

private:
    //COND锁对应的互斥锁
    std::mutex cond_mutex;
    //COND锁
    std::condition_variable cond;
    //最大等待任务数量，超过后无法创建新任务
    int max_tasks = 20;
    //当前任务数量，等同于TASK_QUEUE.SIZE(),但是读取该数据无需加锁
    int count_tasks = 0;
    //任务队列，保存的是LWS_TASK类型的指针
    std::queue<lws_task *> task_queue;
    //关机指令
    int shutdown = 0;
    //当前运行的线程数量
    int count_thread=0;
    //thread指针数组
    std::vector<std::thread *> threads;

    static void *thread_work_loop(lws_thread_pool* pool);

public:
    enum PoolReturn
    {
        PR_DEFAULT = 1,
        PR_TASKS_FULL = -1,
        PR_SHUTDOWN = -2,
        PR_INVALID_TASK = -3
    };

private:
    enum threadReturn
    {
        TR_DEFAULT = 1,
        TR_NOTASK = -1
    };
};

#endif
