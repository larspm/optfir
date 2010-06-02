#include <string>

#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "specparser.h"

xmlXPathObjectPtr getNodeSet (xmlDocPtr doc, xmlChar *xpath)
{
  xmlXPathContextPtr context;
  xmlXPathObjectPtr result;

  context = xmlXPathNewContext(doc);

  if (context == NULL)
  {
    return NULL;
  }

  result = xmlXPathEvalExpression(xpath, context);
  xmlXPathFreeContext(context);

  if (result == NULL)
  {
    return NULL;
  }

  if(xmlXPathNodeSetIsEmpty(result->nodesetval))
  {
    xmlXPathFreeObject(result);
    return NULL;
  }

  return result;
}

FilterSpec parseSpecFile(const char* filename)
{
  FilterSpec spec;

  xmlDocPtr doc;
  doc = xmlParseFile(filename);

  if (doc == NULL)
  {
    throw 1;
  }

  xmlXPathObjectPtr result = getNodeSet(doc, BAD_CAST "/amplitudespec");

  if(result != NULL)
  {
    xmlNodeSetPtr nodeset = result->nodesetval;

    xmlChar* temp;
    temp = xmlGetProp(nodeset->nodeTab[0], BAD_CAST "order");
    if (temp != NULL)
    {
      spec.order = atoi((const char*)temp);
    }

    temp = xmlGetProp(nodeset->nodeTab[0], BAD_CAST "symmetric");
    if (temp != NULL)
    {
      spec.symmetric = std::string("yes") == (const char*)temp;
    }

    temp = xmlGetProp(nodeset->nodeTab[0], BAD_CAST "max_coeff_abs_value");
    if (temp != NULL)
    {
      spec.maxCoeffAbsValue = atof((const char*)temp);
    }

		xmlXPathFreeObject(result);

    result = getNodeSet(doc, BAD_CAST "/amplitudespec/sample");
    if (result != NULL)
    {
      nodeset = result->nodesetval;
      for (int i=0; i < nodeset->nodeNr; i++)
      {
        if ((temp = xmlGetProp(nodeset->nodeTab[i], BAD_CAST "omega")) != NULL)
        {
          SampleSpec& sample = spec.samples[atof((const char*)temp)];

          temp = xmlGetProp(nodeset->nodeTab[i], BAD_CAST "gain");
          if (temp != NULL)
          {
            sample.gain = atof((const char*)temp);
          }
          temp = xmlGetProp(nodeset->nodeTab[i], BAD_CAST "weight");
          if (temp != NULL)
          {
            sample.weight = atof((const char*)temp);
          }
          temp = xmlGetProp(nodeset->nodeTab[i], BAD_CAST "max_deviation");
          if (temp != NULL)
          {
            sample.maxDeviation = atof((const char*)temp);
          }
        }
      }
     xmlXPathFreeObject(result);
    }
  }

  xmlFreeDoc(doc);
  xmlCleanupParser();
  return spec;
}