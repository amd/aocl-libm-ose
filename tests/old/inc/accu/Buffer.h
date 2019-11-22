#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "Exception.h"

#ifdef WIN64

inline void *AlignedMalloc(size_t alignment, size_t bytes)
{
    return _aligned_malloc(bytes, alignment);
}

inline void AlignedFree(void *pMem)
{
    _aligned_free(pMem);
}

#else

inline void *AlignedMalloc(size_t alignment, size_t bytes)
{
    void *pMem;
    posix_memalign(&pMem, alignment, bytes);
    return pMem;
}

inline void AlignedFree(void *pMem)
{
    free(pMem);
}

#endif // WIN64

// Generic buffer class
template <typename T>
class Buffer
{
    size_t count;           // number of elements
    size_t bytes;           // number of bytes
    size_t alignment;       // alignment

    T *pData;               // pointer to data

    void MemAlloc()
    {
        try
        {
            // check alignment
            switch(alignment)
            {
            case 8:     break;
            case 16:    break;
            case 32:    break;
            default:    throw GeneralException("Invalid alignment value", ERROR_LOCATION);
            }
    
            // allocate memory
            void *pMem = AlignedMalloc(alignment, bytes);
            if(pMem == NULL)
            {
                throw GeneralException("Memory allocation failure", ERROR_LOCATION);
            }
            pData = (T *)pMem;
        }
        catch(GeneralException &excp)
        {
            excp.PrintMessageAndExit();
        }
    }

    void MemFree()
    {
        AlignedFree(pData);
    }
        
    void Copy(const Buffer<T> &v_buf)
    {
        count = v_buf.count;
        bytes = v_buf.bytes;
        alignment = v_buf.alignment;

        MemAlloc();
        memcpy(pData, v_buf.pData, bytes);
    }

public:     
    Buffer() : count(0), bytes(0), alignment(0), pData(NULL)
    {}

    Buffer(const Buffer<T> &v_buf)
    {
        if(this != &v_buf)
        {
            Copy(v_buf);
        }
    }

    Buffer &operator=(const Buffer<T> &v_buf)
    {
        if(this != &v_buf)
        {
            MemFree();
            Copy(v_buf);
        }

        return *this;
    }
     
    Buffer(size_t v_count, size_t v_alignment=16)
    {
        count = v_count;
        bytes = count * sizeof(T);
        alignment = v_alignment;

        MemAlloc();
    }

    void Init(size_t v_count, size_t v_alignment=16)
    {
        count = v_count;
        bytes = count * sizeof(T);
        alignment = v_alignment;

        MemAlloc();
    }

    T *Data()
    {
        return pData;
    }

    const T *Data() const
    {
        return pData;
    }

    size_t Count() const
    {
        return count;
    }
 
    ~Buffer()
    {
        MemFree();
    }
};

#endif // __BUFFER_H__

