/**
 * @brief Fisherman mini-project
 *
 * @copyright Copyright 2013, Trya Srl
 * via Siemens 19 - 39100 Bolzano BZ, ITALY
 *
 * @author Piero Donaggio <piero.donaggio@trya.it>
 * @file main.cpp
 */


#include <cstdlib>
#include <iostream>
#include <string.h>
#include <opencv2/core/core.hpp>

#include "ImageMatcher.h"
#include "ImageReader.h"

int
main (int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "\tUsage: " << argv[0] << " <trainingDir> <queryDir>\n\n";
        return (EXIT_FAILURE);
    }

    std::string datasetDir(argv[1]);
    std::string queryDir(argv[2]);

    ImageReader reader(queryDir);
    std::vector<std::string> queryNames = reader.GetFileNames();
    ImageMatcher matcher;

    std::cout << "Analyzing the whole dataset..." << std::endl;
    matcher.Train(datasetDir);
    std::cout << "Done!" << std::endl << std::endl;

//    std::string matchName = matcher.FindBestMatch(queryDir + queryNames[4]);

    for (std::vector<std::string>::iterator it = queryNames.begin();
            it != queryNames.end(); ++it)
    {
        std::cout << "Searching best match for image " << *it << "..." << std::endl;
        std::string matchName = matcher.FindBestMatch(queryDir + *it);
        std::cout << "Best match found: " << matchName << std::endl << std::endl;
    }

    return (EXIT_SUCCESS);
}

