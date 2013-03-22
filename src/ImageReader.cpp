/**
 * @brief Acquisition library for images.
 *
 * @copyright Copyright 2013, Trya Srl
 * via Siemens 19 - 39100 Bolzano BZ, ITALY
 *
 * @author Piero Donaggio <piero.donaggio@trya.it>
 * @file ImageReader.cpp
 */

#include "ImageReader.h"

#include <algorithm>
#include <glog/logging.h>
#include <boost/filesystem.hpp>
#include <opencv2/highgui/highgui.hpp>

ImageReader::ImageReader() :
    mLastImageIndex(0)
{
    mFileNames.clear();
}

ImageReader::ImageReader(const std::string &imageDirectory) :
    mLastImageIndex(0)
{
    mFileNames.clear();

    (*this)(imageDirectory);
}

cv::Mat
ImageReader::LoadImage (const std::string &fileName)
{
    cv::Mat image;
    namespace fs = boost::filesystem;

    if (fs::exists(fileName) && fs::is_regular_file(fileName))
    {
        image = cv::imread(fileName, CV_LOAD_IMAGE_COLOR);
        if (!image.data)
            throw ImageReaderIOException("Canot load image: " + fileName);
    }
    else
    {
        throw ImageReaderIOException("No such file: " + fileName);
    }

    return image;
}

void
ImageReader::operator() (const std::string &imageDirectory)
{
    namespace fs = boost::filesystem;
    fs::path inputDir(imageDirectory);

    if (fs::exists(inputDir) && fs::is_directory(inputDir))
    {
        mFileNames.clear();
        fs::directory_iterator end, it(inputDir);

        mFilesPath = it->path().parent_path().string() + "/";
        LOG(INFO) << "Set image directory to: " << mFilesPath;

        for (; it != end; ++it)
        {
            // File types must be supported by OpenCV
            /// @todo There should be a bettwer way to check
            /// file type/extension
            if (it->path().extension() == ".tiff" ||
                    it->path().extension() == ".JPG" ||
                    it->path().extension() == ".jpg")
            {
                mFileNames.push_back(it->path().filename().string());
            }
        }

        LOG(INFO) << "Number of images in the directory: " << mFileNames.size();

        std::sort(mFileNames.begin(), mFileNames.end());
    }
    else
    {
        throw ImageReaderIOException("No such directory: " + imageDirectory);
    }
}

const std::vector<std::string>&
ImageReader::GetFileNames() const
{
    return mFileNames;
}

std::vector<cv::Mat>
ImageReader::LoadAllImages() const
{
    std::vector<cv::Mat> imageSet;
    typedef std::vector<std::string>::const_iterator fileIt;

    LOG(INFO) << "Loading all image files...";
    for (fileIt it = mFileNames.begin(); it != mFileNames.end(); ++it)
    {
        cv::Mat image = cv::imread(mFilesPath + *it, CV_LOAD_IMAGE_COLOR);
        if (!image.data)
        {
            LOG(ERROR) << "Could not open image " << mFilesPath + *it;
        }
        else
        {
            imageSet.push_back(image);
        }
    }

    return imageSet;
}

cv::Mat
ImageReader::LoadNextImage()
{
    if (mLastImageIndex >= mFileNames.size())
    {
        // If the exception is caught, it is possible to reload
        // the images from the beginning
        mLastImageIndex = 0;
        throw ImageReaderIOException("No more images to be loaded");
    }

    LOG(INFO) << "Loading image " << mFileNames[mLastImageIndex];

    cv::Mat image = cv::imread(mFilesPath + mFileNames[mLastImageIndex],
                               CV_LOAD_IMAGE_COLOR);

    if (!image.data)
    {
        LOG(ERROR) << "Could not open image "
        << mFilesPath + mFileNames[mLastImageIndex];
        throw ImageReaderIOException("Could not open image");
    }

    mLastImageIndex++;
    return image;
}

