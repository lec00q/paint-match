/**
 * @brief Compute 2d feature descriptors from a set of images.
 *
 * @copyright Copyright 2013, Trya Srl
 * via Siemens 19 - 39100 Bolzano BZ, ITALY
 *
 * @author Piero Donaggio <piero.donaggio@trya.it>
 * @file ImageMatcher.cpp
 */

#include "ImageMatcher.h"

#include "opencv2/features2d/features2d.hpp"
#include <opencv2/nonfree/features2d.hpp>

using namespace cv;

ImageMatcher::ImageMatcher(int minHessian) :
    mMinHessian(minHessian)
{
}

void
ImageMatcher::ComputeDescriptors(const Mat &image, Mat &desc)
{
    std::vector<KeyPoint> keypoints;

    SurfFeatureDetector detector(mMinHessian);
    detector.detect(image, keypoints);

    SurfDescriptorExtractor extractor;
    extractor.compute(image, keypoints, desc);
}

void
ImageMatcher::AnalyzeDataset(const std::string &imageDirectory)
{
    bool done = false;
    mDescriptorVec.clear();

    mImageReader(imageDirectory);

    while (!done)
    {
        try
        {
            Mat image = mImageReader.LoadNextImage();

            Mat descriptors;
            ComputeDescriptors(image, descriptors);
            mDescriptorVec.push_back (descriptors);
        }
        catch (const ImageReaderIOException &ex)
        {
            done = true;
        }
    }
}

void
ImageMatcher::MatchImage(const std::string &fileName)
{
    Mat descriptors;
    std::vector<cv::DMatch> matches;

    Mat image = ImageReader::LoadImage(fileName);

    ComputeDescriptors(image, descriptors);

    cv::FlannBasedMatcher matcher;
    /// @todo Match with every image
    matcher.match(descriptors, mDescriptorVec[0], matches);
}

