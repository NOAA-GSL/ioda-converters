/*
 * (C) Copyright 2020 NOAA/NWS/NCEP/EMC
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "BufrParser/BufrTypes.h"
#include "DataObject/DataObject.h"

namespace iodaconv
{
    namespace parser
    {
        namespace bufr
        {
            /// \brief Abstract base class for all Exports.
            class Variable
            {
             public:
                virtual ~Variable() = default;

                /// \brief Variable data objects for previously parsed data from BufrDataMap.
                virtual std::shared_ptr<encoder::DataObject> exportData(const BufrDataMap& map) = 0;
            };
        }  // namespace bufr
    }  // namespace parser
}  // namespace iodaconv


