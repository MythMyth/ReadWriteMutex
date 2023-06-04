#include<mutex>
#include<condition_variable>

class RWMutex final {
public:
    enum RWLockType {
        READ_LOCK,
        WRITE_LOCK
    };
    RWMutex() : m_readReqCnt(0U), m_writeReqCnt(0U), m_writing(false) {}
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
    void* operator new(size_t);
    void* operator new(size_t, void*);
    void* operator new[](size_t);
    void* operator new[](size_t, void*);
    //Private all new operator
    std::mutex m_mutex;
    std::condition_variable m_cv;
    unsigned int m_readReqCnt;
    unsigned int m_writeReqCnt;
    bool m_writing;
};

class RWLockGuard final {
public:
    explicit RWLockGuard(RWMutex& mutex, RWMutex::RWLockType lockType = RWMutex::READ_LOCK) : m_mutex(mutex), m_type(lockType) {
        if(m_type == RWMutex::READ_LOCK) {
            m_mutex.ReadLock();
        } else {
            m_mutex.WriteLock();
        }
    }
    RWLockGuard(const RWLockGuard& another) = delete;
    RWLockGuard(RWLockGuard&& another) = delete;
    RWLockGuard& operator= (const RWLockGuard& another) = delete;
    RWLockGuard& operator= (RWLockGuard&& another) = delete;

    ~RWLockGuard() {
        if(m_type == RWMutex::READ_LOCK) {
            m_mutex.ReadUnlock();
        } else {
            m_mutex.WriteUnlock();
        }
    }

private:
    void* operator new(size_t);
    void* operator new(size_t, void*);
    void* operator new[](size_t);
    void* operator new[](size_t, void*);
    RWMutex& m_mutex;
    const RWMutex::RWLockType m_type;
};
