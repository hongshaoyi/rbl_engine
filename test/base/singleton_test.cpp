#include "gtest/gtest.h"
#include "singleton.h"

class ForTest {
public:
    bool for_test() {return true;}
};

TEST(Singleton, Singleton) {
    ForTest &test = Singleton<ForTest>::get_instance();

    ASSERT_TRUE(test.for_test());
}