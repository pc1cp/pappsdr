// ============================================================================
//
// File .......: mixer.h
//
// Author(s) ..: Stefan Fendt
//
// Licence ....: GNU-GPL V2.0 or later
//
// Purpose ....: Declaration of an audio-queue for the Internet-Radio-Client
//
// ============================================================================

#ifndef __AUDIOQUEUE_HPP__
#define __AUDIOQUEUE_HPP__

//#include "sample.hpp"
#include "complexsample.hpp"
#include "wx/thread.h"

class AudioQueue
{
    private:

    unsigned int        m_wPointer;
    unsigned int        m_rPointer;
    volatile double     m_rTime;
    volatile double     m_wTime;
    unsigned int        m_Size;
    ComplexSample*      m_Data;

    wxMutex             m_QueueLock;
    //wxSemaphore*        m_QueueSemaphore;

    public:

    // Initialization
    AudioQueue( unsigned int size = 0 );

    // Deinitialization
    ~AudioQueue();

    void lock()  { m_QueueLock.Lock();   }
    void unlock(){ m_QueueLock.Unlock(); }

    // Resize queue
    void resize( unsigned int size );

    // Return current lenght until queue is empty
    unsigned int getLength();

    // Return maximum size of the queue
    unsigned int size();

    // Return timing-error between in- and out-samples
    double getTimingError();

    // Put a sample on the queue
    void put( ComplexSample& sample );

    // Pull a sample off the queue
    ComplexSample pull();

    // Peek into the queue for future or past samples (if possible)
    // This does *not* take samples from the queue, it just delivers the
    // value at that relative position to the read-pointer.
    ComplexSample peek( int offset=0 );
};

#endif
