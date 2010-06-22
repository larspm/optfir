#include <iostream>

#include "specparser.h"
#include "filters.h"
#include "optfir.h"

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " filename\n";
    return 1;
  }

  FilterSpec spec;
  try {
     spec = parseSpecFile(argv[1]);
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  std::cerr << "order = " << spec.order << std::endl;
  std::cerr << "symmetric = " << (spec.symmetric?"yes":"no") << std::endl;
  std::cerr << "max_coeff = " << spec.maxCoeffAbsValue << std::endl;

  if(!spec.symmetric)
  {
    std::cerr << "Error: only symmetric filters are implemented, quitting \n";
    return 1;
  }

  std::cerr << std::endl;

  for (SampleSpecMap::const_iterator it = spec.samples.begin();
       it != spec.samples.end();
       it++)
  {
    std::cerr << "H("
              << it->first
              << ") = "
              << it->second
              << ")\n";
  }

  FirFilter* filter = 0;

  if (spec.symmetric)
  {
    if (spec.samples.count(0.0) > 0)
    {
      // TODO: this should be an option.
      std::cerr << "Using forced DC response\n";
      filter = new SymmetricFirFilterForcedDc(spec.samples[0.0], spec.order);
    }
    else
    {
      filter = new SymmetricFirFilter(spec.order);
    }
  }
  else
  {
      filter = new FirFilter(spec.order);
  }
  bool result = findClosestFilter(spec, *filter, spec.samples.count(0.0) > 0);

  std::cerr << "\nResult(" << (result ? "ok" : "fail")  << "):\n\n";

  std::cout << "N = " << spec.order << std::endl;

  std::cout << "w = [ ";
  for (SampleSpecMap::const_iterator it = spec.samples.begin();
       it != spec.samples.end();
       it++)
  {
    std::cout << it->first << " ";
  }
  std::cout << "]'" << std::endl;

  std::cout << "H_opt = [ ";
  for (SampleSpecMap::const_iterator it = spec.samples.begin();
       it != spec.samples.end();
       it++)
  {
    std::cout << it->second << " ";
  }
  std::cout << "]'" << std::endl;

  std::cout << "h_nlopt = [ ";
  for (unsigned int i = 0; i < filter->getOrder(); i++)
  {
    std::cout << (*filter)[i] << " ";
  }
  std::cout << "]'" << std::endl;

  std::cout << "H_nlopt = [ ";
  for (SampleSpecMap::const_iterator it = spec.samples.begin();
       it != spec.samples.end();
       it++)
  {
    std::cout << filter->getAmplitudeResponse(it->first) << " ";
  }
  std::cout << "]'" << std::endl;

  std::cout <<
  "A = [];\n"
  "for j = 1:((N + 1) / 2 - 1 )\n"
  "  A = [A 2*cos( ((N+1)/2 -j ) * w)];\n"
  "endfor\n"
  "A = [A ones(length(w), 1)];\n"
  "h_pls = inv(A' * A) * A' * H_opt\n";

  delete filter;

  return 0;
}
