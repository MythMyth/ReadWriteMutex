#include<mutex>
#include<condition_variable>

class RWMutex{
public:
    RWMutex() : m_readReqCnt(0U), m_writeReqCnt(0U), m_writing(false)/*, m_refCount(1U)*/ {}
    RWMutex(RWMutex&& another) = delete;
    RWMutex& operator= (RWMutex&& another) = delete;
    RWMutex(const RWMutex& another) = delete;
    RWMutex& operator= (const RWMutex& another) = delete;

    void ReadLock() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [&] { return m_writeReqCnt == 0U && m_writing == false; });
        m_readReqCnt++;
    }

    void ReadUnlock() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_readReqCnt--;
        m_cv.notify_all();
    }

    void WriteLock() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_writeReqCnt++;
        m_cv.wait(lock, [&] {return m_readReqCnt == 0U && m_writing == false;});
        m_writeReqCnt--;
        m_writing = true;
    }

    void WriteUnlock() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_writing = false;
        m_cv.notify_all();
    }

private:

//    unsigned int m_refCount;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    unsigned int m_readReqCnt;
    unsigned int m_writeReqCnt;
    bool m_writing;
};
