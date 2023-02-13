#include "common/helpers.h"

#include <memory>
#include <vector>
#include <sstream>
#include <fstream>

int main()
{
  auto spv = generatePlusOneSPV();
  std::ofstream out("spv_gen.spv", std::ios::binary);
  out.write(spv.c_str(), spv.size());

  return 0;
}
