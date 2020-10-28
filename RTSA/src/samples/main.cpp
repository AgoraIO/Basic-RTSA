//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include <cstdio>

#include "gtest/gtest.h"
#if defined(WIN32)
#include <windows.h>
namespace agora {
namespace win32 {
HMODULE g_hInstDll = nullptr;
}
}  // namespace agora
#endif

GTEST_API_ int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  return result;
}
