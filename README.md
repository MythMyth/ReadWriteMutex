###RW mutex

Read-Write mutex implementation for C++ 11

RWMutex class
Instance of this class is uncopiable

ReadLock() : Lock for read
WriteLock() : Lock for write
ReadUnlock() : Unlock mutex, follow ReadLock() after read data completed
WriteUnlock() : Unlock mutex, follow WriteLock() after read data completed

RWLockGuard class
Instance of this class can move only

release(): release mutex owner then return pointer point to holding mutex
mutex(): get pointer point to holding mutex
owns_mutex(): lock own mutex or not
operator bool(): lock own mutex or not
swap(RWLockGuard&): swap content with another lock
swap(RWLockGuard&, RWLockGuard&): swap 2 lock
