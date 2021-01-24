#include "lwsThreadPool.h"

lws_thread_pool::lws_thread_pool() : lws_thread_pool::lws_thread_pool(20, 20)
{
}

lws_thread_pool::lws_thread_pool(int pthread_num, int tasks_num)
{
    max_tasks = tasks_num;

    std::thread *thread_ptr;
    for (int i = 0; i < pthread_num; i++)
    {
        thread_ptr = new std::thread(thread_work_loop, this);
        if (thread_ptr != nullptr)
        {
            count_thread++;
            threads.push_back(thread_ptr);
        }
    }
    return;
}

lws_thread_pool::~lws_thread_pool()
{
    {
        std::unique_lock<std::mutex> uq_lock(cond_mutex);
        shutdown = 1;
        cond.notify_all();
    }
    threadReturn TR;
    for (auto ptr : threads)
    {
        ptr->join();
    }
}

int lws_thread_pool::add_task(lws_task *task)
{

    if (count_tasks == max_tasks)
    {
        return PR_TASKS_FULL;
    }
    if (shutdown)
    {
        return PR_SHUTDOWN;
    }
    if (task == nullptr)
        return PR_INVALID_TASK;
    //加锁
    std::unique_lock<std::mutex> uq_lock(cond_mutex);

    task_queue.push(task);
    count_tasks++;

    cond.notify_one();

    return PR_DEFAULT;
}

void *lws_thread_pool::thread_work_loop(lws_thread_pool *this_pool)
{
    lws_thread_pool *pool = this_pool;
    lws_task *task;
    threadReturn thread_return = TR_DEFAULT;
    while (1)
    {
        {
            //等待条件锁
            std::unique_lock<std::mutex> uq_lock(pool->cond_mutex);

            pool->cond.wait(uq_lock, [&pool]{return pool->count_tasks || pool->shutdown;});
            //非任务原因，被唤醒，退出

            if (pool->shutdown)
            {
                break;
            }
            task = pool->task_queue.front();
            pool->task_queue.pop();
            pool->count_tasks--;
        }

        task->task_function();
    }
#ifdef LWS_TEST
    printf("Pthread:%d return!\n",pool->count_tasks);
#endif



    //从循环中跳出
    pool->count_thread--;
    return NULL;
}