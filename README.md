# benchmark

Benchmark SolidFrame against different libraries 

## On Linux

Prerequisites:
```bash
sudo dnf install re2-devel c-ares-devel
mkdir ext_bench
cd ext_bench
../solidframe/prerequisites/build --openssl
../benchmark/prerequisites/build.sh --boost
../benchmark/prerequisites/build.sh --capnp
../benchmark/prerequisites/build.sh --protobuf
../benchmark/prerequisites/build.sh --grpc
```

```bash
cmake -DCMAKE_BUILD_TYPE=maintain -DEXTERNAL_PATH=~/work/ext_bench -DSolidFrame_DIR=~/work/solidframe/build/maintain ../../
```

```bash
cmake -DCMAKE_BUILD_TYPE=release -DEXTERNAL_PATH=~/work/ext_bench -DSolidFrame_DIR=~/work/solidframe/build/release ../../
```

### Results

On a 12 gen i7, Fedora 41 (20260111):

```
[0/1] Running tests...
Test project /home/valentin/work/benchmark/build/release
        Start   1: SolidEcho100_1000
  1/100 Test   #1: SolidEcho100_1000 ................   Passed    0.44 sec
        Start   2: SolidEcho200_1000
  2/100 Test   #2: SolidEcho200_1000 ................   Passed    0.86 sec
        Start   3: SolidEcho400_1000
  3/100 Test   #3: SolidEcho400_1000 ................   Passed    1.67 sec
        Start   4: SolidRelayEcho100_1000
  4/100 Test   #4: SolidRelayEcho100_1000 ...........   Passed    0.92 sec
        Start   5: SolidRelayEcho200_1000
  5/100 Test   #5: SolidRelayEcho200_1000 ...........   Passed    1.69 sec
        Start   6: SolidRelayEcho400_1000
  6/100 Test   #6: SolidRelayEcho400_1000 ...........   Passed    3.48 sec
        Start   7: BoostEcho100_1000
  7/100 Test   #7: BoostEcho100_1000 ................   Passed    0.45 sec
        Start   8: BoostEcho200_1000
  8/100 Test   #8: BoostEcho200_1000 ................   Passed    0.87 sec
        Start   9: BoostEcho400_1000
  9/100 Test   #9: BoostEcho400_1000 ................   Passed    1.67 sec
        Start  10: BoostCoroEcho100_1000
 10/100 Test  #10: BoostCoroEcho100_1000 ............   Passed    0.46 sec
        Start  11: BoostCoroEcho200_1000
 11/100 Test  #11: BoostCoroEcho200_1000 ............   Passed    0.87 sec
        Start  12: BoostCoroEcho400_1000
 12/100 Test  #12: BoostCoroEcho400_1000 ............   Passed    1.63 sec
        Start  13: BoostRelayEcho100_1000
 13/100 Test  #13: BoostRelayEcho100_1000 ...........   Passed    4.38 sec
        Start  14: BoostRelayEcho200_1000
 14/100 Test  #14: BoostRelayEcho200_1000 ...........   Passed    4.40 sec
        Start  15: BoostRelayEcho400_1000
 15/100 Test  #15: BoostRelayEcho400_1000 ...........   Passed    4.61 sec
        Start  16: Boost1k
 16/100 Test  #16: Boost1k ..........................   Passed    0.01 sec
        Start  17: Cereal1k
 17/100 Test  #17: Cereal1k .........................   Passed    0.01 sec
        Start  18: CerealPortable1k
 18/100 Test  #18: CerealPortable1k .................   Passed    0.01 sec
        Start  19: SolidV3_1k
 19/100 Test  #19: SolidV3_1k .......................   Passed    0.01 sec
        Start  20: Boost10k
 20/100 Test  #20: Boost10k .........................   Passed    0.06 sec
        Start  21: Cereal10k
 21/100 Test  #21: Cereal10k ........................   Passed    0.05 sec
        Start  22: CerealPortable10k
 22/100 Test  #22: CerealPortable10k ................   Passed    0.05 sec
        Start  23: SolidV3_10k
 23/100 Test  #23: SolidV3_10k ......................   Passed    0.06 sec
        Start  24: Boost100k
 24/100 Test  #24: Boost100k ........................   Passed    0.50 sec
        Start  25: Cereal100k
 25/100 Test  #25: Cereal100k .......................   Passed    0.49 sec
        Start  26: CerealPortable100k
 26/100 Test  #26: CerealPortable100k ...............   Passed    0.42 sec
        Start  27: SolidV3_100k
 27/100 Test  #27: SolidV3_100k .....................   Passed    0.46 sec
        Start  28: SolidPlainNoComp_100_1
 28/100 Test  #28: SolidPlainNoComp_100_1 ...........   Passed    0.61 sec
        Start  29: SolidPlainNoComp_100_2
 29/100 Test  #29: SolidPlainNoComp_100_2 ...........   Passed    0.79 sec
        Start  30: SolidPlainNoComp_100_3
 30/100 Test  #30: SolidPlainNoComp_100_3 ...........   Passed    0.74 sec
        Start  31: SolidPlainNoComp_100_4
 31/100 Test  #31: SolidPlainNoComp_100_4 ...........   Passed    1.81 sec
        Start  32: SolidPlainNoComp_100_5
 32/100 Test  #32: SolidPlainNoComp_100_5 ...........   Passed    3.21 sec
        Start  33: SolidPlainNoComp_100_6
 33/100 Test  #33: SolidPlainNoComp_100_6 ...........   Passed    3.52 sec
        Start  34: SolidPlainComp_100_6
 34/100 Test  #34: SolidPlainComp_100_6 .............   Passed    2.77 sec
        Start  35: GrpcPlainNoComp_100_1
 35/100 Test  #35: GrpcPlainNoComp_100_1 ............   Passed    6.30 sec
        Start  36: GrpcPlainNoComp_100_2
 36/100 Test  #36: GrpcPlainNoComp_100_2 ............   Passed    6.85 sec
        Start  37: GrpcPlainNoComp_100_3
 37/100 Test  #37: GrpcPlainNoComp_100_3 ............   Passed    6.89 sec
        Start  38: GrpcPlainNoComp_100_4
 38/100 Test  #38: GrpcPlainNoComp_100_4 ............   Passed    7.81 sec
        Start  39: GrpcPlainNoComp_100_5
 39/100 Test  #39: GrpcPlainNoComp_100_5 ............   Passed    8.93 sec
        Start  40: GrpcPlainNoComp_100_6
 40/100 Test  #40: GrpcPlainNoComp_100_6 ............   Passed    9.30 sec
        Start  41: GrpcPlainNoComp_100_1_Stream
 41/100 Test  #41: GrpcPlainNoComp_100_1_Stream .....   Passed    3.33 sec
        Start  42: GrpcPlainNoComp_100_2_Stream
 42/100 Test  #42: GrpcPlainNoComp_100_2_Stream .....   Passed    3.57 sec
        Start  43: GrpcPlainNoComp_100_3_Stream
 43/100 Test  #43: GrpcPlainNoComp_100_3_Stream .....   Passed    3.56 sec
        Start  44: GrpcPlainNoComp_100_4_Stream
 44/100 Test  #44: GrpcPlainNoComp_100_4_Stream .....   Passed    4.04 sec
        Start  45: GrpcPlainNoComp_100_5_Stream
 45/100 Test  #45: GrpcPlainNoComp_100_5_Stream .....   Passed    4.59 sec
        Start  46: GrpcPlainNoComp_100_6_Stream
 46/100 Test  #46: GrpcPlainNoComp_100_6_Stream .....   Passed    4.74 sec
        Start  47: CapnpPlainNoComp_100_1
 47/100 Test  #47: CapnpPlainNoComp_100_1 ...........   Passed    4.65 sec
        Start  48: CapnpPlainNoComp_100_2
 48/100 Test  #48: CapnpPlainNoComp_100_2 ...........   Passed    5.28 sec
        Start  49: CapnpPlainNoComp_100_3
 49/100 Test  #49: CapnpPlainNoComp_100_3 ...........   Passed    4.90 sec
        Start  50: CapnpPlainNoComp_100_4
 50/100 Test  #50: CapnpPlainNoComp_100_4 ...........   Passed    7.31 sec
        Start  51: CapnpPlainNoComp_100_5
 51/100 Test  #51: CapnpPlainNoComp_100_5 ...........   Passed    9.31 sec
        Start  52: CapnpPlainNoComp_100_6
 52/100 Test  #52: CapnpPlainNoComp_100_6 ...........   Passed   10.33 sec
        Start  53: CapnpPlainNoComp_10_1
 53/100 Test  #53: CapnpPlainNoComp_10_1 ............   Passed    4.36 sec
        Start  54: CapnpPlainNoComp_10_2
 54/100 Test  #54: CapnpPlainNoComp_10_2 ............   Passed    5.05 sec
        Start  55: CapnpPlainNoComp_10_3
 55/100 Test  #55: CapnpPlainNoComp_10_3 ............   Passed    4.62 sec
        Start  56: CapnpPlainNoComp_10_4
 56/100 Test  #56: CapnpPlainNoComp_10_4 ............   Passed    7.06 sec
        Start  57: CapnpPlainNoComp_10_5
 57/100 Test  #57: CapnpPlainNoComp_10_5 ............   Passed    8.48 sec
        Start  58: CapnpPlainNoComp_10_6
 58/100 Test  #58: CapnpPlainNoComp_10_6 ............   Passed   10.13 sec
        Start  59: ThreadPoolSolid1
 59/100 Test  #59: ThreadPoolSolid1 .................   Passed    4.34 sec
        Start  60: ThreadPoolRigtorp1
 60/100 Test  #60: ThreadPoolRigtorp1 ...............   Passed    6.45 sec
        Start  61: ThreadPoolErez1
 61/100 Test  #61: ThreadPoolErez1 ..................   Passed   10.59 sec
        Start  62: FunctionStd1
 62/100 Test  #62: FunctionStd1 .....................   Passed    2.58 sec
        Start  63: FunctionMStd1
 63/100 Test  #63: FunctionMStd1 ....................   Passed    2.31 sec
        Start  64: FunctionBoost1
 64/100 Test  #64: FunctionBoost1 ...................   Passed    2.10 sec
        Start  65: FunctionSolid1
 65/100 Test  #65: FunctionSolid1 ...................   Passed    2.09 sec
        Start  66: FunctionSolid641
 66/100 Test  #66: FunctionSolid641 .................   Passed    2.25 sec
        Start  67: FunctionStd2
 67/100 Test  #67: FunctionStd2 .....................   Passed    2.64 sec
        Start  68: FunctionMStd2
 68/100 Test  #68: FunctionMStd2 ....................   Passed    2.16 sec
        Start  69: FunctionBoost2
 69/100 Test  #69: FunctionBoost2 ...................   Passed    2.44 sec
        Start  70: FunctionSolid2
 70/100 Test  #70: FunctionSolid2 ...................   Passed    2.49 sec
        Start  71: FunctionSolid642
 71/100 Test  #71: FunctionSolid642 .................   Passed    2.35 sec
        Start  72: FunctionStd3
 72/100 Test  #72: FunctionStd3 .....................   Passed    4.62 sec
        Start  73: FunctionMStd3
 73/100 Test  #73: FunctionMStd3 ....................   Passed    2.35 sec
        Start  74: FunctionBoost3
 74/100 Test  #74: FunctionBoost3 ...................   Passed    2.08 sec
        Start  75: FunctionSolid3
 75/100 Test  #75: FunctionSolid3 ...................   Passed    2.11 sec
        Start  76: FunctionSolid643
 76/100 Test  #76: FunctionSolid643 .................   Passed    2.21 sec
        Start  77: FunctionStd4
 77/100 Test  #77: FunctionStd4 .....................   Passed    4.34 sec
        Start  78: FunctionMStd4
 78/100 Test  #78: FunctionMStd4 ....................   Passed    4.09 sec
        Start  79: FunctionBoost4
 79/100 Test  #79: FunctionBoost4 ...................   Passed    4.50 sec
        Start  80: FunctionSolid4
 80/100 Test  #80: FunctionSolid4 ...................   Passed    4.18 sec
        Start  81: FunctionSolid644
 81/100 Test  #81: FunctionSolid644 .................   Passed    2.38 sec
        Start  82: FunctionBoost5
 82/100 Test  #82: FunctionBoost5 ...................   Passed    4.24 sec
        Start  83: FunctionSolid5
 83/100 Test  #83: FunctionSolid5 ...................   Passed    4.39 sec
        Start  84: FunctionSolid645
 84/100 Test  #84: FunctionSolid645 .................   Passed    2.26 sec
        Start  85: FunctionBoost6
 85/100 Test  #85: FunctionBoost6 ...................   Passed    4.85 sec
        Start  86: FunctionSolid6
 86/100 Test  #86: FunctionSolid6 ...................   Passed    4.76 sec
        Start  87: FunctionSolid646
 87/100 Test  #87: FunctionSolid646 .................   Passed    2.23 sec
        Start  88: FunctionBoost7
 88/100 Test  #88: FunctionBoost7 ...................   Passed    5.14 sec
        Start  89: FunctionSolid7
 89/100 Test  #89: FunctionSolid7 ...................   Passed    4.82 sec
        Start  90: FunctionSolid647
 90/100 Test  #90: FunctionSolid647 .................   Passed    2.29 sec
        Start  91: FunctionStd8
 91/100 Test  #91: FunctionStd8 .....................   Passed    4.77 sec
        Start  92: FunctionMStd8
 92/100 Test  #92: FunctionMStd8 ....................   Passed    4.70 sec
        Start  93: FunctionBoost8
 93/100 Test  #93: FunctionBoost8 ...................   Passed    4.87 sec
        Start  94: FunctionSolid8
 94/100 Test  #94: FunctionSolid8 ...................   Passed    4.80 sec
        Start  95: FunctionSolid648
 95/100 Test  #95: FunctionSolid648 .................   Passed    4.71 sec
        Start  96: FunctionStd16
 96/100 Test  #96: FunctionStd16 ....................   Passed    7.45 sec
        Start  97: FunctionMStd16
 97/100 Test  #97: FunctionMStd16 ...................   Passed    7.16 sec
        Start  98: FunctionBoost16
 98/100 Test  #98: FunctionBoost16 ..................   Passed    7.43 sec
        Start  99: FunctionSolid16
 99/100 Test  #99: FunctionSolid16 ..................   Passed    7.33 sec
        Start 100: FunctionSolid6416
100/100 Test #100: FunctionSolid6416 ................   Passed    7.53 sec

100% tests passed, 0 tests failed out of 100
```
