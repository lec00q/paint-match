#include "ImageMatcher.h"
#include "ImageReader.h"
#include "config.h"

#include <gtest/gtest.h>

TEST(ImageMatcherTest, FindBestMatch)
{
    std::string trainingDir(TRAINING_DIR);
    std::string queryDir(QUERY_DIR);

    ImageReader reader(queryDir);
    std::vector<std::string> queryNames = reader.GetFileNames();

    reader(trainingDir);
    std::vector<std::string> trainNames = reader.GetFileNames();

    ImageMatcher matcher;
    matcher.Train(trainingDir);

    for (int i = 0; i < queryNames.size(); ++i)
    {
        std::string matchName = matcher.FindBestMatch(queryDir + queryNames[i]);
        ASSERT_STREQ (trainNames[i].c_str(), matchName.c_str());
    }
}

