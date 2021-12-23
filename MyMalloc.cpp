#include "MyMalloc.h"

#include <string.h>
#include <stdio.h>
#include <cstdarg>
#include <assert.h> 
#include <stdlib.h> 

#define MAX_LOG_SIZE        500
#define LEN_MEMORY_SIZE     25000

#define MEM_ALLOCATED       (0x1 << 7)
#define MEM_FREE            (0x0 << 7)
#define SIZE_OF_SECTOR      sizeof(sector)

void* p_Buffer = nullptr;
bool b_Initialized = false;
bool b_DebugLogs = false;

//*************************************************************************************************
void *skdev::mymalloc(unsigned short uiMemorySize)
{
    LOG_DEBUG("---------- %s called with %d ----------", __func__, uiMemorySize);
    if (!b_Initialized)
    {
        // Filling the whole buffer with zeros to avoid bad reads
        LOG_DEBUG("Initializing the memory buffer for the 1st time");
        p_Buffer = malloc(LEN_MEMORY_SIZE);
        memset(p_Buffer, 0, LEN_MEMORY_SIZE);
        LOG_DEBUG("Buffer Location %p", p_Buffer);

        // Initialize the first sector of the buffer
        sector* pstInitialSector = (sector*)p_Buffer;
        pstInitialSector->bIsFree = true;
        pstInitialSector->uiSize = LEN_MEMORY_SIZE - SIZE_OF_SECTOR;
        pstInitialSector->pstPrevSector = nullptr;
        b_Initialized = true;
    }

    // Jump from sector to sector untill you find a free sector with enough space
    sector* pstCurrentSector = (sector*)p_Buffer;
    while ((char*)pstCurrentSector - (char*)p_Buffer < LEN_MEMORY_SIZE)
    {
        printSector("Current Sector", pstCurrentSector);

        // Check if the current sector is good for use
        if (pstCurrentSector->bIsFree && pstCurrentSector->uiSize >= uiMemorySize)
        {
            // Check if, after allocating the memory, will there be enough space for another sector
            if ((char*)pstCurrentSector + SIZE_OF_SECTOR*2 + uiMemorySize - (char*)p_Buffer < LEN_MEMORY_SIZE)
            {
                sector* pstNextSector = (sector*)((char*)pstCurrentSector + SIZE_OF_SECTOR + uiMemorySize);
                pstNextSector->bIsFree = true;
                pstNextSector->pstPrevSector = pstCurrentSector;
                pstNextSector->uiSize = pstCurrentSector->uiSize - SIZE_OF_SECTOR - uiMemorySize;
                printSector("New Next Sector", pstNextSector);
            }
            // if not use the whole size which may be greater than the size requested
            else
            {
                LOG_DEBUG("Allocating a size of %d instead of %d. Reason: BufferWaste", 
                    pstCurrentSector->uiSize, uiMemorySize);
                uiMemorySize = pstCurrentSector->uiSize;
            }
            
            // Update the current sector
            pstCurrentSector->bIsFree = false;
            pstCurrentSector->uiSize = uiMemorySize;
            printSector("Returning Sector", pstCurrentSector);
            return (void*)((char*)pstCurrentSector + SIZE_OF_SECTOR);            
        }

        pstCurrentSector = (sector*)((char*)pstCurrentSector + SIZE_OF_SECTOR + pstCurrentSector->uiSize);
        LOG_DEBUG("Moving to SectorIndex: %p", pstCurrentSector);
    }

    LOG_DEBUG("Sector out of range: %p", pstCurrentSector);

    return 0;
}

//*************************************************************************************************
void skdev::printSector(const char* pzName, sector *pstSector)
{
    LOG_DEBUG("%s %p-> PrevSecIndex: %p, Size: %d, Free: %s, DataBufferStart: %p, Data: %s", 
        pzName, pstSector, pstSector->pstPrevSector, pstSector->uiSize,
        pstSector->bIsFree ? "Yes" : "No", (char*)pstSector + SIZE_OF_SECTOR,
        pstSector->bIsFree ? "No Data": (char*)pstSector + SIZE_OF_SECTOR);
}

//*************************************************************************************************
void skdev::myfree(void *pAddress)
{
    LOG_DEBUG("---------- %s called with %p ----------", __func__, pAddress);
    sector* pstSector = (sector*)((char*)pAddress - SIZE_OF_SECTOR);
    if (!isValidSector(pstSector))
    {
        LOG_ERROR("Provided Address at %p does not point to a valid sector", pAddress);
        assert(false);
    }
    printSector("Recycling Sector", pstSector);
    pstSector->bIsFree = true;
    
    // Merge with surrounding sectors if possible
    if (pstSector->pstPrevSector != nullptr)
    {
        sector* pstPrevSector = pstSector->pstPrevSector;
        printSector("Left Sector", pstPrevSector);
        if (pstPrevSector->bIsFree)
        {
            LOG_DEBUG("Merging with the left sector");
            pstPrevSector->uiSize += pstSector->uiSize + SIZE_OF_SECTOR;
            memset((char*)pstSector, 0, pstSector->uiSize + SIZE_OF_SECTOR);
            pstSector = pstPrevSector;
            printSector("Merged Sector", pstSector);
        }
    }
    else
    {
        LOG_DEBUG("There is no sector on the left");        
    }

    if (((char*)pstSector) + SIZE_OF_SECTOR + pstSector->uiSize < (char*)p_Buffer + LEN_MEMORY_SIZE - 1)
    {
        sector* pstNextSector = (sector*)(((char*)pstSector) + SIZE_OF_SECTOR + pstSector->uiSize);
        printSector("Right Sector", pstNextSector);
        LOG_DEBUG("Updating Right Sector");
        pstNextSector->pstPrevSector = pstSector;
        if (pstNextSector->bIsFree)
        {
            LOG_DEBUG("Merging with the Right sector");
            sector* pstRightOfNextSector = (sector*)(((char*)pstNextSector) + SIZE_OF_SECTOR + pstNextSector->uiSize);
            pstSector->uiSize += pstNextSector->uiSize + SIZE_OF_SECTOR;
            pstSector->bIsFree = true;
            memset((char*)pstNextSector, 0, pstNextSector->uiSize + SIZE_OF_SECTOR);    
            printSector("Merged Sector", pstSector);
            if (isValidSector(pstRightOfNextSector))
            {
                LOG_DEBUG("Updating Right Sector of Next Sector");
                printSector("Right of Next Sector", pstRightOfNextSector);
                pstRightOfNextSector->pstPrevSector = pstSector;
                printSector("Updated Right of Next Sector", pstRightOfNextSector);
            }
        }
    }
    else
    {
        LOG_DEBUG("There is no sector on the right");
    }
    
    printSector("Final Free-ed Sector", pstSector);    
}

//*************************************************************************************************
sector *skdev::getSector(void *pAddress)
{
    return (sector*)((char*)pAddress - SIZE_OF_SECTOR);
}

//*************************************************************************************************
sector *skdev::getInitialSector()
{
    return (sector*)p_Buffer;
}

//*************************************************************************************************
bool skdev::isValidSector(sector *pstSector)
{
    if ((char*)pstSector - (char*)p_Buffer > LEN_MEMORY_SIZE)
    {
        return false;
    }
    // TODO: Validate
    return true;
}

//*************************************************************************************************
void skdev::logDebug(skdev::LogLevel iLevel, const char *zFormat, ...)
{
    const char* zPrefix = nullptr;
    switch (iLevel)
    {
    case LogLevel::DEBUG:
        if (!b_DebugLogs)
        {
            return;
        }
        zPrefix = "DEBUG";
        break;
    case LogLevel::INFO:
        zPrefix = "INFO";
        break;
    case LogLevel::WARNING:
        zPrefix = "WARNING";
        break;
    case LogLevel::ERROR:
        zPrefix = "ERROR";
        break;
    
    default:
        break;
    }

    va_list args;
    va_start(args, zFormat);
    char zLogLine[MAX_LOG_SIZE];
    vsnprintf(zLogLine, MAX_LOG_SIZE, zFormat, args);
    printf("|%s| %s\n", zPrefix, zLogLine);
    va_end(args);
}

//*************************************************************************************************
void skdev::enableLogging()
{
    LOG_DEBUG("Debug logs enabled");
    b_DebugLogs = true;
}