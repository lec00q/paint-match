/**
 * @brief Acquisition library for images.
 *
 * @copyright Copyright 2013, Trya Srl
 * via Siemens 19 - 39100 Bolzano BZ, ITALY
 *
 * @author Piero Donaggio <piero.donaggio@trya.it>
 * @file ImageReader.h
 */

#ifndef image_reader_h
#define image_reader_h

#include <stdexcept>
#include <vector>
#include <string>
#include <opencv2/core/core.hpp>

/**
 * @class ImageReader
 * @brief A helper class for loading images using OpenCV methods.
 *
 * This class does not attempt to load the images until a call to
 * LoadAllImages() or LoadNextImage() is made. The rationale is to
 * save some memory by not storing all the images, unless not stricty
 * necessary.
 */
class ImageReader
{
public:

    /**
     * @brief Default constructor
     */
    ImageReader();
    /**
     * @brief Constructor from string
     * @param[in] imageDirectory Name of the directory that contains the images
     * to be loaded
     */
    ImageReader(const std::string &imageDirectory);
    /**
     * @brief Default destructor
     */
    virtual ~ImageReader();
    /**
     * @brief Set the source directory that contains the images
     * @param[in] imageDirectory Name of the directory that contains the images
     * to be loaded
     */
    void operator() (const std::string &imageDirectory);
    /**
     * @brief Retrieve image filenames in the source directory
     * @return A vector of filenames
     */
    const std::vector<std::string>& GetFileNames() const;
    /**
     * @brief Load all images in the source directory
     * @return A vector of images
     */
    std::vector<cv::Mat> LoadAllImages() const;
    /**
     * @brief Load the next image in the source directory
     * @return An image
     */
    cv::Mat LoadNextImage();
    /**
     * @brief Load a single image
     * @param[in] fileName Image file name
     * @return An image
     */
    static cv::Mat LoadImage (const std::string &fileName);
    /**
     * @brief Load a single image from the set
     * @param[in] i Index of the image
     * @return An image
     */
    cv::Mat LoadImage (unsigned int i);

private:

    /**
     * @brief A list of image filenames
     */
    std::vector<std::string> mFileNames;
    /**
     * @brief The image file path
     */
    std::string mFilesPath;
    /**
     * @brief The index of the last loaded image
     */
    size_t mLastImageIndex;
};

inline ImageReader::~ImageReader() {};

class ImageReaderIOException : public std::runtime_error
{
public:
    ImageReaderIOException(const std::string &msg = "") :
        runtime_error(msg) {}
};

#endif // header guard
