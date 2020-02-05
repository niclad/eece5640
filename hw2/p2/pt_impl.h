// pt_impl.h - pthreads class for easy implementation
#ifndef PT_IMPL_H
#define PT_IMPL_H

#include <pthread.h>

namespace PTI {
    class PT_Impl {
    private:
        int nThreads;   // number of threads to run on
    
    public:
        int GetNumThreads();    // get the number of threads

    }
}
