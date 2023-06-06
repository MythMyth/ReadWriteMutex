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
    explicit RWLockGuard(RWMutex& mutex, RWMutex::RWLockType lockType = RWMutex::READ_LOCK) : m_mutex(&mutex), m_type(lockType) {
        if(m_type == RWMutex::READ_LOCK) {
            m_mutex->ReadLock();
        } else {
            m_mutex->WriteLock();
        }
    }
    RWLockGuard(const RWLockGuard& another) = delete;
    RWLockGuard(RWLockGuard&& another) {
        m_mutex = another.m_mutex;
        m_type = another.m_type;
        another.m_mutex = NULL;
    }

    RWLockGuard& operator= (const RWLockGuard& another) = delete;
    RWLockGuard& operator= (RWLockGuard&& another) {
        m_mutex = another.m_mutex;
        m_type = another.m_type;
        another.m_mutex = NULL;
        return *this;
    }

    ~RWLockGuard() {
        if(m_own_mutex == true) {
            if(m_type == RWMutex::READ_LOCK) {
                m_mutex->ReadUnlock();
            } else {
                m_mutex->WriteUnlock();
            }
        }
    }

    RWMutex* release() noexcept {
        RWMutex *ret = m_mutex;
        m_mutex = NULL;
        return ret;
    }

    RWMutex* mutex() const noexcept {
        return m_mutex;
    }

    bool owns_lock() const noexcept {
        return m_mutex != NULL;
    }

    bool operator bool() const noexcept {
        return m_mutex != NULL;
    }

    void swap(RWLockGuard& another) {
        RWMutex *t_mutex = m_mutex;
        RWMutex::RWLockType t_type = m_type;
        m_mutex = another.m_mutex;
        m_type = another.m_type;
        another.m_mutex = t_mutex;
        another.m_type = t_type;
    }

    friend void swap(RWLockGuard& lhs, RWLockGuard& rhs) {
        RWMutex *t_mutex = lhs.m_mutex;
        RWMutex::RWLockType t_type = lhs.m_type;
        lhs.m_mutex = rhs.m_mutex;
        lhs.m_type = rhs.m_type;
        rhs.m_mutex = t_mutex;
        rhs.m_type = t_type;
    }

private:
    void* operator new(size_t);
    void* operator new(size_t, void*);
    void* operator new[](size_t);
    void* operator new[](size_t, void*);
    RWMutex *m_mutex;
    RWMutex::RWLockType m_type;
};
