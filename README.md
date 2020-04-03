# SFND 2D Feature Tracking

<img src="images/keypoints.png" width="820" height="248" />

The idea of the camera course is to build a collision detection system - that's the overall goal for the Final Project. As a preparation for this, you will now build the feature tracking part and test various detector / descriptor combinations to see which ones perform best. This mid-term project consists of four parts:

## Implementation

### Data Buffer Optimization

- Acceptance Criteria: Implement a vector for `dataBuffer` objects whose size does not exceed a limit (e.g. 2 elements). This can be achieved by pushing new elements on one end and removing elements on the other end.
- Variables needed:
  - `dataFrameArray` - Array of DataFrame type, this will be the buffer container
  - `bufferSize` - the maximum number of items to store in the buffer
  - `headIndex` - this will be the index to the buffer container's head
  - `tailIndex` - this will be the index to the buffer container's tail
  - `numberOfItemsInBuffer` - this will store the number of items currently in the buffer
- Methods:
  - `writeToBuffer(DataFrame &dataFrameItem)` - writes the address of a dataframe to the buffer array
  - `readFromBuffer()` - reads the first item in the buffer array
- Steps:
  - Create a structure that stores an `array` of type DataFrame of a specified `buffer size`.
  - Keep a `head index` to point at newest item's index and `tail index` to point at the oldest item's index.
  - Keep a counter of the `number of items` in the buffer.
  - Whenever an item is inserted into the buffer, the head index moves to the next index after inserting the item there. The "number of items" counter is incremented. Must meet following conditions:
    - If `number of items >= buffer size`, do nothing. This will lead to loss of data. An alternative solution is to overwrite stale data, but this is out of scope.
    - If `head index == buffer size - 1`, the head moves to the beginning of the buffer (index 0).
  - If an item is deque'd from the buffer, the value at the tail index is returned and the tail index is incremented. The "number of items" counter is decremented.
    - If `number of items in buffer == 0`, do nothing. There is nothing to deque.
    - If `tail index == buffer size - 1`, the head moves to the beginning of the buffer (index 0).
  > Note: instead of using an index counter, we could also increment the pointer address by the size of the `dataBuffer` struct, but I find this approach harder for a human to read; despite the very little difference in time and space complexity.
- Unit test: [./tests/test_circularBuffer.cpp](./tests/test_circularBuffer.cpp)
- References:
  - [Circular buffer example](https://stackoverflow.com/a/827749/9824103)

### Keypoint Detection

- Acceptance Criteria: Implement detectors HARRIS, FAST, BRISK, ORB, AKAZE and SIFT and make them selectable by setting a string accordingly.
- Methods in [./src/matching2D.hpp](./src/matching2D.hpp):
  - detKeypointsHarris
    - Reference: [https://docs.opencv.org/2.4/modules/imgproc/doc/feature_detection.html?highlight=goodfeaturestotrack](https://docs.opencv.org/2.4/modules/imgproc/doc/feature_detection.html?highlight=goodfeaturestotrack)
  - detKeypointsFAST
    - Reference: [https://docs.opencv.org/master/df/d0c/tutorial_py_fast.html](https://docs.opencv.org/master/df/d0c/tutorial_py_fast.html)
  - detKeypointsBRISK
    - Reference: [https://docs.opencv.org/2.4/modules/features2d/doc/feature_detection_and_description.html](https://docs.opencv.org/2.4/modules/features2d/doc/feature_detection_and_description.html)
  - detKeypointsORB
    - Reference: [https://docs.opencv.org/2.4/modules/features2d/doc/feature_detection_and_description.html](https://docs.opencv.org/2.4/modules/features2d/doc/feature_detection_and_description.html)
  - detKeypointsAKAZE
    - Reference: [https://docs.opencv.org/3.4/db/d70/tutorial_akaze_matching.html](https://docs.opencv.org/3.4/db/d70/tutorial_akaze_matching.html)
  - detKeypointsSIFT
    - Reference: [https://docs.opencv.org/3.4/d5/d3c/classcv_1_1xfeatures2d_1_1SIFT.html](https://docs.opencv.org/3.4/d5/d3c/classcv_1_1xfeatures2d_1_1SIFT.html)
- Notes:
  - I've noticed that some keypoints are spot on and other ones sometimes miss that target by a couple of pixels. Rarely the keypoints are completely off (Only ORB was totally wrong, but I read that it may be because of the descriptor needing to be brute force instead of flann.).
  - I ignored adding hyper-parameters to the detectors, but will tweak the parameters for the descriptors.

### Keypoint removal

- Acceptance Criteria: Remove all keypoints outside of a pre-defined rectangle and only use the keypoints within the rectangle for further processing.
- Implementation plan: To remove points outside the ROI, we will erase those points if they are not contained in the rectangle of interest.
- References:
  - [KeyPoint pt](https://docs.opencv.org/master/d2/d29/classcv_1_1KeyPoint.html#ae6b87d798d3e181a472b08fa33883abe)
  - [Rectangle Contains](https://docs.opencv.org/3.4/d2/d44/classcv_1_1Rect__.html)
  - [std::remove_if notation](https://stackoverflow.com/a/22998209/9824103)
- Failed attempts:
  - I thought it may be more efficient to just crop the image instead of compute keypoints on the whole image then erase them, but when I did that, all of the binary descriptors performed really poorly at describing and matching keypoints between consequitive frames. I'm guessing it's because there is less randomness and intensity differences in the cropped image (for binary algorithms). The SIFT descriptor performed beautifully and extremely fast too! But only SIFT worked for this use-case.
    - Code:

      ```c++
        bool bFocusOnVehicle = true;
        cv::Rect vehicleRect(535, 180, 180, 150);
        if (bFocusOnVehicle)
        {
            imgGray = imgGray(vehicleRect);
        }
      ```

### Keypoint descriptors

- Acceptance Criteria: Implement descriptors BRIEF, ORB, FREAK, AKAZE and SIFT and make them selectable by setting a string accordingly.
- Implementation plan:
  - Each descriptor has different hyperparameters that we would need to tweak. For now, I'll just go with the default params.
    - BRIEF
      - Reference: [https://docs.opencv.org/3.4/d1/d93/classcv_1_1xfeatures2d_1_1BriefDescriptorExtractor.html](https://docs.opencv.org/3.4/d1/d93/classcv_1_1xfeatures2d_1_1BriefDescriptorExtractor.html)
    - FREAK
      - Reference: [https://docs.opencv.org/3.4/df/db4/classcv_1_1xfeatures2d_1_1FREAK.html](https://docs.opencv.org/3.4/df/db4/classcv_1_1xfeatures2d_1_1FREAK.html)
    - ORB
      - Reference: [https://docs.opencv.org/3.4/db/d95/classcv_1_1ORB.html](https://docs.opencv.org/3.4/db/d95/classcv_1_1ORB.html)
    - AKAZE
      - Reference: [https://docs.opencv.org/3.4/d8/d30/classcv_1_1AKAZE.html](https://docs.opencv.org/3.4/d8/d30/classcv_1_1AKAZE.html)
    - SIFT
      - Reference: [https://docs.opencv.org/3.4/d5/d3c/classcv_1_1xfeatures2d_1_1SIFT.html](https://docs.opencv.org/3.4/d5/d3c/classcv_1_1xfeatures2d_1_1SIFT.html)
  - Observations:
    - Not all combinations of Detectors and Descriptors work. For example, running Shi-Tomasi detector and passing it to an AKAZE descriptor produces a run-time error. HOG based detectors seem more suited for BRISK and BRIEF descriptors. Some Binary detectors work with other Binary descriptors but not all of them are interchangeable.
  - Types of descriptors:
    - Binary descriptors are: ORB, BRIEF, BRISK, FREAK, AKAZE
    - Floating-point/HOG descriptors are SIFT and SURF
    - References:
      - [Types of descriptors](https://stackoverflow.com/a/52969885/9824103)

### Descriptor Matching and Selection

- Acceptance Criteria: Implement FLANN matching as well as k-nearest neighbor selection. Both methods must be selectable using the respective strings in the main function.
- Implementation plan:
  - I'll follow the documentation on how to implement the matcher and selector.
    - FLANN Matcher
      - Reference [https://docs.opencv.org/3.4/d5/d6f/tutorial_feature_flann_matcher.html](https://docs.opencv.org/3.4/d5/d6f/tutorial_feature_flann_matcher.html)
    - KNN Selector
      - Reference [https://docs.opencv.org/3.4/db/d39/classcv_1_1DescriptorMatcher.html#a378f35c9b1a5dfa4022839a45cdf0e89](https://docs.opencv.org/3.4/db/d39/classcv_1_1DescriptorMatcher.html#a378f35c9b1a5dfa4022839a45cdf0e89)
- Observations:
  - SIFT does not work with BF HAMMING method.

### Descriptor Distance Ratio

- Acceptance Criteria: Use the K-Nearest-Neighbor matching to implement the descriptor distance ratio test, which looks at the ratio of best vs. second-best match to decide whether to keep an associated pair of keypoints.
- Implementation plan:
  - The KNN selector returns a vector of vectors of DMatch. For each point in the first descriptor, it returns n DMatches for the second descriptor for the closest point.
  - According to the DMatch documentatin, DMatch has a method `distance` that returns the distance.
  - We will iterate through the length of the knnMatches, extract the distances of the two closest neighbors and divide them to calculate a ratio. Then we compare the ratio with the required 0.8 tolerance distance ratio, if the ratio is within the acceptable range (`ratio < distanceRatioFilter`), we'll add the DMatch to the matches array.
    - References:
      - [DMatch docs](https://docs.opencv.org/3.4/d4/de0/classcv_1_1DMatch.html)
      - [Secondary reference to Lowe's paper on using distance ratio to find good points](https://docs.opencv.org/master/dc/dc3/tutorial_py_matcher.html)
- Observations:
  - Visually, the accuracy of the points within the distanceRatioFilter is a lot better than the points outside the distanceRatioFilter. SIFT implementation seems to be a bit faster too.

## Dependencies for Running Locally

- cmake >= 2.8
  - All OSes: [click here for installation instructions](https://cmake.org/install/)
- make >= 4.1 (Linux, Mac), 3.81 (Windows)
  - Linux: make is installed by default on most Linux distros
  - Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  - Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
- OpenCV >= 4.1
  - This must be compiled from source using the `-D OPENCV_ENABLE_NONFREE=ON` cmake flag for testing the SIFT and SURF detectors.
  - The OpenCV 4.1.0 source code can be found [here](https://github.com/opencv/opencv/tree/4.1.0)
- gcc/g++ >= 5.4
  - Linux: gcc / g++ is installed by default on most Linux distros
  - Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  - Windows: recommend using [MinGW](http://www.mingw.org/)
- VCPKG
  - install OPENCV with Contrib and nonFree flags `vcpkg install opencv[nonfree,contrib]:x64-windows`, then build the toolset using `vcpkg integrate install`, then build the project using cmake and load the toolset to the path created by vcpkg integrate install.

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./2D_feature_tracking`

Original repository [https://github.com/udacity/SFND_2D_Feature_Tracking](https://github.com/udacity/SFND_2D_Feature_Tracking)
