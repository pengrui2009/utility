#include "ara/filesystem/filesystem.h"
#include "ara/core/string.h"
#include <gtest/gtest.h>

using namespace ara::filesystem;

TEST(Dir, CreatAndRemove) {
  ara::core::String dir = "/tmp/dir/ccc/ccc";
  ara::core::String dir2 = "/tmp/dir/aa/bb";

  EXPECT_TRUE(Filesystem::CreateDirectory(dir));
  EXPECT_TRUE(Filesystem::CreateDirectory(dir2));

  EXPECT_TRUE(Filesystem::Access(dir));
  EXPECT_TRUE(Filesystem::Access(dir2));

  EXPECT_TRUE(Filesystem::RemoveDirectory(dir));
  EXPECT_TRUE(Filesystem::RemoveDirectory(dir2));

  EXPECT_FALSE(Filesystem::Access(dir));
  EXPECT_FALSE(Filesystem::Access(dir2));
}

TEST(Dir, Rename) {
  ara::core::String source = "/tmp/dir/rename/aa";
  ara::core::String target = "/tmp/dir/rename/bb";

  Filesystem::RemoveDirectory(source);
  Filesystem::RemoveDirectory(target);

  EXPECT_TRUE(Filesystem::CreateDirectory(source));
  EXPECT_TRUE(Filesystem::Access(source));

  EXPECT_TRUE(Filesystem::Rename(source, target));
  EXPECT_TRUE(Filesystem::Access(target));
  EXPECT_FALSE(Filesystem::Access(source));
}

TEST(Dir, Copy) {
  ara::core::String source = "/tmp/dir/copy/aa";
  ara::core::String target = "/tmp/dir/copy/bb";

  Filesystem::RemoveDirectory(source);
  Filesystem::RemoveDirectory(target);

  EXPECT_TRUE(Filesystem::CreateDirectory(source));
  EXPECT_TRUE(Filesystem::Access(source));

  EXPECT_TRUE(Filesystem::CopyDirectory(source, target));
  EXPECT_TRUE(Filesystem::Access(target));
  EXPECT_TRUE(Filesystem::Access(source));
}

// TEST(Zip, Unzip) {
//   ara::core::String zip = "/tmp/dir/zip/aa.zip";
//   ara::core::String extract = "/tmp/dir/zip";

//   Filesystem::Unzip(zip, extract);
// }


int main(int argc, char** argv) {
  try {
    ::testing::InitGoogleTest(&argc, argv);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
  }
  return RUN_ALL_TESTS();
}