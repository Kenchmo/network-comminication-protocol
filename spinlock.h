//
//  spinlock.h
//  network communication protocol
//
// Reference: http://www.boost.org/doc/libs/1_54_0/doc/html/atomic/usage_examples.html
// http://www.chaoticmind.net/~hcb/projects/boost.atomic/

#ifndef spinlock_h
#define spinlock_h

//#include "boost.atomic/boost/atomic.hpp"
//
//class spinlock {
//private:
//    typedef enum {Locked, Unlocked} LockState;
//    boost::atomic<LockState> state_;
//    
//public:
//    spinlock() : state_(Unlocked) {}
//    
//    void lock()
//    {
//        while (state_.exchange(Locked, boost::memory_order_acquire) == Locked) {
//            /* busy-wait */
//        }
//    }
//    void unlock()
//    {
//        state_.store(Unlocked, boost::memory_order_release);
//    }
//};

using namespace std;

class spinlock {
private:
    typedef enum {Locked, Unlocked} LockState;
    atomic<LockState> state_;
    
public:
    spinlock() : state_(Unlocked) {}
    
    void lock()
    {
        while (state_.exchange(Locked, memory_order_acquire) == Locked) {
            /* busy-wait */
        }
    }
    void unlock()
    {
        state_.store(Unlocked, memory_order_release);
    }
};

#endif /* spinlock_h */
