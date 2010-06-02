#include <iostream>

#include "specparser.h"
#include "filters.h"
#include "optfir.h"

int main()
{
  FilterSpec spec = parseSpecFile("spec.xml");

  std::cerr << "order = " << spec.order << std::endl;
  std::cerr << "symmetric = " << (spec.symmetric?"yes":"no") << std::endl;
  std::cerr << "max_coeff = " << spec.maxCoeffAbsValue << std::endl;

  std::cerr << std::endl;

  for (SampleSpecMap::const_iterator it = spec.samples.begin();
       it != spec.samples.end();
       it++)
  {
    std::cerr << "H("
              << it->first
              << ") = "
              << it->second.gain
              << " (weight = "
              << it->second.weight
              << ", max_deviation = "
              << it->second.maxDeviation
              << ")\n";
  }

  FirFilter* filter = spec.symmetric ? (new SymmetricFirFilter(spec.order))
                                     : (new FirFilter(spec.order));

  bool result = findClosestFilter(spec, *filter);

  std::cerr << "\nResult(" << (result ? "ok" : "fail")  << "):\n\n";

  std::cout << "N = " << spec.order << std::endl;

  std::cout << "h = [ ";
  for (int i = 0; i < filter->getOrder(); i++)
  {
    std::cout << (*filter)[i] << " ";
  }
  std::cout << "]" << std::endl;


  std::cout << "w = [ ";
  for (SampleSpecMap::const_iterator it = spec.samples.begin();
       it != spec.samples.end();
       it++)
  {
    std::cout << it->first << " ";
  }
  std::cout << "]" << std::endl;

  std::cout << "H = [ ";
  for (SampleSpecMap::const_iterator it = spec.samples.begin();
       it != spec.samples.end();
       it++)
  {
    std::cout << filter->getAmplitudeResponse(it->first) << " ";
  }
  std::cout << "]" << std::endl;

  delete filter;

  return 0;
}
