#include <cmath>
#include <iostream>
#include <stdexcept>
#include <numeric>

#include <nlopt.hpp>

#include "optfir.h"
#include "filters.h"

double L2Norm(const FilterSpec& x, FirFilter& y)
{
  double d(0);

  for (SampleSpecMap::const_iterator it = x.samples.begin();
       it != x.samples.end();
       it++)
  {
    d += pow(std::abs(it->second) - y.getAmplitudeResponse(it->first), 2);
  }

  return sqrt(d);
}

struct ObjectiveData {
  const FilterSpec& spec;
  FirFilter& filter;
};

double objective(const std::vector<double>& x, std::vector<double>& grad, void* f_data)
{
  FirFilter& filter = reinterpret_cast<ObjectiveData*>(f_data)->filter;
  filter = x;

  const FilterSpec& spec = reinterpret_cast<ObjectiveData*>(f_data)->spec;

  double obj = L2Norm(spec, filter);

  return obj;
}

struct FdcConstraintData
{
  int constraint_number;
  double dc_gain;
};

double constraint_fdc(const std::vector<double>& x, std::vector<double>& grad, void *fc_data)
{
  FdcConstraintData* data = (FdcConstraintData*)fc_data;

  return (data->constraint_number==1) ? (+data->dc_gain - 2.0 * std::accumulate(x.begin(), x.end(), 0.0) - 1.0)
                                      : (-data->dc_gain + 2.0 * std::accumulate(x.begin(), x.end(), 0.0) - 1.0);
}

bool findClosestFilter(const FilterSpec spec, FirFilter& filter, bool forcedDc)
{
  const int N = filter.getRank();

  nlopt::opt opt(nlopt::LN_COBYLA, N);

  std::vector<double> lb(N, -fabs(spec.maxCoeffAbsValue));
  std::vector<double> ub(N, +fabs(spec.maxCoeffAbsValue));
  opt.set_lower_bounds(lb);
  opt.set_upper_bounds(ub);

  ObjectiveData objectiveData = { spec, filter};
  opt.set_min_objective(objective, &objectiveData);

  FdcConstraintData fdcConstaints[] = { {1, filter.getAmplitudeResponse(0.0)}, {2, filter.getAmplitudeResponse(0.0)} };
  if (forcedDc)
  {
    opt.add_inequality_constraint(constraint_fdc, &fdcConstaints[0], 0.0);
    opt.add_inequality_constraint(constraint_fdc, &fdcConstaints[1], 0.0);
  }

  std::vector<double>  x(N, 0);
  for (int i = 0; i < N; i++)
  {
    x[i]  = filter[i];
  }

  opt.set_maxeval(1000);

  double minf;
  nlopt::result result = nlopt::SUCCESS;
  try
  {
    result = opt.optimize(x, minf);
  }
  catch (std::runtime_error& e)
  {
    std::cerr << e.what();
  }
  catch (...)
  {
    std::cerr << "something went wrong\n";
    result = nlopt::FAILURE;
  }

  filter = x;

  return result > 0;
}

