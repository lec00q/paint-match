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
#include <sstream>
#include <algorithm>
#include <numeric>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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
ImageMatcher::Train (const std::string &imageDirectory)
{
    bool done = false;

    mImageReader(imageDirectory);
    mFileNames = mImageReader.GetFileNames();

    mMatcher.clear();

    while (!done)
    {
        try
        {
            Mat image = mImageReader.LoadNextImage();

            Mat descriptors;
            std::vector<KeyPoint> keypoints;

            ComputeDescriptors(image, descriptors, keypoints);

            mTrainDescriptors.push_back(descriptors);
            mTrainKeypoints.push_back(keypoints);
        }
        catch (const ImageReaderIOException &ex)
        {
            done = true;
        }
    }
}

bool CompFunc (float val, DMatch d)
{
    return (val < d.distance);
}

float sumFunc (float val, DMatch d)
{
    return (val + d.distance);
}

float
ImageMatcher::HomographyMatching(const Mat &objDescriptors,
                                 const Mat &sceneDescriptors,
                                 const std::vector<KeyPoint> &objKeypoints,
                                 const std::vector<KeyPoint> &sceneKeypoints,
                                 Mat &homography)
{
    typedef std::vector<cv::DMatch>::iterator DMatchIt;
    std::vector<DMatch> filteredMatches, matches12, matches21;
    std::vector<Point2f> obj;
    std::vector<Point2f> scene;

    mMatcher.match(objDescriptors, sceneDescriptors, matches12);
    mMatcher.match(sceneDescriptors, objDescriptors, matches21);

    // Cross-validation
    for (size_t i = 0; i < matches12.size(); i++)
    {
        DMatch forward = matches12[i];
        DMatch backward = matches21[forward.trainIdx];
        if (backward.trainIdx == forward.queryIdx)
        {
            filteredMatches.push_back(forward);
            obj.push_back (objKeypoints[forward.queryIdx].pt);
            scene.push_back (sceneKeypoints[forward.trainIdx].pt);
        }
    }

    float meanDistance = 100;

    if (obj.size() >= 4)
    {
        Mat mask;
        meanDistance = 0;

        // Compute homography and retrieve inliers
        homography = findHomography (obj, scene, CV_RANSAC, 3, mask);

        // Compute mean distance for inliers only
        int *maskPtr = mask.ptr<int>(0);
        for(int i = 0; i < filteredMatches.size(); i++)
        {
            if (maskPtr)
                meanDistance += filteredMatches[i].distance;

            maskPtr++;
        }

        meanDistance /= countNonZero(mask);
    }

    return meanDistance;
}

const std::string
ImageMatcher::FindBestMatch(const std::string &fileName)
{
    float c;
    return (FindBestMatch(fileName, c));
}

const std::string
ImageMatcher::FindBestMatch(const std::string &fileName,
                            float &confidence)
{
    typedef std::vector<cv::DMatch>::iterator DMatchIt;
    typedef std::vector<cv::Mat>::iterator descIt;

    std::string result("No match found");

    Mat queryDesc;
    std::vector<KeyPoint> queryKey;
    std::vector<float> distances;

    // Safety load the query image
    Mat image = ImageReader::LoadImage(fileName);

    // Compute keypoints for query image
    ComputeDescriptors(image, queryDesc, queryKey);

    // Match against every image in the set
    for (int i = 0; i < mTrainDescriptors.size(); ++i)
    {
        cv::Mat H;

        float dist = HomographyMatching(queryDesc, mTrainDescriptors[i],
                                        queryKey, mTrainKeypoints[i], H);

        distances.push_back(dist);

#ifdef SHOW_WARPED
        // Reload image in the dataset
        Mat trainImage = mImageReader.LoadImage(i);
        try
        {
            Mat warped;
            warpPerspective(image, warped, H, trainImage.size());
            cv::namedWindow("warped", CV_WINDOW_KEEPRATIO);
            cv::imshow("warped", warped);
            cv::waitKey(0);
        }
        catch (const cv::Exception &ex) {}
#endif
    }

    std::vector<float>::iterator best =
        std::min_element (distances.begin(), distances.end());

    if (best != distances.end())
        result = mFileNames[best - distances.begin()];

    // Compute second best match in order to compare the different algos
    float real_best = *best;
    *best = 100;
    std::vector<float>::iterator second_best =
        std::min_element (distances.begin(), distances.end());

    confidence = *second_best - real_best;

    return result;
}

const std::string
ImageMatcher::MatchImageDebug (const std::string &imageDirectory,
                               const std::string &fileName)
{
    bool done = false;
    typedef std::vector<cv::DMatch>::iterator DMatchIt;
    typedef std::vector<cv::Mat>::iterator descIt;

    mTrainDescriptors.clear();

    mImageReader(imageDirectory);
    mFileNames = mImageReader.GetFileNames();

    cv::FlannBasedMatcher matcher;

    Mat image = ImageReader::LoadImage(fileName);

    Mat descriptors;
    std::vector<KeyPoint> keypoints;
    ComputeDescriptors(image, descriptors, keypoints);

    int count = 0;
    while (!done)
    {
        try
        {
            std::cout << "match image " << count << std::endl;
            Mat image2 = mImageReader.LoadNextImage();

            Mat descriptors2;
            std::vector<KeyPoint> keypoints2;
            ComputeDescriptors(image2, descriptors2, keypoints2);

//            std::vector<cv::DMatch> matches;
//            matcher.match(descriptors2, descriptors, matches);

            std::vector<DMatch> matches, matches12, matches21;
            matcher.match(descriptors2, descriptors, matches12);
            matcher.match(descriptors, descriptors2, matches21);
            for (size_t i = 0; i < matches12.size(); i++)
            {
                DMatch forward = matches12[i];
                DMatch backward = matches21[forward.trainIdx];
                if (backward.trainIdx == forward.queryIdx)
                    matches.push_back(forward);
            }

            std::sort (matches.begin(), matches.end());

            /**************************************************/
            std::vector<Point2f> obj;
            std::vector<Point2f> scene;
            DMatchIt dIt = matches.begin();

            for (; dIt != matches.end(); ++dIt)
            {
                obj.push_back (keypoints2[dIt->queryIdx].pt);
                scene.push_back (keypoints[dIt->trainIdx].pt);

                if ((0.01 + dIt->distance) > 3 * (0.01 + matches[0].distance))
                    break;
            }

            std::vector<DMatch> good_matches (matches.begin(), dIt);

            if (good_matches.size() >= 4)
            {
                Mat H = findHomography(obj, scene, CV_RANSAC );

                //-- Draw only "good" matches
                Mat img_matches;
                drawMatches(image2, keypoints2, image, keypoints,
                            good_matches, img_matches, Scalar::all(-1),
                            Scalar::all(-1), std::vector<char>(),
                            DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

                //-- Get the corners from the image_1 ( the object to be "detected" )
                std::vector<Point2f> obj_corners(4);
                obj_corners[0] = cvPoint(0,0);
                obj_corners[1] = cvPoint(image2.cols, 0 );
                obj_corners[2] = cvPoint(image2.cols, image2.rows);
                obj_corners[3] = cvPoint(0, image2.rows);
                std::vector<Point2f> scene_corners(4);

                try
                {
                    perspectiveTransform( obj_corners, scene_corners, H);
                    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
                    line( img_matches, scene_corners[0] + Point2f( image2.cols, 0),
                          scene_corners[1] + Point2f( image2.cols, 0), Scalar(0, 255, 0), 4 );
                    line( img_matches, scene_corners[1] + Point2f( image2.cols, 0),
                          scene_corners[2] + Point2f( image2.cols, 0), Scalar( 0, 255, 0), 4 );
                    line( img_matches, scene_corners[2] + Point2f( image2.cols, 0),
                          scene_corners[3] + Point2f( image2.cols, 0), Scalar( 0, 255, 0), 4 );
                    line( img_matches, scene_corners[3] + Point2f( image2.cols, 0),
                          scene_corners[0] + Point2f( image2.cols, 0), Scalar( 0, 255, 0), 4 );

                    //-- Show detected matches
//            cv::namedWindow("Good Matches", CV_WINDOW_KEEPRATIO);
//            cv::imshow("Good Matches", img_matches);
//            cv::waitKey(0);

                    std::ostringstream ss;
                    ss << "image" << count << ".jpg";
                    cv::imwrite(ss.str(), img_matches);
                }
                catch (const cv::Exception &ex)
                {
                    std::cerr << ex.what() << std::endl;
                }

            }

            count++;
        }
        catch (const ImageReaderIOException &ex)
        {
            done = true;
        }
    }

    std::string result;
    return result;
}



