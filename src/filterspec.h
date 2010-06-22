#ifndef FILTERSPEC_H
#define FILTERSPEC_H

#include <map>

typedef std::map<double, double> SampleSpecMap;

struct FilterSpec {
  unsigned int order;
  bool symmetric;
  double maxCoeffAbsValue;
  SampleSpecMap samples;
};

#endif
