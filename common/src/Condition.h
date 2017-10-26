#ifndef CONDITION_H 
#define CONDITION_H

#include <pthread.h>
#include <boost/noncopyable.hpp>
#include "lock.h"

namespace BackCom {
    class Condition : public boost::noncopyable {
        private:
            CLock& mMutex;
            pthread_cond_t mCond;
        public:
            explicit Condition(CLock& mutex) : mMutex(mutex) {
                pthread_cond_init(&mCond, NULL);
            }
            ~Condition() {
                pthread_cond_destroy(&mCond);
            }
            void Wait() {
                pthread_cond_wait(&mCond, mMutex.GetPthreadMutex());
            }

            void Notify() {
                pthread_cond_signal(&mCond);
            }
            void NotifyAll() {
                pthread_cond_broadcast(&mCond);
            }
    };
}

#endif /*CONDITION_H*/
