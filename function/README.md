# Function
```
taskset -c 4 function/gbench_function
2026-01-16T19:44:03+02:00
Running function/gbench_function
Run on (14 X 400 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x14)
  L1 Instruction 32 KiB (x14)
  L2 Unified 1280 KiB (x14)
  L3 Unified 24576 KiB (x1)
Load Average: 0.35, 0.76, 1.26
---------------------------------------------------------------------------------------------------------
Benchmark                                               Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------------------------------------
Create/std::function/CS1/1000                        8345 ns         8331 ns        86598 items_per_second=120.038M/s
Create/std::move_only_function/CS1/1000              7888 ns         7872 ns        88718 items_per_second=127.035M/s
Create/solid::Function/CS1/1000                      7381 ns         7366 ns        94137 items_per_second=135.761M/s
Create/solid::Function64T/CS1/1000                   7475 ns         7460 ns        91352 items_per_second=134.055M/s
Create/boost::function/CS1/1000                      7196 ns         7181 ns        95057 items_per_second=139.257M/s
Run/std::function/CS1/1000                           1038 ns         1037 ns       668014 items_per_second=9.64686G/s
Run/std::move_only_function/CS1/1000                 1053 ns         1051 ns       663473 items_per_second=9.51295G/s
Run/solid::Function/CS1/1000                         1043 ns         1041 ns       647943 items_per_second=9.60165G/s
Run/solid::Function64T/CS1/1000                      1273 ns         1271 ns       556069 items_per_second=7.86987G/s
Run/boost::function/CS1/1000                         1262 ns         1260 ns       555968 items_per_second=7.93656G/s
CreateAndRun/std::function/CS1/1000                 18726 ns        18687 ns        37497 items_per_second=53.5141M/s
CreateAndRun/std::move_only_function/CS1/1000       20548 ns        20513 ns        34150 items_per_second=48.7508M/s
CreateAndRun/solid::Function/CS1/1000               17543 ns        17505 ns        39975 items_per_second=57.1273M/s
CreateAndRun/solid::Function64T/CS1/1000            19556 ns        19517 ns        35841 items_per_second=51.237M/s
CreateAndRun/boost::function/CS1/1000               19422 ns        19385 ns        36008 items_per_second=51.5874M/s
Create/std::function/CS2/1000                        8472 ns         8456 ns        82095 items_per_second=118.266M/s
Create/std::move_only_function/CS2/1000              7882 ns         7867 ns        87392 items_per_second=127.116M/s
Create/solid::Function/CS2/1000                      7695 ns         7680 ns        90561 items_per_second=130.215M/s
Create/solid::Function64T/CS2/1000                   7645 ns         7629 ns        92184 items_per_second=131.073M/s
Create/boost::function/CS2/1000                      7370 ns         7355 ns        93165 items_per_second=135.959M/s
Run/std::function/CS2/1000                           1271 ns         1268 ns       550677 items_per_second=7.88425G/s
Run/std::move_only_function/CS2/1000                 1046 ns         1044 ns       668076 items_per_second=9.57445G/s
Run/solid::Function/CS2/1000                         1301 ns         1299 ns       536994 items_per_second=7.69771G/s
Run/solid::Function64T/CS2/1000                      1344 ns         1341 ns       506983 items_per_second=7.45535G/s
Run/boost::function/CS2/1000                         1440 ns         1437 ns       510625 items_per_second=6.95783G/s
CreateAndRun/std::function/CS2/1000                 19767 ns        19729 ns        35376 items_per_second=50.6873M/s
CreateAndRun/std::move_only_function/CS2/1000       18516 ns        18482 ns        37706 items_per_second=54.1064M/s
CreateAndRun/solid::Function/CS2/1000               20184 ns        20146 ns        34655 items_per_second=49.6368M/s
CreateAndRun/solid::Function64T/CS2/1000            20305 ns        20263 ns        34576 items_per_second=49.3505M/s
CreateAndRun/boost::function/CS2/1000               21989 ns        21947 ns        31935 items_per_second=45.5637M/s
Create/std::function/CS3/1000                       33148 ns        33084 ns        20698 items_per_second=30.2262M/s
Create/std::move_only_function/CS3/1000              8393 ns         8378 ns        83184 items_per_second=119.367M/s
Create/solid::Function/CS3/1000                      7893 ns         7878 ns        88362 items_per_second=126.935M/s
Create/solid::Function64T/CS3/1000                   8052 ns         8038 ns        87198 items_per_second=124.415M/s
Create/boost::function/CS3/1000                      7817 ns         7801 ns        89518 items_per_second=128.184M/s
Run/std::function/CS3/1000                           1358 ns         1356 ns       517096 items_per_second=7.37667G/s
Run/std::move_only_function/CS3/1000                 1089 ns         1087 ns       641914 items_per_second=9.20166G/s
Run/solid::Function/CS3/1000                         1335 ns         1333 ns       521819 items_per_second=7.50387G/s
Run/solid::Function64T/CS3/1000                      1316 ns         1314 ns       532669 items_per_second=7.61185G/s
Run/boost::function/CS3/1000                         1118 ns         1115 ns       624401 items_per_second=8.97142G/s
CreateAndRun/std::function/CS3/1000                 46011 ns        45920 ns        15151 items_per_second=21.7771M/s
CreateAndRun/std::move_only_function/CS3/1000       19512 ns        19476 ns        35647 items_per_second=51.3459M/s
CreateAndRun/solid::Function/CS3/1000               21130 ns        21089 ns        33017 items_per_second=47.4179M/s
CreateAndRun/solid::Function64T/CS3/1000            19123 ns        19086 ns        36759 items_per_second=52.3941M/s
CreateAndRun/boost::function/CS3/1000               18687 ns        18651 ns        37489 items_per_second=53.6165M/s
Create/std::function/CS4/1000                       33467 ns        33400 ns        21095 items_per_second=29.9403M/s
Create/std::move_only_function/CS4/1000             32608 ns        32538 ns        21459 items_per_second=30.7334M/s
Create/solid::Function/CS4/1000                     32885 ns        32823 ns        21131 items_per_second=30.4664M/s
Create/solid::Function64T/CS4/1000                   8176 ns         8161 ns        85841 items_per_second=122.537M/s
Create/boost::function/CS4/1000                     33272 ns        33211 ns        21615 items_per_second=30.1103M/s
Run/std::function/CS4/1000                           1582 ns         1580 ns       441471 items_per_second=6.33044G/s
Run/std::move_only_function/CS4/1000                 1338 ns         1335 ns       517137 items_per_second=7.48922G/s
Run/solid::Function/CS4/1000                         1355 ns         1351 ns       516052 items_per_second=7.39951G/s
Run/solid::Function64T/CS4/1000                      1338 ns         1335 ns       515998 items_per_second=7.491G/s
Run/boost::function/CS4/1000                         1362 ns         1360 ns       516993 items_per_second=7.35296G/s
CreateAndRun/std::function/CS4/1000                 46564 ns        46478 ns        14905 items_per_second=21.5157M/s
CreateAndRun/std::move_only_function/CS4/1000       44185 ns        44094 ns        15961 items_per_second=22.6791M/s
CreateAndRun/solid::Function/CS4/1000               46930 ns        46836 ns        14874 items_per_second=21.3512M/s
CreateAndRun/solid::Function64T/CS4/1000            21810 ns        21768 ns        31848 items_per_second=45.9382M/s
CreateAndRun/boost::function/CS4/1000               46130 ns        46040 ns        14980 items_per_second=21.7203M/s
Create/std::function/CS5/1000                       33550 ns        33480 ns        20837 items_per_second=29.8682M/s
Create/std::move_only_function/CS5/1000             32794 ns        32731 ns        21150 items_per_second=30.5521M/s
Create/solid::Function/CS5/1000                     32330 ns        32262 ns        21143 items_per_second=30.9961M/s
Create/solid::Function64T/CS5/1000                   8203 ns         8187 ns        84062 items_per_second=122.146M/s
Create/boost::function/CS5/1000                     33458 ns        33390 ns        20688 items_per_second=29.9492M/s
Run/std::function/CS5/1000                           1401 ns         1398 ns       498239 items_per_second=7.15149G/s
Run/std::move_only_function/CS5/1000                 1533 ns         1530 ns       455593 items_per_second=6.53404G/s
Run/solid::Function/CS5/1000                         1594 ns         1591 ns       440866 items_per_second=6.28385G/s
Run/solid::Function64T/CS5/1000                      1575 ns         1573 ns       443582 items_per_second=6.35924G/s
Run/boost::function/CS5/1000                         1384 ns         1381 ns       508385 items_per_second=7.24088G/s
CreateAndRun/std::function/CS5/1000                 48807 ns        48703 ns        13973 items_per_second=20.5327M/s
CreateAndRun/std::move_only_function/CS5/1000       46013 ns        45920 ns        15120 items_per_second=21.7772M/s
CreateAndRun/solid::Function/CS5/1000               49609 ns        49512 ns        14106 items_per_second=20.1972M/s
CreateAndRun/solid::Function64T/CS5/1000            21309 ns        21269 ns        32776 items_per_second=47.0161M/s
CreateAndRun/boost::function/CS5/1000               49884 ns        49778 ns        13907 items_per_second=20.0893M/s
Create/std::function/CS6/1000                       33007 ns        32943 ns        21024 items_per_second=30.3557M/s
Create/std::move_only_function/CS6/1000             32757 ns        32695 ns        21291 items_per_second=30.586M/s
Create/solid::Function/CS6/1000                     33221 ns        33154 ns        20947 items_per_second=30.1622M/s
Create/solid::Function64T/CS6/1000                   8252 ns         8237 ns        84270 items_per_second=121.402M/s
Create/boost::function/CS6/1000                     33948 ns        33881 ns        20560 items_per_second=29.5148M/s
Run/std::function/CS6/1000                           1399 ns         1397 ns       499023 items_per_second=7.15901G/s
Run/std::move_only_function/CS6/1000                 1562 ns         1558 ns       444940 items_per_second=6.41981G/s
Run/solid::Function/CS6/1000                         1587 ns         1584 ns       437669 items_per_second=6.31255G/s
Run/solid::Function64T/CS6/1000                      1364 ns         1362 ns       512311 items_per_second=7.34263G/s
Run/boost::function/CS6/1000                         1337 ns         1335 ns       500314 items_per_second=7.49195G/s
CreateAndRun/std::function/CS6/1000                 48649 ns        48551 ns        14465 items_per_second=20.597M/s
CreateAndRun/std::move_only_function/CS6/1000       45520 ns        45433 ns        15082 items_per_second=22.0105M/s
CreateAndRun/solid::Function/CS6/1000               48537 ns        48442 ns        14304 items_per_second=20.6434M/s
CreateAndRun/solid::Function64T/CS6/1000            21370 ns        21329 ns        32704 items_per_second=46.8836M/s
CreateAndRun/boost::function/CS6/1000               46424 ns        46333 ns        15304 items_per_second=21.5828M/s
Create/std::function/CS7/1000                       33515 ns        33450 ns        20846 items_per_second=29.8958M/s
Create/std::move_only_function/CS7/1000             32583 ns        32517 ns        21469 items_per_second=30.7535M/s
Create/solid::Function/CS7/1000                     32647 ns        32583 ns        21471 items_per_second=30.6913M/s
Create/solid::Function64T/CS7/1000                   8104 ns         8087 ns        85691 items_per_second=123.648M/s
Create/boost::function/CS7/1000                     33319 ns        33254 ns        20625 items_per_second=30.0716M/s
Run/std::function/CS7/1000                           1489 ns         1487 ns       466743 items_per_second=6.72646G/s
Run/std::move_only_function/CS7/1000                 1367 ns         1365 ns       506742 items_per_second=7.32601G/s
Run/solid::Function/CS7/1000                         1457 ns         1455 ns       473339 items_per_second=6.87405G/s
Run/solid::Function64T/CS7/1000                      1591 ns         1588 ns       434019 items_per_second=6.29601G/s
Run/boost::function/CS7/1000                         1438 ns         1436 ns       483420 items_per_second=6.96427G/s
CreateAndRun/std::function/CS7/1000                 49399 ns        49304 ns        14143 items_per_second=20.2825M/s
CreateAndRun/std::move_only_function/CS7/1000       46404 ns        46320 ns        14961 items_per_second=21.5889M/s
CreateAndRun/solid::Function/CS7/1000               49486 ns        49388 ns        14382 items_per_second=20.2479M/s
CreateAndRun/solid::Function64T/CS7/1000            23541 ns        23495 ns        29846 items_per_second=42.5623M/s
CreateAndRun/boost::function/CS7/1000               48096 ns        48001 ns        14535 items_per_second=20.8328M/s
Create/std::function/CS8/1000                       33404 ns        33333 ns        21095 items_per_second=30.0004M/s
Create/std::move_only_function/CS8/1000             32093 ns        32029 ns        21633 items_per_second=31.2214M/s
Create/solid::Function/CS8/1000                     32792 ns        32729 ns        21413 items_per_second=30.5536M/s
Create/solid::Function64T/CS8/1000                  32416 ns        32354 ns        21068 items_per_second=30.9076M/s
Create/boost::function/CS8/1000                     34264 ns        34199 ns        20696 items_per_second=29.2409M/s
Run/std::function/CS8/1000                           1525 ns         1522 ns       456866 items_per_second=6.56994G/s
Run/std::move_only_function/CS8/1000                 1370 ns         1368 ns       504509 items_per_second=7.31031G/s
Run/solid::Function/CS8/1000                         1499 ns         1496 ns       462440 items_per_second=6.68451G/s
Run/solid::Function64T/CS8/1000                      1643 ns         1640 ns       420024 items_per_second=6.09734G/s
Run/boost::function/CS8/1000                         1480 ns         1478 ns       466172 items_per_second=6.76714G/s
CreateAndRun/std::function/CS8/1000                 49558 ns        49466 ns        14057 items_per_second=20.216M/s
CreateAndRun/std::move_only_function/CS8/1000       46663 ns        46578 ns        15167 items_per_second=21.4696M/s
CreateAndRun/solid::Function/CS8/1000               47697 ns        47601 ns        14851 items_per_second=21.0079M/s
CreateAndRun/solid::Function64T/CS8/1000            49626 ns        49526 ns        13943 items_per_second=20.1913M/s
CreateAndRun/boost::function/CS8/1000               48759 ns        48665 ns        14366 items_per_second=20.5488M/s
Create/std::function/CS16/1000                      34044 ns        33975 ns        20600 items_per_second=29.4331M/s
Create/std::move_only_function/CS16/1000            31614 ns        31552 ns        22013 items_per_second=31.6932M/s
Create/solid::Function/CS16/1000                    33559 ns        33494 ns        20911 items_per_second=29.8557M/s
Create/solid::Function64T/CS16/1000                 32934 ns        32850 ns        21191 items_per_second=30.4418M/s
Create/boost::function/CS16/1000                    34394 ns        34329 ns        20929 items_per_second=29.1298M/s
Run/std::function/CS16/1000                          2239 ns         2235 ns       311891 items_per_second=4.47415G/s
Run/std::move_only_function/CS16/1000                2068 ns         2064 ns       335787 items_per_second=4.84382G/s
Run/solid::Function/CS16/1000                        2248 ns         2245 ns       309820 items_per_second=4.45524G/s
Run/solid::Function64T/CS16/1000                     2253 ns         2249 ns       308131 items_per_second=4.44556G/s
Run/boost::function/CS16/1000                        2213 ns         2210 ns       315323 items_per_second=4.52516G/s
CreateAndRun/std::function/CS16/1000                56379 ns        56261 ns        11553 items_per_second=17.7742M/s
CreateAndRun/std::move_only_function/CS16/1000      53817 ns        53695 ns        12746 items_per_second=18.6237M/s
CreateAndRun/solid::Function/CS16/1000              54803 ns        54692 ns        12461 items_per_second=18.2842M/s
CreateAndRun/solid::Function64T/CS16/1000           54853 ns        54736 ns        12561 items_per_second=18.2694M/s
CreateAndRun/boost::function/CS16/1000              55689 ns        55569 ns        12244 items_per_second=17.9955M/s
```