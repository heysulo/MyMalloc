#define LOG_DEBUG(fmt, args...)     skdev::logDebug(skdev::LogLevel::DEBUG, fmt, ##args)
#define LOG_INFO(fmt, args...)      skdev::logDebug(skdev::LogLevel::INFO, fmt, ##args)
#define LOG_WARN(fmt, args...)      skdev::logDebug(skdev::LogLevel::WARNING, fmt, ##args)
#define LOG_ERROR(fmt, args...)     skdev::logDebug(skdev::LogLevel::ERROR, fmt, ##args)

struct sector {
    bool bIsFree;
    unsigned short uiSize;
    sector* pstPrevSector;
};

namespace skdev {
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    void* mymalloc(unsigned short uiMemorySize);
    void myfree(void* pAddress);
    void printSector(const char* pzName, sector* pstSector);

    bool isValidSector(sector* pstSector);

    void logDebug(LogLevel iLevel, const char* zFormat, ...);
    void enableLogging();
}