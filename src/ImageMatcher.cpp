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

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/calib3d/calib3d.hpp"

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
ImageMatcher::ComputeDescriptors(const Mat &image, Mat &desc,
                                 std::vector<KeyPoint> &keypoints)
{
    SurfFeatureDetector detector(mMinHessian);
    detector.detect(image, keypoints);

    SurfDescriptorExtractor extractor;
    extractor.compute(image, keypoints, desc);
}

void
ImageMatcher::AnalyzeDataset(const std::string &imageDirectory)
{
    bool done = false;

    mImageReader(imageDirectory);
    mFileNames = mImageReader.GetFileNames();

    while (!done)
    {
        try
        {
            Mat image = mImageReader.LoadNextImage();

            Mat descriptors;
            ComputeDescriptors(image, descriptors);

            mDescriptorVec.push_back(descriptors);
        }
        catch (const ImageReaderIOException &ex)
        {
            done = true;
        }
    }
}

bool sortFunc (DMatch i, DMatch j)
{
    return (i.distance < j.distance);
}

const std::string
ImageMatcher::MatchImage(const std::string &fileName)
{
    typedef std::vector<cv::DMatch>::iterator DMatchIt;
    typedef std::vector<cv::Mat>::iterator descIt;

    cv::FlannBasedMatcher matcher;
    matcher.clear();
    matcher.add (mDescriptorVec);
    matcher.train ();

    Mat image = ImageReader::LoadImage(fileName);

    Mat descriptors;
    ComputeDescriptors(image, descriptors);

    std::vector<cv::DMatch> matches;
    // Match against every image in the set
    matcher.match(descriptors, matches);

    std::string result;

    /// @todo warning no match!
    result = mFileNames[matches[0].imgIdx];

    return result;
}

const std::string
ImageMatcher::MatchImageDebug (const std::string &imageDirectory,
                               const std::string &fileName)
{
    bool done = false;
    typedef std::vector<cv::DMatch>::iterator DMatchIt;
    typedef std::vector<cv::Mat>::iterator descIt;

    mDescriptorVec.clear();

    mImageReader(imageDirectory);
    mFileNames = mImageReader.GetFileNames();

    cv::FlannBasedMatcher matcher;

    Mat image = ImageReader::LoadImage(fileName);

    Mat descriptors;
    std::vector<KeyPoint> keypoints;
    ComputeDescriptors(image, descriptors, keypoints);

    while (!done)
    {
        try
        {
            Mat image2 = mImageReader.LoadNextImage();

            Mat descriptors2;
            std::vector<KeyPoint> keypoints2;
            ComputeDescriptors(image2, descriptors2, keypoints2);

            std::vector<cv::DMatch> matches;
            matcher.match(descriptors, descriptors2, matches);

            std::sort (matches.begin(), matches.end(), sortFunc);

            /**************************************************/
            std::vector<Point2f> obj;
            std::vector<Point2f> scene;
            DMatchIt dIt = matches.begin();

            for (; dIt != matches.end(); ++dIt)
            {
                obj.push_back (keypoints[dIt->queryIdx].pt);
                scene.push_back (keypoints2[dIt->trainIdx].pt);

                if ((0.01 + dIt->distance) > 3 * (0.01 + matches[0].distance))
                    break;
            }

            std::vector<DMatch> good_matches (matches.begin(), dIt);
            Mat H = findHomography(obj, scene, CV_RANSAC );

            //-- Draw only "good" matches
            Mat img_matches;
            drawMatches(image, keypoints, image2, keypoints2,
                        good_matches, img_matches, Scalar::all(-1),
                        Scalar::all(-1), std::vector<char>(),
                        DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

            //-- Get the corners from the image_1 ( the object to be "detected" )
            std::vector<Point2f> obj_corners(4);
            obj_corners[0] = cvPoint(0,0);
            obj_corners[1] = cvPoint( image.cols, 0 );
            obj_corners[2] = cvPoint( image.cols, image.rows );
            obj_corners[3] = cvPoint( 0, image.rows );
            std::vector<Point2f> scene_corners(4);

            perspectiveTransform( obj_corners, scene_corners, H);

            //-- Draw lines between the corners (the mapped object in the scene - image_2 )
            line( img_matches, scene_corners[0] + Point2f( image.cols, 0), scene_corners[1] + Point2f( image.cols, 0), Scalar(0, 255, 0), 4 );
            line( img_matches, scene_corners[1] + Point2f( image.cols, 0), scene_corners[2] + Point2f( image.cols, 0), Scalar( 0, 255, 0), 4 );
            line( img_matches, scene_corners[2] + Point2f( image.cols, 0), scene_corners[3] + Point2f( image.cols, 0), Scalar( 0, 255, 0), 4 );
            line( img_matches, scene_corners[3] + Point2f( image.cols, 0), scene_corners[0] + Point2f( image.cols, 0), Scalar( 0, 255, 0), 4 );

            //-- Show detected matches
            cv::imshow("Good Matches", img_matches);

            cv::waitKey(0);
        }
        catch (const ImageReaderIOException &ex)
        {
            done = true;
        }
    }

    std::string result;
    return result;
}


