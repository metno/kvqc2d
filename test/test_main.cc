
#include <milog/milog.h>
#include <gtest/gtest.h>

int main(int argc, char* argv[])
{
    milog::Logger::logger().logLevel( milog::FATAL );

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
