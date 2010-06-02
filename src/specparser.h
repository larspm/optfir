/* Document example:
 *  <amplitudespec order="7" symmetric="true" max_coeff_abs_value="123.45">
 *    <sample omega="0.0" gain="1.0" weight="1.0" max_deviation="0.1"/>
 *    <sample omega="0.5" gain="0.5" weight="1.0" max_deviation="0.1"/>
 *      .
 *      .
 *      .
 *  </amplitudespec>
 */

#ifndef SPECPARSER_H
#define SPECPARSER_H

#include "filterspec.h"

FilterSpec parseSpecFile(const char* filename);

#endif
