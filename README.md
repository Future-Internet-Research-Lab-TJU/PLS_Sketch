# PLS-Sketch: A High-Performance Sketch for Finding Persistent Low-Spread Flows

---
### Paper

__Zhuo Li, Junheng Ye, Jindian Liu, Yu Zhang, Peng Luo, Jian Shi.__ 
PLS-Sketch: A High-Performance Sketch for Finding Persistent Low-Spread Flows

---
### Files
- Models/CardinalityEstimator/BloomFilter.*: the bloom filter of PLS-Sketch
- Models/PLSSketch.*: the class of PLS-Sketch
- Models/PLSSketchTest.*: the program of testing PLS-Sketch
- Utils/include/murmur.h: murmur hash
- Utils/include/PcapReader.h: the program of reading pcapng file
- Utils/include/reader.h: the program of reading pcap file
- Utils/include/utils.h: util function
- Test.cpp: entry file
---

### Compile and Run the examples
PLS-Sketch is implemented with C++. We show how to compile the examples on
Ubuntu with g++ and cmake.

#### Requirements
- Ensure __g++__, __make__ and _cmake_ packet are installed.  Our experimental platform is
  equipped with Ubuntu 20.04 and C++20 or higher is required

  - Prepare the test files.
   - You can use Real-life Transactional, CAIDA, MAWI, DataCenter and so on to test its performance. The code can support multiple file formats, including .pcap, .dat and so on.
   - You can use the dataset _1.pcap_ to test.


   #### Compile
- Compile examples with make

```
    $ cd build
    $ cmake ..
    $ cmake --build .
    $ ./Sketch
```

- The g++ compiler should be suitable 


#### Run
- Run the examples, and the program will output some statistics. 

```
    $ ./Sketch
```
