#include "MyMalloc.h"
#include <string.h>
#include <assert.h> 
#ifdef GTEST_ACTIVE
#include <gtest/gtest.h>
#endif

#define SIZE_OF_SECTOR      sizeof(sector)

#ifndef GTEST_ACTIVE
int main()
{
    skdev::enableLogging();
    void* pAddress1 = skdev::mymalloc(25000 - SIZE_OF_SECTOR - 4);
    assert(skdev::getSector(pAddress1)->uiSize == 25000 - SIZE_OF_SECTOR);

    void* pAddress2 = skdev::mymalloc(1);
    assert(pAddress2 ==  nullptr);

    skdev::myfree(pAddress1);
    assert(skdev::getInitialSector()->uiSize == 25000 - SIZE_OF_SECTOR);
    return 0;
}
#endif

#ifdef GTEST_ACTIVE

// Demonstrate some basic assertions.
TEST(MyMallocTests, BasicMemoryAllocation) {
    skdev::enableLogging();
    void* pAddress = skdev::mymalloc(100);
    strncpy((char*)pAddress, "SULOCHANA", 9);

    sector* pSector = skdev::getSector(pAddress);
    EXPECT_EQ(pSector->bIsFree, false);
    EXPECT_EQ(pSector->uiSize, 100);
    EXPECT_EQ(pSector->pstPrevSector, nullptr);
    EXPECT_STRCASEEQ(((char*)pSector + SIZE_OF_SECTOR), "SULOCHANA");

    skdev::myfree(pAddress);

    pSector = skdev::getInitialSector();
    EXPECT_EQ(pSector->bIsFree, true);
    EXPECT_EQ(pSector->uiSize, 25000 - SIZE_OF_SECTOR);
    EXPECT_EQ(pSector->pstPrevSector, nullptr);
}

TEST(MyMallocTests, MultipleMemoryAllocation) {
    skdev::enableLogging();
    void* pAddress1 = skdev::mymalloc(100);
    void* pAddress2 = skdev::mymalloc(250);
    void* pAddress3 = skdev::mymalloc(500);
    void* pAddress4 = skdev::mymalloc(1000);

    EXPECT_EQ(skdev::getSector(pAddress1)->uiSize, 100);
    EXPECT_EQ(skdev::getSector(pAddress2)->uiSize, 250);
    EXPECT_EQ(skdev::getSector(pAddress3)->uiSize, 500);
    EXPECT_EQ(skdev::getSector(pAddress4)->uiSize, 1000);

    skdev::myfree(pAddress2);
    skdev::myfree(pAddress1);
    EXPECT_EQ(skdev::getInitialSector()->uiSize, 100 + 250 + SIZE_OF_SECTOR);

    skdev::myfree(pAddress3);
    EXPECT_EQ(skdev::getInitialSector()->uiSize, 100 + 250 + 500 + SIZE_OF_SECTOR*2);

    skdev::myfree(pAddress4);
    EXPECT_EQ(skdev::getInitialSector()->uiSize, 25000 - SIZE_OF_SECTOR);
}

TEST(MyMallocTests, SingleMemoryFull) {
    skdev::enableLogging();
    void* pAddress1 = skdev::mymalloc(25000 - SIZE_OF_SECTOR);
    EXPECT_EQ(skdev::getSector(pAddress1)->uiSize, 25000 - SIZE_OF_SECTOR);

    void* pAddress2 = skdev::mymalloc(1);
    EXPECT_EQ(pAddress2,  nullptr);

    skdev::myfree(pAddress1);
    EXPECT_EQ(skdev::getInitialSector()->uiSize, 25000 - SIZE_OF_SECTOR);
}

TEST(MyMallocTests, SingleMemoryAlmostFull) {
    skdev::enableLogging();
    void* pAddress1 = skdev::mymalloc(25000 - SIZE_OF_SECTOR - 1);
    EXPECT_EQ(skdev::getSector(pAddress1)->uiSize, 25000 - SIZE_OF_SECTOR);

    void* pAddress2 = skdev::mymalloc(1);
    EXPECT_EQ(pAddress2,  nullptr);

    skdev::myfree(pAddress1);
    EXPECT_EQ(skdev::getInitialSector()->uiSize, 25000 - SIZE_OF_SECTOR);

    pAddress1 = skdev::mymalloc(25000 - SIZE_OF_SECTOR*2);
    EXPECT_EQ(skdev::getSector(pAddress1)->uiSize, 25000 - SIZE_OF_SECTOR);

    pAddress2 = skdev::mymalloc(1);
    EXPECT_EQ(pAddress2,  nullptr);

    skdev::myfree(pAddress1);
    EXPECT_EQ(skdev::getInitialSector()->uiSize, 25000 - SIZE_OF_SECTOR);
}

TEST(MyMallocTests, EdgeAllocationMinimumSize) {
    skdev::enableLogging();
    void* pAddress1 = skdev::mymalloc(25000 - SIZE_OF_SECTOR*2 - 1);
    EXPECT_EQ(skdev::getSector(pAddress1)->uiSize, 25000 - SIZE_OF_SECTOR*2 -1);

    void* pAddress2 = skdev::mymalloc(1);
    EXPECT_NE(pAddress2,  nullptr);
    EXPECT_EQ(skdev::getSector(pAddress2)->uiSize, 1);

    skdev::myfree(pAddress1);
    skdev::myfree(pAddress2);
    EXPECT_EQ(skdev::getInitialSector()->uiSize, 25000 - SIZE_OF_SECTOR);
}
#endif