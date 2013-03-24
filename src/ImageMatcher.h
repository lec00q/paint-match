/**
 * @brief Compute 2d feature descriptors from a set of images.
 *
 * @copyright Copyright 2013, Trya Srl
 * via Siemens 19 - 39100 Bolzano BZ, ITALY
 *
 * @author Piero Donaggio <piero.donaggio@trya.it>
 * @file ImageMatcher.h
 */

#ifndef dataset_analyzer_h
#define dataset_analyzer_h

#include "ImageReader.h"

#include <vector>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

class ImageMatcher
{
public:

    ImageMatcher(int minHessian = 400);

    /**
     * @brief Train the classifier by feeding a dataset of images.
     */
    void Train(const std::string &imageDirectory);

    /**
     * @brief Find the best match among the images in the dataset.
     * @note The classifier must be trained.
     */
    const std::string FindBestMatch(const std::string &fileName);

    const std::string FindBestMatch(const std::string &fileName,
                                    float &confidence);

    const std::string MatchImageDebug (const std::string &imageDirectory,
                                       const std::string &fileName);

private:

    void ComputeDescriptors(const cv::Mat &image, cv::Mat &desc);
    void ComputeDescriptors(const cv::Mat &image, cv::Mat &desc,
                            std::vector<cv::KeyPoint> &keypoints);

    float HomographyMatching(const cv::Mat &objDescriptors,
                             const cv::Mat &sceneDescriptors,
                             const std::vector<cv::KeyPoint> &objKeypoints,
                             const std::vector<cv::KeyPoint> &sceneKeypoints,
                             cv::Mat &homography);

    ImageReader mImageReader;

    cv::FlannBasedMatcher mMatcher;
<<<<<<< HEAD
=======
//    cv::BFMatcher mMatcher;
>>>>>>> 91d97a6... Confidence is computed as a ratio

    std::vector<std::string> mFileNames;
    std::vector<cv::Mat> mTrainDescriptors;
    std::vector<std::vector<cv::KeyPoint> > mTrainKeypoints;

    int mMinHessian;
};

#endif

