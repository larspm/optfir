/* Document example:
 *  <amplitudespec order="7" symmetric="yes" max_coeff_abs_value="123.45" max_gain="1.0">
 *    <sample omega="0.0" gain="1.0"/>
 *    <sample omega="0.5" gain="0.5"/>
 *      .
 *      .
 *      .
 *  </amplitudespec>
 */

#ifndef SPECPARSER_H
#define SPECPARSER_H

#include "filterspec.h"

namespace optfir
{
  FilterSpec parseSpecFile(const char* filename);
}

#endif
