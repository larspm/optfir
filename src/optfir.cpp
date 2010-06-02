#include <cmath>
#include <iostream>
#include <stdexcept>

#include <nlopt.h>

#include "optfir.h"
#include "filters.h"

double L2Norm(const FilterSpec& x, FirFilter& y)
{
  double d(0);

  for (SampleSpecMap::const_iterator it = x.samples.begin();
       it != x.samples.end();
       it++)
  {
    d += pow(std::abs(it->second.gain) - y.getAmplitudeResponse(it->first), 2) * it->second.weight;
  }

  return sqrt(d);
}

struct ObjectiveData {
  const FilterSpec& spec;
  FirFilter& filter;
};

double objective(int n, const double* x, double* grad, void* f_data)
{
  FirFilter& filter = reinterpret_cast<ObjectiveData*>(f_data)->filter;
  filter = x;

  const FilterSpec& spec = reinterpret_cast<ObjectiveData*>(f_data)->spec;

  double obj = L2Norm(spec, filter);

  if (grad != 0)
  {
    throw std::runtime_error("gradient not implemented with weights yet");
    for (int i = 0; i < n; i++)
    {
      grad[i] = 0;
      for (SampleSpecMap::const_iterator k = spec.samples.begin();
           k != spec.samples.end();
           k++)
      {
        grad[i] += (filter.getAmplitudeResponse(k->first) - k->second.gain) * filter.getGradient(k->first)[i];
      }
      grad[i] /= obj;
    }
  }

  return obj;
}

bool findClosestFilter(const FilterSpec spec, FirFilter& filter)
{
  const int N = filter.getRank();

  double* x  = new double[N];
  double* lb = new double[N];
  double* ub = new double[N];
  double min_obj = 0;

  for (int i = 0; i < N; i++)
  {
    x[i]  = 0;
    lb[i] = -700;
    ub[i] = +700;
  }

  ObjectiveData objectiveData = { spec, filter};

  nlopt_result result =
  nlopt_minimize_constrained(NLOPT_LN_COBYLA,
                             N,
                             objective,
                             &objectiveData,
                             0,
                             NULL,
                             NULL,
                             NULL,
                             lb,
                             ub,
                             x,
                             &min_obj,
                             -HUGE_VAL,
                             0.0,
                             0.0,
                             0.0,
                             NULL,
                             1000,
                             0.0);

  filter = x;

  delete[] x;
  delete[] lb;
  delete[] ub;

  return result > 0;
}
