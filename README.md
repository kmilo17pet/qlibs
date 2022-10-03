[![Built for](https://img.shields.io/badge/built%20for-microcontrollers-lightgrey?logo=WhiteSource)](https://github.com/kmilo17pet/qTools)
[![CodeFactor](https://www.codefactor.io/repository/github/kmilo17pet/qlibs/badge)](https://www.codefactor.io/repository/github/kmilo17pet/qlibs)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/14d566939d2e4d4181088cc1c6666fa3)](https://www.codacy.com/gh/kmilo17pet/qTools/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=kmilo17pet/qTools&amp;utm_campaign=Badge_Grade)
[![CodeInspectorScore](https://api.codiga.io/project/27197/score/svg)](https://app.codiga.io/project/27197/dashboard)
[![CodeInspectorGrade](https://api.codiga.io/project/27197/status/svg)](https://app.codiga.io/project/27197/dashboard)
[![Softacheck](https://softacheck.com/app/repository/kmilo17pet/qlibs/badge)](https://softacheck.com/app/repository/kmilo17pet/qlibs/issues)
[![Documentation](https://softacheck.com/app/repository/kmilo17pet/qlibs/documentation/badge)](https://softacheck.com/app/docs/kmilo17pet/qlibs/)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kmilo17pet/qlibs?logo=webpack)](https://github.com/kmilo17pet/qlibs/releases)
[![MISRAC2012](https://img.shields.io/badge/MISRAC2012-Compliant-blue.svg?logo=c)](https://en.wikipedia.org/wiki/MISRA_C)
[![CERT](https://img.shields.io/badge/CERT-Compliant-blue.svg?logo=c)](https://wiki.sei.cmu.edu/confluence/display/seccode/SEI+CERT+Coding+Standards)
[![C Standard](https://img.shields.io/badge/STD-C99-green.svg?logo=c)](https://en.wikipedia.org/wiki/C99)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/kmilo17pet/qTools/graphs/commit-activity)
[![License](https://img.shields.io/github/license/kmilo17pet/qTools)](https://github.com/kmilo17pet/qTools/blob/main/LICENSE) 


![qlibs_logo_small](https://user-images.githubusercontent.com/11412210/192115666-a5e3b615-b635-47bb-a30a-fb9107a53b48.png)
# qlibs : A collection of useful libraries for embedded systems


* Download the latest release [here](https://github.com/kmilo17pet/qlibs/releases)
* Documentation and API Reference [here](https://kmilo17pet.github.io/qlibs/)


Below is the list of the modules provided and their features:

- qSSmoother : Filters to smooth noisy signals
   - `LPF1`: _Low Pass Filter Order 1_
   - `LPF2`: _Low Pass Filter Order 2_
   - `MWM1`: _Moving Window Median O(n)_
   - `MWM2`: _Moving Window Median O(1): With TDL(works efficient for large windows)_
   - `MOR1`: _Moving Outlier Removal O(n)_
   - `MOR2`: _Moving Outlier Removal O(1): With TDL(works efficient for large windows)_
   - `GMWF`: _Gaussian filter_
   - `KLMN`: _Scalar Kalman filter_
   - `EXPW`: _Exponential weighting filter_
- qPID : Closed Loop PID Controller
  - Derivative filter
  - Anti-Windup
  - Tracking Mode
  - Auto-tunning 
  - Additive MRAC
- qLTISys : Recursive LTI systems evaluation by transfer functions
  - Continuous
  - Discrete
- qFIS : Fuzzy Inference System Engine
  - Mamdani
  - Sugeno
  - Tsukamoto
- qFP16 : Q16.16 Fixed-point math
  - Basic operations
  - Trigonometric functions
  - Exponential functions
- qCRC : Generic Cyclic Redundancy Check (CRC) calculator
  - CRC8
  - CRC16
  - CRC32
- qBitField: A bit-field manipulation library
- qTDL : Tapped Delay Line in O(1). 
- qRMS : Recursive Root Mean Square(RMS) calculation of a signal.


