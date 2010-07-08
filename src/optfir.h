#ifndef OPTFIR_H
#define OPTFIR_H

#include "filterspec.h"

namespace optfir
{
  class FirFilter;

  double L2Norm(const FilterSpec& x, FirFilter& y);
  bool findClosestFilter(const FilterSpec spec, FirFilter& filter, bool forcedDc, bool optForRatio, std::string* resultText = 0);
}

#endif
