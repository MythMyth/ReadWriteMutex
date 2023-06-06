###RW mutex

Read-Write mutex implementation for C++ 11</br>
</br>
RWMutex class</br>
Instance of this class is uncopiable</br>
</br>
ReadLock() : Lock for read</br>
WriteLock() : Lock for write</br>
ReadUnlock() : Unlock mutex, follow ReadLock() after read data completed</br>
WriteUnlock() : Unlock mutex, follow WriteLock() after read data completed</br>
</br>
RWLockGuard class</br>
Instance of this class can move only</br>
</br>
release(): release mutex owner then return pointer point to holding mutex</br>
mutex(): get pointer point to holding mutex</br>
owns_mutex(): lock own mutex or not</br>
operator bool(): lock own mutex or not</br>
swap(RWLockGuard&): swap content with another lock</br>
swap(RWLockGuard&, RWLockGuard&): swap 2 lock</br>
