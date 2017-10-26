#ifndef ATOMIC_H
#define ATOMIC_H

namespace BackCom {
    template<typename T>
    class AtomicInteger {
        private:
            volatile T mValue;
        protected:
            AtomicInteger(const AtomicInteger&);
            const AtomicInteger& operator=(const AtomicInteger&);
        public:
            AtomicInteger() : mValue(0) {}

            T Get() const {
                // in gcc >= 4.7: __atomic_load_n(&mValue_, __ATOMIC_SEQ_CST)
                return __sync_val_compare_and_swap(const_cast<volatile T *>(&mValue), 0, 0);
            }

            T Set(T newValue) {
                // in gcc >= 4.7: __atomic_store_n(&mValue, newValue, __ATOMIC_SEQ_CST)
                return __sync_lock_test_and_set(&mValue, newValue);
            }

            T GetAndAdd(T _x) {
                // in gcc >= 4.7: __atomic_fetch_add(&mValue_, x, __ATOMIC_SEQ_CST)
                return __sync_fetch_and_add(&mValue, _x);
            }

            T AddAndGet(T _x) {
                return GetAndAdd(_x) + _x;
            }

            T Increment() {
                return AddAndGet(1);
            }

            T Decrement() {
                return AddAndGet(-1);
            }

            AtomicInteger<T>& operator=(T v) {
                this->Set(v);
                return *this;
            }

            operator T() {
                return this->Get();
            }
    };
}

typedef BackCom::AtomicInteger<int> AtomicInt32;

#endif /*ATOMIC_H*/
