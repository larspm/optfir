#include <cmath>
#include <iostream>
#include <stdexcept>
#include <numeric>

#include <nlopt.hpp>

#include "optfir.h"
#include "filters.h"

namespace optfir
{
  double L2Norm(const FilterSpec& x, const FirFilter& y)
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

  double ratioDistance(const FilterSpec& x, const FirFilter& y)
  {
    double d(0);

    for (SampleSpecMap::const_iterator it = x.samples.begin();
         it != x.samples.end();
         it++)
    {
      d += pow(log(std::abs(it->second)) - log(y.getAmplitudeResponse(it->first)), 2);
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

  double objective_ratio(const std::vector<double>& x, std::vector<double>& grad, void* f_data)
  {
    FirFilter& filter = reinterpret_cast<ObjectiveData*>(f_data)->filter;
    filter = x;

    const FilterSpec& spec = reinterpret_cast<ObjectiveData*>(f_data)->spec;

    double obj = ratioDistance(spec, filter);

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

  struct MaxGainConstraintData
  {
    const FilterSpec& spec;
    const FirFilter& filter;
  };

  double constraint_max_gain(const std::vector<double>& x, std::vector<double>& grad, void *fc_data)
  {
    const FilterSpec& spec = reinterpret_cast<const MaxGainConstraintData*>(fc_data)->spec;
    const FirFilter& filter = reinterpret_cast<const MaxGainConstraintData*>(fc_data)->filter;

    double maxGain = 0.0;
    for (SampleSpecMap::const_iterator it = spec.samples.begin();
         it != spec.samples.end();
         it++)
    {
      double temp = filter.getAmplitudeResponse(it->first);
      if (temp > maxGain)
      {
        maxGain = temp;
      }
    }

    return maxGain - spec.maxGain;
  }

  bool findClosestFilter(const FilterSpec spec, FirFilter& filter, bool forcedDc, bool optForRatio, std::string* resultText)
  {
    const int N = filter.getRank();

    nlopt::opt opt(nlopt::LN_COBYLA, N);

    if (spec.maxCoeffAbsValue > 0.0)
    {
      std::vector<double> lb(N, -fabs(spec.maxCoeffAbsValue));
      std::vector<double> ub(N, +fabs(spec.maxCoeffAbsValue));
      opt.set_lower_bounds(lb);
      opt.set_upper_bounds(ub);
    }

    ObjectiveData objectiveData = { spec, filter};
    opt.set_min_objective(optForRatio ? objective_ratio : objective, &objectiveData);

    FdcConstraintData fdcConstaints[] = { {1, filter.getAmplitudeResponse(0.0)}, {2, filter.getAmplitudeResponse(0.0)} };
    if (forcedDc)
    {
      opt.add_inequality_constraint(constraint_fdc, &fdcConstaints[0], 0.0);
      opt.add_inequality_constraint(constraint_fdc, &fdcConstaints[1], 0.0);
    }

    MaxGainConstraintData mgConstraint = { spec, filter};
    if (spec.maxGain > 0.0)
    {
      opt.add_inequality_constraint(constraint_max_gain, &mgConstraint, 0.0);
    }

    std::vector<double>  x(N, 0.0);
    for (int i = 0; i < N; i++)
    {
      x[i]  = filter[i];
    }

    opt.set_maxeval(5000);
    opt.set_xtol_abs(1e-10);

    double minf;
    nlopt::result result = nlopt::SUCCESS;
    try
    {
      result = opt.optimize(x, minf);
    }
    catch (std::runtime_error& e)
    {
      std::cerr << e.what() << std::endl;
      result = nlopt::FAILURE;
    }
    catch (...)
    {
      std::cerr << "something went wrong\n";
      result = nlopt::FAILURE;
    }

    filter = x;

    if (resultText != 0)
    {
      using namespace nlopt;
      switch (result)
      {
        case FAILURE:          *resultText = "FAILURE";          break;
        case INVALID_ARGS:     *resultText = "INVALID_ARGS";     break;
        case OUT_OF_MEMORY:    *resultText = "OUT_OF_MEMORY";    break;
        case ROUNDOFF_LIMITED: *resultText = "ROUNDOFF_LIMITED"; break;
        case FORCED_STOP:      *resultText = "FORCED_STOP";      break;
        case SUCCESS:          *resultText = "SUCCESS";          break;
        case STOPVAL_REACHED:  *resultText = "STOPVAL_REACHED";  break;
        case FTOL_REACHED:     *resultText = "FTOL_REACHED";     break;
        case XTOL_REACHED:     *resultText = "XTOL_REACHED";     break;
        case MAXEVAL_REACHED:  *resultText = "MAXEVAL_REACHED";  break;
        case MAXTIME_REACHED:  *resultText = "MAXTIME_REACHED";  break;
        default: break;
      }
    }

    return result > 0;
  }
}

