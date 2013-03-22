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
#include <glog/logging.h>
#include <opencv2/core/core.hpp>

#include "ImageMatcher.h"

int
main (int argc, char *argv[])
{
    if (argc < 3)
        /// @todo Print usage
        return (EXIT_FAILURE);

    ImageMatcher matcher;

    // Initialize Google's logging library.
    google::InitGoogleLogging(argv[0]);

    std::string datasetDir(argv[1]);
    std::string image(argv[2]);

    std::cout << "Analyzing whole dataset..." << std::endl;

    matcher.AnalyzeDataset(datasetDir);

    std::cout << "Match image " << image;

    matcher.MatchImage(image);

    return (EXIT_SUCCESS);
}
