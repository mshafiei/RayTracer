#include <core/memoryarena.h>

namespace rt
{
    void *AllocAligned(std::size_t size) {
#if defined(_WIN32)
        return _aligned_malloc(size, PBRT_L1_CACHE_LINE_SIZE);
#elif defined (__OpenBSD__) || defined(__APPLE__)
        Allocate excess memory to ensure an aligned pointer can be returned
#else
        return memalign(PBRT_L1_CACHE_LINE_SIZE, size);
#endif
    }

    MemoryArena::MemoryArena(uint32_t bs) {
        blockSize = bs;
        curBlockPos = 0;
        currentBlock = AllocAligned<char>(blockSize);
    }

    void* MemoryArena::Alloc(uint32_t sz) {
        sz = ((sz + 15) & (~15));
            if (curBlockPos + sz > blockSize) {
                usedBlocks.push_back(currentBlock);
                if (availableBlocks.size() && sz <= blockSize) {
                    currentBlock = availableBlocks.back();
                    availableBlocks.pop_back();
                }
                else
                    currentBlock = AllocAligned<char>(std::max(sz, blockSize));
                curBlockPos = 0;
            }
        void *ret = currentBlock + curBlockPos;
        curBlockPos += sz;
        return ret;
    }

    
    void MemoryArena::FreeAll() {
        curBlockPos = 0;
        while (usedBlocks.size()) {
            availableBlocks.push_back(usedBlocks.back());
            usedBlocks.pop_back();
        }
    }
    void FreeAligned(void *ptr) {
        if (!ptr) return;
#if defined(_WIN32)
        _aligned_free(ptr);
#elif defined (PBRT_IS_OPENBSD) || defined(PBRT_IS_APPLE)
        free(((void**)ptr)[-1]);
#else
        free(ptr);
#endif
    }
}
