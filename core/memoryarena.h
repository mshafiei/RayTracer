#ifndef CG1RAYTRACER_GROUPS_MEMORYARENA_HEADER
#define CG1RAYTRACER_GROUPS_MEMORYARENA_HEADER

#include <vector>
#include <core/windowsdefines.h>
#ifndef PBRT_L1_CACHE_LINE_SIZE
#define PBRT_L1_CACHE_LINE_SIZE 64
#endif

//memory manager according to PBR book pg. 1015
namespace rt
{
    void *AllocAligned(std::size_t size);

    template <typename T> T *AllocAligned(uint32_t count) {
        return (T *)AllocAligned(count * sizeof(T));
    }

    void FreeAligned(void *ptr);

    class MemoryArena {
    public:
        MemoryArena(uint32_t bs = 32768);
        void *Alloc(uint32_t sz);
        
        template<typename T> T *Alloc(uint32_t count = 1) {
            T *ret = (T *)Alloc(count * sizeof(T));
            for (uint32_t i = 0; i < count; ++i)
                new (&ret[i]) T();
            return ret;
        }

        void FreeAll();
    private:
        uint32_t curBlockPos, blockSize;
        char *currentBlock;
        std::vector<char *> usedBlocks, availableBlocks;
    };
}

#endif
