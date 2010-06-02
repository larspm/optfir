#ifndef OPTFIR_H
#define OPTFIR_H

#include "filterspec.h"

class FirFilter;

double L2Norm(const FilterSpec& x, FirFilter& y);
bool   findClosestFilter(const FilterSpec spec, FirFilter& filter);

#endif
