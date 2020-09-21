/*
 * (C) Copyright 2020 NOAA/NWS/NCEP/EMC
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "IodaDescription.h"

static const char* FILENAME_SECTION = "filename";
static const char* DIMENSIONS_SECTION = "dimensions";
static const char* VARIABLES_SECTION = "variables";

static const char* SCALE_NAME = "name";
static const char* SCALE_SIZE = "size";

static const char* VARIABLE_NAME = "name";
static const char* VARIABLE_SOURCE = "source";
static const char* VARIABLE_SCALES = "dimensions";
static const char* VARIABLE_LONGNAME = "longName";
static const char* VARIABLE_UNITS = "units";
static const char* VARIABLE_RANGE = "range";
static const char* VARIABLE_COORDS = "coordinates";


namespace Ingester
{
    IodaDescription::IodaDescription(const eckit::Configuration& conf, const std::string& basepath)
    {
        if (conf.has(FILENAME_SECTION) && !basepath.empty())
        {
            filepath_ = basepath + "/" + conf.getString(FILENAME_SECTION);
        }
        else
        {
            filepath_ = "";
        }

        for (const auto& scaleConf : conf.getSubConfigurations(DIMENSIONS_SECTION))
        {
            ScaleDescription scale;
            scale.name = scaleConf.getString(SCALE_NAME);
            scale.size = scaleConf.getString(SCALE_SIZE);

            addScale(scale);
        }

        for (const auto& varConf : conf.getSubConfigurations(VARIABLES_SECTION))
        {
            VariableDescription variable;
            variable.name = varConf.getString(VARIABLE_NAME);
            variable.source = varConf.getString(VARIABLE_SOURCE);
            variable.dimensions = varConf.getStringVector(VARIABLE_SCALES);
            variable.longName = varConf.getString(VARIABLE_LONGNAME);
            variable.units = varConf.getString(VARIABLE_UNITS);

            if (varConf.has(VARIABLE_COORDS))
            {
                variable.coordinates =
                    std::make_shared<std::string> (varConf.getString(VARIABLE_COORDS));
            }
            else
            {
                variable.coordinates = nullptr;
            }

            if (varConf.has(VARIABLE_RANGE))
            {
                auto range = std::make_shared<Range>();
                range->start = std::stoi(varConf.getStringVector(VARIABLE_RANGE)[0]);
                range->end = std::stoi(varConf.getStringVector(VARIABLE_RANGE)[1]);
                variable.range = range;
            }
            else
            {
                variable.range = nullptr;
            }

            addVariable(variable);
        }
    }

    void IodaDescription::addScale(ScaleDescription scale)
    {
        dimensions_.push_back(scale);
    }

    void IodaDescription::addVariable(VariableDescription variable)
    {
        variables_.push_back(variable);
    }
}  // namespace Ingester