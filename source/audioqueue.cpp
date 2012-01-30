#include "audioqueue.hpp"
#include <iostream>

// ============================================================================
// Initialization
// ============================================================================

AudioQueue::AudioQueue( unsigned int size )
    :
    m_wPointer(0),
    m_rPointer(0),
    m_rTime(0),
    m_wTime(0),
    m_Size(0),
    m_Data(0)
//    m_QueueSemaphore(0)
{
    // if called with size argument, then create queue with that size
    if(size != 0)
    {
        m_Data = new ComplexSample[ size ];
        m_Size = size;
        //m_QueueSemaphore = new wxSemaphore( 0, size-10 );
    }
    std::cerr << "AudioQueue created...\n";
}

// ============================================================================
// Deinitialization
// ============================================================================

AudioQueue::~AudioQueue()
{
    delete [] m_Data;
    //delete    m_QueueSemaphore;
    std::cerr << "AudioQueue destroyed...\n";
}

// ============================================================================
// Resize queue
// ============================================================================

void AudioQueue::resize( unsigned int size )
{
    // if new size differs from old one...
    if( size != m_Size )
    {
        // get rid of old queue...
        delete [] m_Data;
        //delete    m_QueueSemaphore;

        m_Data = 0;
        m_Size = 0;

        // create new queue
        if(size != 0)
        {
            m_Data = new ComplexSample[ size ];
            m_Size = size;
            //m_QueueSemaphore = new wxSemaphore( 0, size-10 );
        }
    }
}

// ============================================================================
// How many samples can we read until queue is empty?
// ============================================================================

unsigned int AudioQueue::getLength()
{
    // Get the distance (in samples) from the write to the read-Pointer in
    // the queue and take into account that there might be a wrap-arround

    if( m_wPointer >= m_rPointer )
    {
        return( m_wPointer - m_rPointer );
    }

    return( m_Size - (m_rPointer - m_rPointer) );
}

// ============================================================================
// How big is the queue in total?
// ============================================================================

unsigned int AudioQueue::size()
{
    return( m_Size );
}

// ============================================================================
// Timing-Difference (for sound-card-sync)
// ============================================================================

double AudioQueue::getTimingError()
{
    // Get the time-distance (in nr of samples) for read and write attempts
    // to the queue.
    // This allows to measure the relative sampling-rate-error of an input-
    // source in regard to a given output-sink. This information will be
    // feed into a PID-controller during resampling of the input audio-data
    // in the according callback-function to prevent slow loss of audio-
    // sync in beetween two different hardware-clocks of two different
    // soundcards.
    //std::cerr << "m_wTime:" << m_wTime << " m_rTime:" << m_rTime << " \r";
    return( m_rTime - m_wTime );
    //return( +1 );//(100-getLength()) );
}

// ============================================================================
// Put a sample on the queue
// ============================================================================

void AudioQueue::put( ComplexSample& sample )
{
    // Lock Queue
    //wxMutexLocker lock(m_QueueLock);
    //if( m_QueueSemaphore->Post() == wxSEMA_NO_ERROR )
    {
        // check if there is some headroom in the queue to write this sample
        // otherwise just drop it...
        if( getLength() < (m_Size-1) )
        {
            // write Sample to current write-position
            m_Data[m_wPointer] = sample;

            // increment current write-position and limit it to queue-size
            // by using a wrap arround
            m_wPointer += 1;
            m_wPointer %= m_Size;
        }
        else
        {
            //std::cerr << "w";
        }
    }
    //else
    //{
    //    std::cerr << "w";
    //}

    // increment writeTime by one sample
    m_wTime = m_wTime+1.0;

    //Queue gets unlocked by leaving this scope...
}

// ============================================================================
// get a sample off the queue
// ============================================================================

ComplexSample AudioQueue::pull()
{
    // Lock Queue
    //wxMutexLocker lock(m_QueueLock);
    //if( m_QueueSemaphore->TryWait() == wxSEMA_NO_ERROR )
    //{
    // check if there is something on the queue that we can get...
    // if so, take that sample... if not just deliver the last sample...
    if( getLength() > 0 )
    {
        // increment current read-position and limit it to queue-size
        // by using a wrap arround
        m_rPointer += 1;
        m_rPointer %= m_Size;
    }
    //}
    else
    {
        //std::cerr << "r";
    }
    //if( m_rPointer == 0 )
//std::cerr //<< "wPtr: " << m_wPointer  << "\n"
          //<< "rPtr: " << m_rPointer  << "\n"
  //        << "err: "  << getTimingError() << "\n";

    // increment readTime by one sample

    m_rTime += 1.0;

    // Read sample from current read-position
    // Queue gets unlocked by leaving this scope...
    return( m_Data[m_rPointer] );
}
