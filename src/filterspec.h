#ifndef FILTERSPEC_H
#define FILTERSPEC_H

#include <map>

struct SampleSpec {
  double gain;
  double weight;
  double maxDeviation;
};

typedef std::map<double, SampleSpec> SampleSpecMap;

struct FilterSpec {
  unsigned int order;
  bool symmetric;
  double maxCoeffAbsValue;
  SampleSpecMap samples;
};

#endif
