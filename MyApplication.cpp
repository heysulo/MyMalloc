#include "MyMalloc.h"
#include <string.h>

int main()
{
    skdev::enableLogging();
    LOG_DEBUG("Starting application. Sizes -> sector: %d, char: %d, short: %d, bool: %d",
         sizeof(sector), sizeof(char), sizeof(unsigned short), sizeof(bool));
    void* pAddress = skdev::mymalloc(100);
    strncpy((char*)pAddress, "SULOCHANA", 9);
    skdev::myfree(pAddress);
    LOG_DEBUG("Execution completed");
    return 0;
}