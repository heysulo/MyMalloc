#include "MyMalloc.h"
#include <string.h>
#include <gtest/gtest.h>

// int main()
// {
//     skdev::enableLogging();
//     LOG_DEBUG("Starting application. Sizes -> sector: %d, char: %d, short: %d, bool: %d",
//          sizeof(sector), sizeof(char), sizeof(unsigned short), sizeof(bool));
//     void* pAddress = skdev::mymalloc(100);
//     strncpy((char*)pAddress, "SULOCHANA", 9);
//     skdev::myfree(pAddress);
//     LOG_DEBUG("Execution completed");
//     return 0;
// }

// Demonstrate some basic assertions.
TEST(MyMallocTests, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 43);
}
