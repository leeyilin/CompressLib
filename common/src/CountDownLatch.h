#ifndef COUNTDOWNLATCH_H 
#define COUNTDOWNLATCH_H

#include "Condition.h"

//taken from muduo/base/CountDownLatch.h
namespace BackCom {
    class CountDownLatch : public boost::noncopyable {
        private:
            mutable CLock mMutex;
            Condition mCondition;
            int mCount;
        public:
            explicit CountDownLatch(int count) : mMutex(), mCondition(mMutex), mCount(count) {
            }
            void Wait() {
                CAutoLock lock(mMutex);
                while(mCount > 0) {
                    mCondition.Wait();
                }
            }
            void CountDown() {
                CAutoLock lock(mMutex);
                --mCount;
                if(mCount == 0) {
                    mCondition.NotifyAll();
                }
            }
            int GetCount() const {
                CAutoLock lock(mMutex);
                return mCount;
            }
    };
}

#endif /*COUNTDOWNLATCH_H*/
