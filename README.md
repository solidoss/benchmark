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

On a 12 gen i7, Fedora 41 (20250123):

```
Test project /home/valentin/work/benchmark/build/release
      Start  1: SolidEcho100_1000
 1/57 Test  #1: SolidEcho100_1000 ................   Passed    0.40 sec
      Start  2: SolidEcho200_1000
 2/57 Test  #2: SolidEcho200_1000 ................   Passed    0.79 sec
      Start  3: SolidEcho400_1000
 3/57 Test  #3: SolidEcho400_1000 ................   Passed    1.51 sec
      Start  4: SolidRelayEcho100_1000
 4/57 Test  #4: SolidRelayEcho100_1000 ...........   Passed    0.83 sec
      Start  5: SolidRelayEcho200_1000
 5/57 Test  #5: SolidRelayEcho200_1000 ...........   Passed    1.70 sec
      Start  6: SolidRelayEcho400_1000
 6/57 Test  #6: SolidRelayEcho400_1000 ...........   Passed    3.40 sec
      Start  7: BoostEcho100_1000
 7/57 Test  #7: BoostEcho100_1000 ................   Passed    4.74 sec
      Start  8: BoostEcho200_1000
 8/57 Test  #8: BoostEcho200_1000 ................   Passed    4.97 sec
      Start  9: BoostEcho400_1000
 9/57 Test  #9: BoostEcho400_1000 ................   Passed    4.57 sec
      Start 10: BoostRelayEcho100_1000
10/57 Test #10: BoostRelayEcho100_1000 ...........   Passed    4.95 sec
      Start 11: BoostRelayEcho200_1000
11/57 Test #11: BoostRelayEcho200_1000 ...........   Passed    4.77 sec
      Start 12: BoostRelayEcho400_1000
12/57 Test #12: BoostRelayEcho400_1000 ...........   Passed    4.68 sec
      Start 13: Boost1k
13/57 Test #13: Boost1k ..........................   Passed    0.01 sec
      Start 14: Cereal1k
14/57 Test #14: Cereal1k .........................   Passed    0.01 sec
      Start 15: CerealPortable1k
15/57 Test #15: CerealPortable1k .................   Passed    0.01 sec
      Start 16: SolidV3_1k
16/57 Test #16: SolidV3_1k .......................   Passed    0.01 sec
      Start 17: Boost10k
17/57 Test #17: Boost10k .........................   Passed    0.05 sec
      Start 18: Cereal10k
18/57 Test #18: Cereal10k ........................   Passed    0.05 sec
      Start 19: CerealPortable10k
19/57 Test #19: CerealPortable10k ................   Passed    0.04 sec
      Start 20: SolidV3_10k
20/57 Test #20: SolidV3_10k ......................   Passed    0.05 sec
      Start 21: Boost100k
21/57 Test #21: Boost100k ........................   Passed    0.48 sec
      Start 22: Cereal100k
22/57 Test #22: Cereal100k .......................   Passed    0.36 sec
      Start 23: CerealPortable100k
23/57 Test #23: CerealPortable100k ...............   Passed    0.36 sec
      Start 24: SolidV3_100k
24/57 Test #24: SolidV3_100k .....................   Passed    0.45 sec
      Start 25: SolidPlainNoComp_100_1
25/57 Test #25: SolidPlainNoComp_100_1 ...........   Passed    0.60 sec
      Start 26: SolidPlainNoComp_100_2
26/57 Test #26: SolidPlainNoComp_100_2 ...........   Passed    0.77 sec
      Start 27: SolidPlainNoComp_100_3
27/57 Test #27: SolidPlainNoComp_100_3 ...........   Passed    0.74 sec
      Start 28: SolidPlainNoComp_100_4
28/57 Test #28: SolidPlainNoComp_100_4 ...........   Passed    1.65 sec
      Start 29: SolidPlainNoComp_100_5
29/57 Test #29: SolidPlainNoComp_100_5 ...........   Passed    2.82 sec
      Start 30: SolidPlainNoComp_100_6
30/57 Test #30: SolidPlainNoComp_100_6 ...........   Passed    3.02 sec
      Start 31: SolidPlainComp_100_6
31/57 Test #31: SolidPlainComp_100_6 .............   Passed    2.74 sec
      Start 32: GrpcPlainNoComp_100_1
32/57 Test #32: GrpcPlainNoComp_100_1 ............   Passed    5.97 sec
      Start 33: GrpcPlainNoComp_100_2
33/57 Test #33: GrpcPlainNoComp_100_2 ............   Passed    6.66 sec
      Start 34: GrpcPlainNoComp_100_3
34/57 Test #34: GrpcPlainNoComp_100_3 ............   Passed    6.58 sec
      Start 35: GrpcPlainNoComp_100_4
35/57 Test #35: GrpcPlainNoComp_100_4 ............   Passed    7.86 sec
      Start 36: GrpcPlainNoComp_100_5
36/57 Test #36: GrpcPlainNoComp_100_5 ............   Passed    9.21 sec
      Start 37: GrpcPlainNoComp_100_6
37/57 Test #37: GrpcPlainNoComp_100_6 ............   Passed    9.62 sec
      Start 38: GrpcPlainNoComp_100_1_Stream
38/57 Test #38: GrpcPlainNoComp_100_1_Stream .....   Passed    3.03 sec
      Start 39: GrpcPlainNoComp_100_2_Stream
39/57 Test #39: GrpcPlainNoComp_100_2_Stream .....   Passed    3.16 sec
      Start 40: GrpcPlainNoComp_100_3_Stream
40/57 Test #40: GrpcPlainNoComp_100_3_Stream .....   Passed    3.19 sec
      Start 41: GrpcPlainNoComp_100_4_Stream
41/57 Test #41: GrpcPlainNoComp_100_4_Stream .....   Passed    3.77 sec
      Start 42: GrpcPlainNoComp_100_5_Stream
42/57 Test #42: GrpcPlainNoComp_100_5_Stream .....   Passed    4.76 sec
      Start 43: GrpcPlainNoComp_100_6_Stream
43/57 Test #43: GrpcPlainNoComp_100_6_Stream .....   Passed    4.93 sec
      Start 44: CapnpPlainNoComp_100_1
44/57 Test #44: CapnpPlainNoComp_100_1 ...........   Passed    4.60 sec
      Start 45: CapnpPlainNoComp_100_2
45/57 Test #45: CapnpPlainNoComp_100_2 ...........   Passed    5.28 sec
      Start 46: CapnpPlainNoComp_100_3
46/57 Test #46: CapnpPlainNoComp_100_3 ...........   Passed    4.93 sec
      Start 47: CapnpPlainNoComp_100_4
47/57 Test #47: CapnpPlainNoComp_100_4 ...........   Passed    7.51 sec
      Start 48: CapnpPlainNoComp_100_5
48/57 Test #48: CapnpPlainNoComp_100_5 ...........   Passed    9.38 sec
      Start 49: CapnpPlainNoComp_100_6
49/57 Test #49: CapnpPlainNoComp_100_6 ...........   Passed   10.40 sec
      Start 50: CapnpPlainNoComp_10_1
50/57 Test #50: CapnpPlainNoComp_10_1 ............   Passed    4.35 sec
      Start 51: CapnpPlainNoComp_10_2
51/57 Test #51: CapnpPlainNoComp_10_2 ............   Passed    5.10 sec
      Start 52: CapnpPlainNoComp_10_3
52/57 Test #52: CapnpPlainNoComp_10_3 ............   Passed    4.70 sec
      Start 53: CapnpPlainNoComp_10_4
53/57 Test #53: CapnpPlainNoComp_10_4 ............   Passed    7.27 sec
      Start 54: CapnpPlainNoComp_10_5
54/57 Test #54: CapnpPlainNoComp_10_5 ............   Passed    9.91 sec
      Start 55: CapnpPlainNoComp_10_6
55/57 Test #55: CapnpPlainNoComp_10_6 ............   Passed   10.13 sec
      Start 56: ThreadPoolSolid1
56/57 Test #56: ThreadPoolSolid1 .................   Passed    5.28 sec
      Start 57: ThreadPoolErez1
57/57 Test #57: ThreadPoolErez1 ..................   Passed   10.79 sec
```

On a 12 gen i7, WSL Fedora 36 (20230917):
```
Test project /home/vipal/work/benchmark/build/release
      Start  1: SolidEcho100_1000
 1/43 Test  #1: SolidEcho100_1000 ................   Passed    0.60 sec
      Start  2: SolidEcho200_1000
 2/43 Test  #2: SolidEcho200_1000 ................   Passed    1.13 sec
      Start  3: SolidEcho400_1000
 3/43 Test  #3: SolidEcho400_1000 ................   Passed    2.16 sec
      Start  4: SolidRelayEcho100_1000
 4/43 Test  #4: SolidRelayEcho100_1000 ...........   Passed    1.81 sec
      Start  5: SolidRelayEcho200_1000
 5/43 Test  #5: SolidRelayEcho200_1000 ...........   Passed    3.56 sec
      Start  6: SolidRelayEcho400_1000
 6/43 Test  #6: SolidRelayEcho400_1000 ...........   Passed    5.33 sec
      Start  7: BoostEcho100_1000
 7/43 Test  #7: BoostEcho100_1000 ................   Passed    5.31 sec
      Start  8: BoostEcho200_1000
 8/43 Test  #8: BoostEcho200_1000 ................   Passed    5.65 sec
      Start  9: BoostEcho400_1000
 9/43 Test  #9: BoostEcho400_1000 ................   Passed    5.95 sec
      Start 10: BoostRelayEcho100_1000
10/43 Test #10: BoostRelayEcho100_1000 ...........   Passed    5.72 sec
      Start 11: BoostRelayEcho200_1000
11/43 Test #11: BoostRelayEcho200_1000 ...........   Passed    6.31 sec
      Start 12: BoostRelayEcho400_1000
12/43 Test #12: BoostRelayEcho400_1000 ...........   Passed    7.43 sec
      Start 13: Boost1k
13/43 Test #13: Boost1k ..........................   Passed    0.02 sec
      Start 14: Cereal1k
14/43 Test #14: Cereal1k .........................   Passed    0.01 sec
      Start 15: CerealPortable1k
15/43 Test #15: CerealPortable1k .................   Passed    0.02 sec
      Start 16: SolidV3_1k
16/43 Test #16: SolidV3_1k .......................   Passed    0.02 sec
      Start 17: Boost10k
17/43 Test #17: Boost10k .........................   Passed    0.12 sec
      Start 18: Cereal10k
18/43 Test #18: Cereal10k ........................   Passed    0.09 sec
      Start 19: CerealPortable10k
19/43 Test #19: CerealPortable10k ................   Passed    0.10 sec
      Start 20: SolidV3_10k
20/43 Test #20: SolidV3_10k ......................   Passed    0.13 sec
      Start 21: Boost100k
21/43 Test #21: Boost100k ........................   Passed    1.04 sec
      Start 22: Cereal100k
22/43 Test #22: Cereal100k .......................   Passed    0.74 sec
      Start 23: CerealPortable100k
23/43 Test #23: CerealPortable100k ...............   Passed    0.78 sec
      Start 24: SolidV3_100k
24/43 Test #24: SolidV3_100k .....................   Passed    0.99 sec
      Start 25: SolidPlainNoComp_100_1
25/43 Test #25: SolidPlainNoComp_100_1 ...........   Passed    0.85 sec
      Start 26: SolidPlainNoComp_100_2
26/43 Test #26: SolidPlainNoComp_100_2 ...........   Passed    1.96 sec
      Start 27: SolidPlainNoComp_100_3
27/43 Test #27: SolidPlainNoComp_100_3 ...........   Passed    1.55 sec
      Start 28: SolidPlainNoComp_100_4
28/43 Test #28: SolidPlainNoComp_100_4 ...........   Passed    5.15 sec
      Start 29: SolidPlainNoComp_100_5
29/43 Test #29: SolidPlainNoComp_100_5 ...........   Passed    8.38 sec
      Start 30: SolidPlainNoComp_100_6
30/43 Test #30: SolidPlainNoComp_100_6 ...........   Passed    9.89 sec
      Start 31: SolidPlainComp_100_6
31/43 Test #31: SolidPlainComp_100_6 .............   Passed    9.96 sec
      Start 32: GrpcPlainNoComp_100_1
32/43 Test #32: GrpcPlainNoComp_100_1 ............   Passed   14.51 sec
      Start 33: GrpcPlainNoComp_100_2
33/43 Test #33: GrpcPlainNoComp_100_2 ............   Passed   15.11 sec
      Start 34: GrpcPlainNoComp_100_3
34/43 Test #34: GrpcPlainNoComp_100_3 ............   Passed   14.95 sec
      Start 35: GrpcPlainNoComp_100_4
35/43 Test #35: GrpcPlainNoComp_100_4 ............   Passed   16.34 sec
      Start 36: GrpcPlainNoComp_100_5
36/43 Test #36: GrpcPlainNoComp_100_5 ............   Passed   17.78 sec
      Start 37: GrpcPlainNoComp_100_6
37/43 Test #37: GrpcPlainNoComp_100_6 ............   Passed   18.93 sec
      Start 38: GrpcPlainNoComp_100_1_Stream
38/43 Test #38: GrpcPlainNoComp_100_1_Stream .....   Passed    4.12 sec
      Start 39: GrpcPlainNoComp_100_2_Stream
39/43 Test #39: GrpcPlainNoComp_100_2_Stream .....   Passed    4.48 sec
      Start 40: GrpcPlainNoComp_100_3_Stream
40/43 Test #40: GrpcPlainNoComp_100_3_Stream .....   Passed    4.47 sec
      Start 41: GrpcPlainNoComp_100_4_Stream
41/43 Test #41: GrpcPlainNoComp_100_4_Stream .....   Passed    5.94 sec
      Start 42: GrpcPlainNoComp_100_5_Stream
42/43 Test #42: GrpcPlainNoComp_100_5_Stream .....   Passed    7.30 sec
      Start 43: GrpcPlainNoComp_100_6_Stream
43/43 Test #43: GrpcPlainNoComp_100_6_Stream .....   Passed    7.60 sec

100% tests passed, 0 tests failed out of 43
```
---------------------------------------------------------------------

On a 12 gen i7, WSL Fedora 36 :
```
Test project /home/vipal/work/benchmark/build/release
      Start  1: SolidEcho100_1000
 1/37 Test  #1: SolidEcho100_1000 ................   Passed    0.30 sec
      Start  2: SolidEcho200_1000
 2/37 Test  #2: SolidEcho200_1000 ................   Passed    0.61 sec
      Start  3: SolidEcho400_1000
 3/37 Test  #3: SolidEcho400_1000 ................   Passed    1.11 sec
      Start  4: SolidRelayEcho100_1000
 4/37 Test  #4: SolidRelayEcho100_1000 ...........   Passed    1.05 sec
      Start  5: SolidRelayEcho200_1000
 5/37 Test  #5: SolidRelayEcho200_1000 ...........   Passed    2.02 sec
      Start  6: SolidRelayEcho400_1000
 6/37 Test  #6: SolidRelayEcho400_1000 ...........   Passed    3.57 sec
      Start  7: BoostEcho100_1000
 7/37 Test  #7: BoostEcho100_1000 ................   Passed    5.48 sec
      Start  8: BoostEcho200_1000
 8/37 Test  #8: BoostEcho200_1000 ................   Passed    5.90 sec
      Start  9: BoostEcho400_1000
 9/37 Test  #9: BoostEcho400_1000 ................   Passed    6.45 sec
      Start 10: BoostRelayEcho100_1000
10/37 Test #10: BoostRelayEcho100_1000 ...........   Passed    6.15 sec
      Start 11: BoostRelayEcho200_1000
11/37 Test #11: BoostRelayEcho200_1000 ...........   Passed    6.61 sec
      Start 12: BoostRelayEcho400_1000
12/37 Test #12: BoostRelayEcho400_1000 ...........   Passed    7.13 sec
      Start 13: Boost1k
13/37 Test #13: Boost1k ..........................   Passed    0.01 sec
      Start 14: Cereal1k
14/37 Test #14: Cereal1k .........................   Passed    0.01 sec
      Start 15: CerealPortable1k
15/37 Test #15: CerealPortable1k .................   Passed    0.01 sec
      Start 16: SolidV3_1k
16/37 Test #16: SolidV3_1k .......................   Passed    0.01 sec
      Start 17: Boost10k
17/37 Test #17: Boost10k .........................   Passed    0.06 sec
      Start 18: Cereal10k
18/37 Test #18: Cereal10k ........................   Passed    0.05 sec
      Start 19: CerealPortable10k
19/37 Test #19: CerealPortable10k ................   Passed    0.05 sec
      Start 20: SolidV3_10k
20/37 Test #20: SolidV3_10k ......................   Passed    0.07 sec
      Start 21: Boost100k
21/37 Test #21: Boost100k ........................   Passed    0.59 sec
      Start 22: Cereal100k
22/37 Test #22: Cereal100k .......................   Passed    0.49 sec
      Start 23: CerealPortable100k
23/37 Test #23: CerealPortable100k ...............   Passed    0.48 sec
      Start 24: SolidV3_100k
24/37 Test #24: SolidV3_100k .....................   Passed    0.68 sec
      Start 25: SolidPlainNoComp_100_1
25/37 Test #25: SolidPlainNoComp_100_1 ...........   Passed    0.53 sec
      Start 26: SolidPlainNoComp_100_2
26/37 Test #26: SolidPlainNoComp_100_2 ...........   Passed    1.07 sec
      Start 27: SolidPlainNoComp_100_3
27/37 Test #27: SolidPlainNoComp_100_3 ...........   Passed    0.87 sec
      Start 28: SolidPlainNoComp_100_4
28/37 Test #28: SolidPlainNoComp_100_4 ...........   Passed    2.99 sec
      Start 29: SolidPlainNoComp_100_5
29/37 Test #29: SolidPlainNoComp_100_5 ...........   Passed    4.84 sec
      Start 30: SolidPlainNoComp_100_6
30/37 Test #30: SolidPlainNoComp_100_6 ...........   Passed    5.43 sec
      Start 31: SolidPlainComp_100_6
31/37 Test #31: SolidPlainComp_100_6 .............   Passed    5.45 sec
      Start 32: GrpcPlainNoComp_100_1
32/37 Test #32: GrpcPlainNoComp_100_1 ............   Passed    5.86 sec
      Start 33: GrpcPlainNoComp_100_2
33/37 Test #33: GrpcPlainNoComp_100_2 ............   Passed    6.39 sec
      Start 34: GrpcPlainNoComp_100_3
34/37 Test #34: GrpcPlainNoComp_100_3 ............   Passed    6.26 sec
      Start 35: GrpcPlainNoComp_100_4
35/37 Test #35: GrpcPlainNoComp_100_4 ............   Passed    7.93 sec
      Start 36: GrpcPlainNoComp_100_5
36/37 Test #36: GrpcPlainNoComp_100_5 ............   Passed    9.78 sec
      Start 37: GrpcPlainNoComp_100_6
37/37 Test #37: GrpcPlainNoComp_100_6 ............   Passed   11.16 sec

100% tests passed, 0 tests failed out of 37
```
AWS aarch64:

```
Running tests...
Test project /home/ec2-user/work/benchmark/build/release
      Start  1: SolidEcho100_1000
 1/37 Test  #1: SolidEcho100_1000 ................   Passed    1.60 sec
      Start  2: SolidEcho200_1000
 2/37 Test  #2: SolidEcho200_1000 ................   Passed    3.43 sec
      Start  3: SolidEcho400_1000
 3/37 Test  #3: SolidEcho400_1000 ................   Passed    6.95 sec
      Start  4: SolidRelayEcho100_1000
 4/37 Test  #4: SolidRelayEcho100_1000 ...........   Passed    4.64 sec
      Start  5: SolidRelayEcho200_1000
 5/37 Test  #5: SolidRelayEcho200_1000 ...........   Passed    9.54 sec
      Start  6: SolidRelayEcho400_1000
 6/37 Test  #6: SolidRelayEcho400_1000 ...........   Passed   17.79 sec
      Start  7: BoostEcho100_1000
 7/37 Test  #7: BoostEcho100_1000 ................   Passed    5.53 sec
      Start  8: BoostEcho200_1000
 8/37 Test  #8: BoostEcho200_1000 ................   Passed    6.17 sec
      Start  9: BoostEcho400_1000
 9/37 Test  #9: BoostEcho400_1000 ................   Passed    8.60 sec
      Start 10: BoostRelayEcho100_1000
10/37 Test #10: BoostRelayEcho100_1000 ...........   Passed    6.92 sec
      Start 11: BoostRelayEcho200_1000
11/37 Test #11: BoostRelayEcho200_1000 ...........   Passed    8.91 sec
      Start 12: BoostRelayEcho400_1000
12/37 Test #12: BoostRelayEcho400_1000 ...........   Passed   18.58 sec
      Start 13: Boost1k
13/37 Test #13: Boost1k ..........................   Passed    0.04 sec
      Start 14: Cereal1k
14/37 Test #14: Cereal1k .........................   Passed    0.03 sec
      Start 15: CerealPortable1k
15/37 Test #15: CerealPortable1k .................   Passed    0.03 sec
      Start 16: SolidV3_1k
16/37 Test #16: SolidV3_1k .......................   Passed    0.03 sec
      Start 17: Boost10k
17/37 Test #17: Boost10k .........................   Passed    0.32 sec
      Start 18: Cereal10k
18/37 Test #18: Cereal10k ........................   Passed    0.24 sec
      Start 19: CerealPortable10k
19/37 Test #19: CerealPortable10k ................   Passed    0.27 sec
      Start 20: SolidV3_10k
20/37 Test #20: SolidV3_10k ......................   Passed    0.30 sec
      Start 21: Boost100k
21/37 Test #21: Boost100k ........................   Passed    2.99 sec
      Start 22: Cereal100k
22/37 Test #22: Cereal100k .......................   Passed    2.41 sec
      Start 23: CerealPortable100k
23/37 Test #23: CerealPortable100k ...............   Passed    2.66 sec
      Start 24: SolidV3_100k
24/37 Test #24: SolidV3_100k .....................   Passed    2.97 sec
      Start 25: SolidPlainNoComp_100_1
25/37 Test #25: SolidPlainNoComp_100_1 ...........   Passed    2.76 sec
      Start 26: SolidPlainNoComp_100_2
26/37 Test #26: SolidPlainNoComp_100_2 ...........   Passed    5.27 sec
      Start 27: SolidPlainNoComp_100_3
27/37 Test #27: SolidPlainNoComp_100_3 ...........   Passed    4.49 sec
      Start 28: SolidPlainNoComp_100_4
28/37 Test #28: SolidPlainNoComp_100_4 ...........   Passed   12.93 sec
      Start 29: SolidPlainNoComp_100_5
29/37 Test #29: SolidPlainNoComp_100_5 ...........   Passed   18.95 sec
      Start 30: SolidPlainNoComp_100_6
30/37 Test #30: SolidPlainNoComp_100_6 ...........   Passed   21.71 sec
      Start 31: SolidPlainComp_100_6
31/37 Test #31: SolidPlainComp_100_6 .............   Passed   20.82 sec
      Start 32: GrpcPlainNoComp_100_1
32/37 Test #32: GrpcPlainNoComp_100_1 ............   Passed   26.80 sec
      Start 33: GrpcPlainNoComp_100_2
33/37 Test #33: GrpcPlainNoComp_100_2 ............   Passed   29.30 sec
      Start 34: GrpcPlainNoComp_100_3
34/37 Test #34: GrpcPlainNoComp_100_3 ............   Passed   28.59 sec
      Start 35: GrpcPlainNoComp_100_4
35/37 Test #35: GrpcPlainNoComp_100_4 ............   Passed   37.63 sec
      Start 36: GrpcPlainNoComp_100_5
36/37 Test #36: GrpcPlainNoComp_100_5 ............   Passed   45.39 sec
      Start 37: GrpcPlainNoComp_100_6
37/37 Test #37: GrpcPlainNoComp_100_6 ............   Passed   49.55 sec

100% tests passed, 0 tests failed out of 37
```
