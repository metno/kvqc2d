
#include <milog/milog.h>
#include <gtest/gtest.h>

int main(int argc, char* argv[])
{
#if 0
    milog::Logger::logger().logLevel( milog::DEBUG );
#else
    milog::Logger::logger().logLevel( milog::FATAL );
#endif

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
