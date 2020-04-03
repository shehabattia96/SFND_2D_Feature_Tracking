#include <numeric>
#include "matching2D.hpp"

using namespace std;
using namespace cv;

// Find best matches for keypoints in two camera images based on several matching methods
void matchDescriptors(std::vector<KeyPoint> &kPtsSource, std::vector<KeyPoint> &kPtsRef, Mat &descSource, Mat &descRef, std::vector<DMatch> &matches, std::string descriptorFamily, std::string matcherType, std::string selectorType)
{
    // configure matcher
    bool crossCheck = false;
    Ptr<DescriptorMatcher> matcher;

    if (matcherType.compare("MAT_BF") == 0)
    {
        int normType = NORM_HAMMING;
        matcher = BFMatcher::create(normType, crossCheck);
    }
    else if (matcherType.compare("MAT_FLANN") == 0)
    {
        if (descriptorFamily.compare("DES_BINARY") == 0) { 
            matcher = makePtr<FlannBasedMatcher>(makePtr<cv::flann::LshIndexParams>(12, 20, 2)); // references Binary descriptor LshIndex + FlannBasedMatcher https://stackoverflow.com/a/43835993/9824103
        } else {
            matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
        }
        
    }

    // perform matching task
    if (selectorType.compare("SEL_NN") == 0)
    { // nearest neighbor (best match)

        matcher->match(descSource, descRef, matches); // Finds the best match for each descriptor in desc1
    }
    else if (selectorType.compare("SEL_KNN") == 0)
    {
        if ( descSource.empty() || descRef.empty()) {
            return;
        }
        std::vector<std::vector<DMatch>> knnMatches;
        unsigned int k = 2;	// Number of neighbors
        matcher->knnMatch(descSource, descRef, knnMatches, k);

        float distanceRatioFilter = 0.8f;

        for (std::vector<std::vector<DMatch>>::iterator matchVector = knnMatches.begin() ; matchVector != knnMatches.end(); ++matchVector) {
            if ((*matchVector).size() >= 2)
            { // only calculate ratio if there are two matches
                float closetDescriptor1Distance = (*matchVector)[0].distance;
                float closetDescriptor2Distance = (*matchVector)[1].distance;
                float ratio = closetDescriptor1Distance / closetDescriptor2Distance;
                if (ratio < distanceRatioFilter) matches.push_back((*matchVector)[0]);
            }
        }
    }
    std::cout << "Found " << matches.size() << " matches." << endl;
}

// Use one of several types of state-of-art descriptors to uniquely identify keypoints
void descKeypoints(vector<KeyPoint> &keypoints, Mat &img, Mat &descriptors, string descriptorType)
{
    // select appropriate descriptor
    Ptr<DescriptorExtractor> extractor;
    if (descriptorType.compare("BRISK") == 0)
    {

        int threshold = 30;        // FAST/AGAST detection threshold score.
        int octaves = 3;           // detection octaves (use 0 to do single scale)
        float patternScale = 1.0f; // apply this scale to the pattern used for sampling the neighbourhood of a keypoint.

        extractor = BRISK::create(threshold, octaves, patternScale);
    }
    else if (descriptorType.compare("BRIEF") == 0)
    {
        // all of the descriptions are taken from the opencv docs at https://docs.opencv.org/3.4/d1/d93/classcv_1_1xfeatures2d_1_1BriefDescriptorExtractor.html
        int bytes = 32; // legth of the descriptor in bytes, valid values are: 16, 32 (default) or 64 .
        bool use_orientation = false; // sample patterns using keypoints orientation, disabled by default.

        extractor = xfeatures2d::BriefDescriptorExtractor::create(bytes, use_orientation);
    }
    else if (descriptorType.compare("ORB") == 0)
    {
        // all of the descriptions are taken from the opencv docs at https://docs.opencv.org/3.4/db/d95/classcv_1_1ORB.html
        int nfeatures = 500; //	The maximum number of features to retain.
        float scaleFactor = 1.2f; // Pyramid decimation ratio, greater than 1. scaleFactor==2 means the classical pyramid, where each next level has 4x less pixels than the previous, but such a big scale factor will degrade feature matching scores dramatically. 
        int nlevels = 8; // The number of pyramid levels. The smallest level will have linear size equal to input_image_linear_size/pow(scaleFactor, nlevels - firstLevel).
        int edgeThreshold = 31; // This is size of the border where the features are not detected. It should roughly match the patchSize parameter.
        int firstLevel = 0; // The level of pyramid to put source image to. Previous layers are filled with upscaled source image.
        int WTA_K = 2; // The number of points that produce each element of the oriented BRIEF descriptor. 
        ORB::ScoreType scoreType = ORB::HARRIS_SCORE; // The default HARRIS_SCORE means that Harris algorithm is used to rank features 
        int patchSize = 31; // The size of the patch used by the oriented BRIEF descriptor. 
        int fastThreshold = 20; // The fast threshold

        extractor = ORB::create(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType,patchSize, fastThreshold);
    }
    else if (descriptorType.compare("FREAK") == 0)
    {
        // all of the descriptions are taken from the opencv docs at https://docs.opencv.org/3.4/df/db4/classcv_1_1xfeatures2d_1_1FREAK.html
        bool orientationNormalized = true; // Enable orientation normalization.
        bool scaleNormalized = true; // Enable scale normalization.
        float patternScale = 22.0f; // Scaling of the description pattern.
        int nOctaves = 4; // Number of octaves covered by the detected keypoints.

        extractor = xfeatures2d::FREAK::create(orientationNormalized, scaleNormalized, patternScale, nOctaves);
    }
    else if (descriptorType.compare("AKAZE") == 0)
    {
        // all of the descriptions are taken from the opencv docs at https://docs.opencv.org/3.4/d8/d30/classcv_1_1AKAZE.html
        AKAZE::DescriptorType descriptor_type = AKAZE::DESCRIPTOR_MLDB; // Type of the extracted descriptor: DESCRIPTOR_KAZE, DESCRIPTOR_KAZE_UPRIGHT, DESCRIPTOR_MLDB or DESCRIPTOR_MLDB_UPRIGHT.
        int descriptor_size = 0; // Size of the descriptor in bits. 0 -> Full size
        int descriptor_channels = 3; // Number of channels in the descriptor (1, 2, 3)
        float threshold = 0.001f; // Detector response threshold to accept point
        int nOctaves = 4; // Maximum octave evolution of the image
        int nOctaveLayers = 4; // Default number of sublevels per scale level
        KAZE::DiffusivityType diffusivity = KAZE::DIFF_PM_G2; // Diffusivity type. DIFF_PM_G1, DIFF_PM_G2, DIFF_WEICKERT or DIFF_CHARBONNIER

        extractor = AKAZE::create(descriptor_type, descriptor_size, descriptor_channels, threshold, nOctaves, nOctaveLayers, diffusivity);
    }
    else if (descriptorType.compare("SIFT") == 0)
    {
        // all of the descriptions are taken from the opencv docs at https://docs.opencv.org/3.4/d5/d3c/classcv_1_1xfeatures2d_1_1SIFT.html
        int nfeatures  = 0;        // The number of best features to retain. The features are ranked by their scores (measured in SIFT algorithm as the local contrast)
        int nOctaveLayers = 3;           // The number of layers in each octave. 3 is the value used in D. Lowe paper. The number of octaves is computed automatically from the image resolution.
        double contrastThreshold = 0.04; // The contrast threshold used to filter out weak features in semi-uniform (low-contrast) regions. The larger the threshold, the less features are produced by the detector.
        double edgeThreshold = 10.0; // The threshold used to filter out edge-like features. Note that the its meaning is different from the contrastThreshold, i.e. the larger the edgeThreshold, the less features are filtered out (more features are retained).
        double sigma = 1.6; // The sigma of the Gaussian applied to the input image at the octave #0. If your image is captured with a weak camera with soft lenses, you might want to reduce the number.

        extractor = xfeatures2d::SIFT::create(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);
    }

    // perform feature description
    double t = (double)getTickCount();
    extractor->compute(img, keypoints, descriptors);
    t = ((double)getTickCount() - t) / getTickFrequency();
    std::cout << descriptorType << " descriptor extraction in " << 1000 * t / 1.0 << " ms" << endl;
}

//***** Detectors *****//

// Detect keypoints in image using FAST 
void detKeypointsFAST(vector<KeyPoint> &keypoints, Mat &img, bool bVis)
{
    Ptr<FastFeatureDetector> detector = FastFeatureDetector::create();
    Mat output;
    detector->detect(img, keypoints, output);
    // visualize results
    visualizeKeyPoints(keypoints, img, bVis, "FAST");
}

// Detect keypoints in image using ORB 
void detKeypointsORB(vector<KeyPoint> &keypoints, Mat &img, bool bVis)
{
    Ptr<ORB> detector = ORB::create();
    Mat output;
    detector->detect(img, keypoints, output);
    // visualize results
    visualizeKeyPoints(keypoints, img, bVis, "ORB");
}

// Detect keypoints in image using BRISK 
void detKeypointsBRISK(vector<KeyPoint> &keypoints, Mat &img, bool bVis)
{
    Ptr<BRISK> detector = BRISK::create( );
    Mat output;
    detector->detect(img, keypoints, output);
    // visualize results
    visualizeKeyPoints(keypoints, img, bVis, "BRISK");
}

// Detect keypoints in image using AKAZE 
void detKeypointsAKAZE(vector<KeyPoint> &keypoints, Mat &img, bool bVis)
{
    Ptr<AKAZE> detector = AKAZE::create( );
    Mat output;
    detector->detect(img, keypoints, output);
    // visualize results
    visualizeKeyPoints(keypoints, img, bVis, "AKAZE");
}

// Detect keypoints in image using SIFT 
void detKeypointsSIFT(vector<KeyPoint> &keypoints, Mat &img, bool bVis)
{
    Ptr<xfeatures2d::SIFT> detector = xfeatures2d::SIFT::create( );
    Mat output;
    detector->detect(img, keypoints, output);
    // visualize results
    visualizeKeyPoints(keypoints, img, bVis, "SIFT");
}

// Detect keypoints in image using the traditional Shi-Thomasi detector
void detKeypointsShiTomasi(vector<KeyPoint> &keypoints, Mat &img, bool bVis)
{
    detKeypointsGoodFeaturesToTrack(keypoints, img, bVis, false);
}

// Detect keypoints in image using the HARRIS detector
void detKeypointsHarris(vector<KeyPoint> &keypoints, Mat &img, bool bVis)
{
    detKeypointsGoodFeaturesToTrack(keypoints, img, bVis, true);
}

void detKeypointsGoodFeaturesToTrack(vector<KeyPoint> &keypoints, Mat &img, bool bVis, bool useHarris)
{
    // compute detector parameters based on image size
    int blockSize = 4;       //  size of an average block for computing a derivative covariation matrix over each pixel neighborhood
    double maxOverlap = 0.0; // max. permissible overlap between two features in %
    double minDistance = (1.0 - maxOverlap) * blockSize;
    int maxCorners = img.rows * img.cols / max(1.0, minDistance); // max. num. of keypoints

    double qualityLevel = 0.01; // minimal accepted quality of image corners
    double k = 0.04;

    // Apply corner detection
    double t = (double)getTickCount();
    vector<Point2f> corners;
    goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance, Mat(), blockSize, useHarris, k);

    // add corners to result vector
    for (auto it = corners.begin(); it != corners.end(); ++it)
    {
        KeyPoint newKeyPoint;
        newKeyPoint.pt = Point2f((*it).x, (*it).y);
        newKeyPoint.size = blockSize;
        keypoints.push_back(newKeyPoint);
    }
    t = ((double)getTickCount() - t) / getTickFrequency();
    std::cout << (useHarris ? "Harris" : "Shi-Tomasi") << "detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    visualizeKeyPoints(keypoints, img, bVis, (std::string)(useHarris ? "Harris" : "Shi-Tomasi"));
}

void visualizeKeyPoints(vector<KeyPoint> &keypoints, Mat &img, bool bVis, std::string detectorName)
{
    // visualize results
    if (bVis)
    {
        Mat visImage = img.clone();
        drawKeypoints(img, keypoints, visImage, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "Visualize KeyPoints results for detector " + detectorName;
        namedWindow(windowName, 6);
        imshow(windowName, visImage);
        waitKey(0);
    }
}