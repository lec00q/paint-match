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
#include <boost/filesystem.hpp>

#include "ImageMatcher.h"

int
main (int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "\n\tUsage: " << argv[0] << " <trainingDir> <queryImage>\n\n";
        return (EXIT_FAILURE);
    }

    float confidence;
    std::string datasetDir(argv[1]);
    std::string queryImage(argv[2]);

    ImageMatcher matcher;

    namespace fs = boost::filesystem;
    if (!(fs::exists(queryImage) && fs::is_regular_file(queryImage)))
    {
        std::cout << queryImage << " is not a valid image file\n";
        return (EXIT_FAILURE);
    }

    std::cout << "Analyzing the whole dataset..." << std::endl;
    matcher.Train(datasetDir);
    std::cout << "Done!" << std::endl << std::endl;

    std::cout << "Searching best match..." << std::endl;
    std::string matchName = matcher.FindBestMatch(queryImage, confidence);
    std::cout << "Best match found: " << matchName << std::endl;
    std::cout << "Confidence: " << confidence << std::endl << std::endl;

    return (EXIT_SUCCESS);
}

