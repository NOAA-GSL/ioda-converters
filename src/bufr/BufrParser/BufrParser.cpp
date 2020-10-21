/*
 * (C) Copyright 2020 NOAA/NWS/NCEP/EMC
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "BufrParser.h"

#include <iostream>
#include <map>

#include "bufr.interface.h"

#include "BufrParser/BufrCollectors/BufrCollectors.h"
#include "BufrMnemonicSet.h"
#include "DataContainer.h"

#include "Exports/MnemonicExport.h"
#include "Exports/DatetimeExport.h"
#include "Exports/Export.h"


namespace Ingester
{

    BufrParser::BufrParser(BufrDescription &description) :
        description_(description),
        fileUnit_(0)
    {
        reset();
    }

    BufrParser::~BufrParser()
    {
        closeBufrFile();
    }

    std::shared_ptr <DataContainer> BufrParser::parse(const size_t maxMsgsToParse)
    {
        const unsigned int SubsetStringLength = 25;

        assert(fileUnit_ > 0);

        auto collectors = BufrCollectors(fileUnit_);
        collectors.addMnemonicSets(description_.getMnemonicSets());

        char subset[SubsetStringLength];
        int iddate;

        unsigned int messageNum = 0;
        while (ireadmg_f(fileUnit_, subset, &iddate, SubsetStringLength) == 0)
        {
            while (ireadsb_f(fileUnit_) == 0)
            {
                collectors.collect();
            }

            if (maxMsgsToParse > 0 && ++messageNum >= maxMsgsToParse) break;
        }

        return exportData(collectors.finalize());
    }

    std::shared_ptr<DataContainer> BufrParser::exportData(const BufrDataMap& srcData)
    {
        auto outputData = std::make_shared<DataContainer>();
        auto exportMap = description_.getExportMap();

        unsigned int size = 0;
        auto exportIt = exportMap.begin();
        while (exportIt != exportMap.end())
        {
            auto key = exportIt->first;
            auto data_exporter = exportIt->second;

            if (!size) size = srcData.begin()->second.rows();

            outputData->add(key, data_exporter->exportData(srcData));

            exportIt++;
        }

        outputData->setSize(size);

        return outputData;
    }

    void BufrParser::openBufrFile(const std::string &filepath)
    {
        const unsigned int FortranFileId = 11;

        fileUnit_ = FortranFileId;
        open_f(fileUnit_, filepath.c_str());
        openbf_f(fileUnit_, "IN", fileUnit_);
    }

    void BufrParser::closeBufrFile()
    {
        closbf_f(fileUnit_);
        close_f(fileUnit_);
        fileUnit_ = 0;
    }

    void BufrParser::reset()
    {
        if (fileUnit_ != 0)
        {
            closeBufrFile();
        }

        openBufrFile(description_.filepath());
    }
}  // namespace Ingester