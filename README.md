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

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./2D_feature_tracking`

Original repository [https://github.com/udacity/SFND_2D_Feature_Tracking](https://github.com/udacity/SFND_2D_Feature_Tracking)
