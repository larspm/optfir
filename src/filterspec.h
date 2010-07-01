#ifndef FILTERSPEC_H
#define FILTERSPEC_H

#include <map>

typedef std::map<double, double> SampleSpecMap;

struct FilterSpec {
  FilterSpec()
    : order(0)
    , symmetric(false)
    , maxCoeffAbsValue(-1)
    , maxGain(-1)
  {};
  unsigned int order;
  bool symmetric;
  double maxCoeffAbsValue;
  double maxGain;
  SampleSpecMap samples;
};

#endif
