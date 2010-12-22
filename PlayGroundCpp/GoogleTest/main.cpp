#include "gtest/gtest.h"


using testing::Test;
using testing::InitGoogleTest;


class Summator
{
public:
    Summator(int inA, int inB) : mA(inA), mB(inB) {}

    int result() const { return mA + mB; }

private:
    int mA, mB;
};


class SummatorTest : public Test
{
protected:
    virtual void SetUp();
    virtual void TearDown();
};


void SummatorTest::SetUp()
{
}


void SummatorTest::TearDown()
{
}


TEST_F(SummatorTest, ResultIsCorrect)
{
    Summator sum(3, 4);
    EXPECT_EQ(sum.result(), 7);
    EXPECT_TRUE(sum.result() > 6);
    EXPECT_TRUE(sum.result() < 8);
}


TEST_F(SummatorTest, ResultIsOdd)
{
    Summator sum(1, 2);
    EXPECT_TRUE(sum.result() % 2 == 1);
}


int main(int argc, char **argv)
{
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

