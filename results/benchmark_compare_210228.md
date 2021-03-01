# Running Queue Latency Microbenchmark : side-by-side compare algorithms

All test runs performed on [Target Hardware](target_architecture.md)

```

| name ---- | cpus | rate - msg/sec | total msg count | lat med ns | lat 90p ns | lat 95p ns | lat 99p ns |lat 99.5p ns|lat 99.7p ns| lat max ns | 0123456789ABCDEFGHIJKLMNOPQRSTUVXYZ log2 hist
| ----------|------|----------------|-----------------|------------|------------|------------|------------|------------|------------|------------|----------------------------------------------
| mutex     |    4 |             10 |             100 |       1133 |       2159 |       5024 |      30666 |      30666 |      30666 |      30666 | ........3663311
| mut+drain |    4 |             10 |             100 |       1149 |       2124 |       4844 |      23999 |      23999 |      23999 |      23999 | ........4563211
| mutex  cb |    4 |             10 |             100 |       1510 |       2209 |       4339 |      25762 |      25762 |      25762 |      25762 | ........35633.1
| mut+dr cb |    4 |             10 |             100 |       1204 |       2056 |       2515 |      25779 |      25779 |      25779 |      25779 | ........26632.1
| spinlock  |    4 |             10 |             100 |       1797 |       5721 |       8956 |      27484 |      27484 |      27484 |      27484 | ........4555431
| spindrain |    4 |             10 |             100 |       1151 |       3645 |       4939 |      27166 |      27166 |      27166 |      27166 | ........45653.1
| spin  cb  |    4 |             10 |             100 |        780 |       1494 |       1795 |      29580 |      29580 |      29580 |      29580 | ........565.1.1
| spindr cb |    4 |             10 |             100 |        565 |       1177 |       1468 |      23922 |      23922 |      23922 |      23922 | .......16641..1
| lockfree  |    4 |             10 |             100 |        633 |        807 |        856 |      26244 |      26244 |      26244 |      26244 | ........57....1
| lockfr+dr |    4 |             10 |             100 |        718 |        971 |       1026 |      20084 |      20084 |      20084 |      20084 | ........473...1

| mutex     |    4 |            100 |            1000 |       1148 |       2212 |       3208 |       5796 |       7033 |      10278 |      33092 | ........699762.1
| mut+drain |    4 |            100 |            1000 |       1112 |       1959 |       2259 |       5774 |       7444 |      10109 |      35803 | ........799652.1
| mutex  cb |    4 |            100 |            1000 |       1203 |       2195 |       2804 |       5982 |       7018 |       8069 |      30086 | ........6997511
| mut+dr cb |    4 |            100 |            1000 |       1204 |       1902 |       2304 |       5431 |       6936 |       7844 |      28738 | ........69A6511
| spinlock  |    4 |            100 |            1000 |       2051 |       5904 |       8057 |      12801 |      13964 |      15367 |      34918 | .......158998611
| spindrain |    4 |            100 |            1000 |       1273 |       3670 |       4940 |       7899 |       9327 |      10168 |      35298 | .......2799873.1
| spin  cb  |    4 |            100 |            1000 |        741 |       1833 |       2513 |       4021 |       4955 |       5978 |      33870 | ........89974..1
| spindr cb |    4 |            100 |            1000 |        573 |       1027 |       1239 |       1981 |       2328 |       2589 |      31267 | .......49A73..1
| lockfree  |    4 |            100 |            1000 |        549 |        663 |        700 |        806 |        860 |        925 |      32940 | ........9A.....1
| lockfr+dr |    4 |            100 |            1000 |        656 |        765 |        807 |       1052 |       1083 |       1150 |      34526 | ........6A4....1

| mutex     |    4 |           1000 |           10000 |       1147 |       2132 |       2837 |       5627 |       6761 |       8220 |      89570 | ........ACDA951.1
| mut+drain |    4 |           1000 |           10000 |       1111 |       1943 |       2315 |       5150 |       5468 |       5836 |      88152 | .......2ACDA931.1
| mutex  cb |    4 |           1000 |           10000 |       1182 |       2056 |       2454 |       5263 |       5772 |       6081 |      96004 | ........9CDA83..1
| mut+dr cb |    4 |           1000 |           10000 |       1170 |       1865 |       2149 |       4551 |       5275 |       5504 |      96219 | ........9CD981..1
| spinlock  |    4 |           1000 |           10000 |       2280 |       6608 |       8327 |      12661 |      14651 |      15720 |      98163 | ........9BCCB95.1
| spindrain |    4 |           1000 |           10000 |       1382 |       3603 |       4591 |       6819 |       8104 |       8857 |      95557 | .......2ACCCA6..1
| spin  cb  |    4 |           1000 |           10000 |        839 |       1786 |       2238 |       3268 |       3738 |       4075 |      93960 | .......4BDCA5...1
| spindr cb |    4 |           1000 |           10000 |        638 |       1126 |       1346 |       1984 |       2246 |       2451 |      96697 | .......7CDB72...1
| lockfree  |    4 |           1000 |           10000 |        567 |        691 |        722 |        793 |        819 |        841 |      88383 | ........CD2.....1
| lockfr+dr |    4 |           1000 |           10000 |        630 |        764 |        802 |        946 |       1005 |       1051 |      91039 | ........BE6.....1

| mutex     |    4 |           2500 |           25000 |       1172 |       2109 |       2762 |       5518 |       6326 |       7250 |     177219 | .......2BEEBA61..1
| mut+drain |    4 |           2500 |           25000 |       1122 |       1978 |       2466 |       5239 |       5757 |       6622 |     184196 | .......3BEEBA5...1
| mutex  cb |    4 |           2500 |           25000 |       1175 |       2024 |       2486 |       5300 |       5975 |       6601 |     184266 | ........BEEBA6...1
| mut+dr cb |    4 |           2500 |           25000 |       1308 |       2060 |       2362 |       5195 |       5586 |       5948 |     187403 | ........ADEBA5...1
| spinlock  |    4 |           2500 |           25000 |       2253 |       6744 |       8607 |      12859 |      14665 |      15885 |     185832 | .......2BCDDDB6..1
| spindrain |    4 |           2500 |           25000 |       1360 |       3480 |       4363 |       6507 |       7416 |       8030 |     179266 | .......3BDEDB6...1
| spin  cb  |    4 |           2500 |           25000 |        766 |       1714 |       2134 |       3091 |       3453 |       3706 |     179642 | .......3CEDB61...1
| spindr cb |    4 |           2500 |           25000 |        647 |       1167 |       1417 |       1989 |       2248 |       2479 |     179613 | .......8DEC8.....1
| lockfree  |    4 |           2500 |           25000 |        568 |        685 |        716 |        791 |        833 |        868 |     172989 | ........DF4......1
| lockfr+dr |    4 |           2500 |           25000 |        557 |        680 |        708 |        758 |        780 |        807 |     184167 | ........DF3......1

| mutex     |    4 |           5000 |           50000 |       1139 |       2067 |       2589 |       5356 |       5954 |       6601 |     332032 | .......3BFFCB71..21
| mut+drain |    4 |           5000 |           50000 |       1123 |       1970 |       2423 |       5129 |       5619 |       6201 |     335871 | .......2CFFCB61..21
| mutex  cb |    4 |           5000 |           50000 |       1284 |       2180 |       3240 |       5657 |       6413 |       7601 |     338870 | ........BEFDB7...21
| mut+dr cb |    4 |           5000 |           50000 |       1291 |       2071 |       2383 |       5196 |       5655 |       6132 |     331809 | .......1BEFCB6...21
| spinlock  |    4 |           5000 |           50000 |       2177 |       6087 |       7799 |      11677 |      13600 |      14774 |     333260 | .......1BDEEEB71.21
| spindrain |    4 |           5000 |           50000 |       1343 |       3387 |       4266 |       6291 |       7226 |       7798 |     333819 | .......3CEFEC7...21
| spin  cb  |    4 |           5000 |           50000 |        686 |       1503 |       1865 |       2727 |       3095 |       3424 |     337774 | .......6EFEB6....21
| spindr cb |    4 |           5000 |           50000 |        578 |        946 |       1158 |       1680 |       1916 |       2103 |     336697 | .......AEFC82....21
| lockfree  |    4 |           5000 |           50000 |        591 |        708 |        742 |        810 |        835 |        861 |     325133 | ........EG4.....2.1
| lockfr+dr |    4 |           5000 |           50000 |        614 |        753 |        797 |        876 |        913 |        942 |     332643 | ........EG6......21

| mutex     |    4 |           7500 |           75000 |       1175 |       2156 |       2907 |       5573 |       6357 |       7268 |     488814 | .......3CFGDC82.222
| mut+drain |    4 |           7500 |           75000 |       1109 |       1939 |       2362 |       5101 |       5585 |       6031 |     483633 | .......3DFGCC71.222
| mutex  cb |    4 |           7500 |           75000 |       1281 |       2174 |       3190 |       5680 |       6669 |       7717 |     481382 | ........CFGDC8..222
| mut+dr cb |    4 |           7500 |           75000 |       1246 |       2005 |       2304 |       5053 |       5538 |       5987 |     477473 | ........CFGDB7..222
| spinlock  |    4 |           7500 |           75000 |       2322 |       6890 |       8876 |      13510 |      15565 |      16962 |     481618 | .......3CEFFED9.222
| spindrain |    4 |           7500 |           75000 |       1438 |       3774 |       4781 |       7111 |       8155 |       8965 |     479340 | .......4DFFFD91.222
| spin  cb  |    4 |           7500 |           75000 |        994 |       2074 |       2555 |       3799 |       4340 |       4811 |     480300 | .......6DGFD93..222
| spindr cb |    4 |           7500 |           75000 |        670 |       1142 |       1357 |       1920 |       2170 |       2346 |     482282 | .......9FGE91...222
| lockfree  |    4 |           7500 |           75000 |        588 |        708 |        745 |        830 |        869 |        907 |     463944 | .......1EG6....2.22
| lockfr+dr |    4 |           7500 |           75000 |        649 |        756 |        781 |        842 |        866 |        881 |     472450 | ........CH5.....222

| mutex     |    4 |          10000 |          100000 |       1147 |       2135 |       3289 |       5754 |       6775 |       7925 |     637200 | .......2DGGDC822.332
| mut+drain |    4 |          10000 |          100000 |       1120 |       1991 |       2526 |       5260 |       5875 |       6874 |     620813 | .......4DGGDC82.2231
| mutex  cb |    4 |          10000 |          100000 |       1195 |       2138 |       2801 |       5671 |       6465 |       7471 |     615712 | .......2DGGEC8..2231
| mut+dr cb |    4 |          10000 |          100000 |       1228 |       2054 |       2384 |       5277 |       5774 |       6466 |     611365 | .......2CFGDC81.2231
| spinlock  |    4 |          10000 |          100000 |       2333 |       6890 |       8919 |      13486 |      15310 |      16607 |     615526 | .......4DEFFFD912231
| spindrain |    4 |          10000 |          100000 |       1351 |       3655 |       4642 |       6987 |       8004 |       8808 |     622101 | .......7EFFFD92.2231
| spin  cb  |    4 |          10000 |          100000 |        886 |       1886 |       2357 |       3511 |       4056 |       4490 |     614831 | .......7EGFD93..2231
| spindr cb |    4 |          10000 |          100000 |        551 |        827 |        993 |       1394 |       1576 |       1723 |     620876 | .......BGGD7..2.2231
| lockfree  |    4 |          10000 |          100000 |        600 |        742 |        791 |        911 |        954 |        989 |     611856 | .......2FH8..2..2231
| lockfr+dr |    4 |          10000 |          100000 |        689 |        771 |        794 |        854 |        897 |        939 |     614989 | ........DH8..2..2231



                                                                                                                                                                                                                                                                                                [35/1934]
| mutex     |    4 |         100000 |         1000000 |       1176 |       2370 |       4157 |       6483 |       7606 |       8716 |    2256036 | .......8GJJHGC53456785
| mut+drain |    4 |         100000 |         1000000 |       1167 |       2244 |       3218 |       6069 |       7063 |       8045 |    2866891 | .......AHJJHGC43456788
| mutex  cb |    4 |         100000 |         1000000 |       1229 |       2396 |       4144 |       6646 |       7870 |       9029 |    2192656 | .......6GJJHGC53456785
| mut+dr cb |    4 |         100000 |         1000000 |       1224 |       2292 |       3252 |       6175 |       7192 |       8116 |    2180126 | .......9GJJHGC43456785
| spinlock  |    4 |         100000 |         1000000 |       1791 |       5630 |       7319 |      11347 |      13196 |      14699 |    2916485 | .......8GIJIIGB3456788
| spindrain |    4 |         100000 |         1000000 |       1478 |       4334 |       5571 |       8502 |       9814 |      10853 |    2315561 | ......1CHIJIHE74456786
| spin  cb  |    4 |         100000 |         1000000 |        844 |       2091 |       2676 |       4093 |       4722 |       5238 |    2367799 | .......AHJJHE733456786
| spindr cb |    4 |         100000 |         1000000 |        716 |       1577 |       2003 |       3050 |       3523 |       3905 |    2886736 | .......DIJIGB324456788
| lockfree  |    4 |         100000 |         1000000 |        645 |        730 |        758 |        863 |        915 |        954 |    2663686 | ........GKA34333456787
| lockfr+dr |    4 |         100000 |         1000000 |        685 |        764 |        780 |        822 |        864 |        916 |    2361060 | ........CKB22133456786


| name ---- | cpus | rate - msg/sec | total msg count | lat med ns | lat 90p ns | lat 95p ns | lat 99p ns |lat 99.5p ns|lat 99.7p ns| lat max ns | 0123456789ABCDEFGHIJKLMNOPQRSTUVXYZ log2 hist
| ----------|------|----------------|-----------------|------------|------------|------------|------------|------------|------------|------------|----------------------------------------------
| mutex     |    2 |             10 |             100 |       1067 |       1644 |       1769 |      30532 |      30532 |      30532 |      30532 | ........366.1.2
| mut+drain |    2 |             10 |             100 |       1094 |       1412 |       1799 |      25611 |      25611 |      25611 |      25611 | ........456.211
| mutex  cb |    2 |             10 |             100 |       1138 |       1612 |       1881 |      28123 |      28123 |      28123 |      28123 | ........457.1.1
| mut+dr cb |    2 |             10 |             100 |        954 |       1323 |       1423 |      29091 |      29091 |      29091 |      29091 | ........556.1.1
| spinlock  |    2 |             10 |             100 |        963 |       2363 |       3549 |      20979 |      20979 |      20979 |      20979 | ........6564211
| spindrain |    2 |             10 |             100 |        368 |        986 |       1853 |      21031 |      21031 |      21031 |      21031 | .......27532.11
| spin  cb  |    2 |             10 |             100 |        399 |        896 |       1218 |      21191 |      21191 |      21191 |      21191 | ........7531..1
| spindr cb |    2 |             10 |             100 |        369 |        526 |        669 |      22183 |      22183 |      22183 |      22183 | .......274.1..1
| lockfree  |    2 |             10 |             100 |        490 |        604 |        692 |      25732 |      25732 |      25732 |      25732 | ........66....1
| lockfr+dr |    2 |             10 |             100 |        543 |        734 |        801 |      26770 |      26770 |      26770 |      26770 | ........66....1

| mutex     |    2 |            100 |            1000 |       1106 |       1575 |       1648 |       2119 |       2249 |       6916 |      34186 | ........79A421.1
| mut+drain |    2 |            100 |            1000 |       1044 |       1260 |       1350 |       2007 |       4913 |       5686 |      30275 | .......289A2311
| mutex  cb |    2 |            100 |            1000 |       1019 |       1336 |       1544 |       1775 |       3248 |       5673 |      36616 | ........89922..1
| mut+dr cb |    2 |            100 |            1000 |       1052 |       1243 |       1291 |       1490 |       1771 |       6103 |      37654 | ........89A.2..1
| spinlock  |    2 |            100 |            1000 |        862 |       3478 |       4509 |       7267 |       8265 |       9486 |      32420 | ........9888722
| spindrain |    2 |            100 |            1000 |        443 |       1030 |       1330 |       2096 |       2884 |       3535 |      34863 | .......4A8741..1
| spin  cb  |    2 |            100 |            1000 |        400 |        721 |        867 |       1356 |       1770 |       2329 |      35832 | .......5A852...1
| spindr cb |    2 |            100 |            1000 |        386 |        554 |        710 |       1257 |       1440 |       2187 |      37254 | .......4A742...1
| lockfree  |    2 |            100 |            1000 |        474 |        598 |        630 |        723 |        741 |        772 |      29675 | ........A9....1
| lockfr+dr |    2 |            100 |            1000 |        514 |        755 |        801 |        848 |        863 |        942 |      32087 | .......199....1

| mutex     |    2 |           1000 |           10000 |       1095 |       1588 |       1675 |       1934 |       2109 |       2186 |     146739 | .......2ACD641...1
| mut+drain |    2 |           1000 |           10000 |        994 |       1225 |       1302 |       1628 |       1727 |       1804 |     145109 | .......6BDD321...1
| mutex  cb |    2 |           1000 |           10000 |       1060 |       1325 |       1418 |       1764 |       1846 |       1939 |     147357 | ........BCD421...1
| mut+dr cb |    2 |           1000 |           10000 |       1011 |       1225 |       1281 |       1442 |       1609 |       1721 |     149521 | ........CCD12....1
| spinlock  |    2 |           1000 |           10000 |       1001 |       4051 |       5489 |       8702 |      10627 |      11725 |     144283 | .......5CBCBA83..1
| spindrain |    2 |           1000 |           10000 |        395 |       1064 |       1364 |       2095 |       2391 |       2603 |     144218 | .......8DCA71....1
| spin  cb  |    2 |           1000 |           10000 |        460 |       1030 |       1266 |       1869 |       2120 |       2308 |     142935 | .......8DCA6.....1
| spindr cb |    2 |           1000 |           10000 |        381 |        478 |        504 |        611 |        682 |        728 |     148951 | .......8E921.....1
| lockfree  |    2 |           1000 |           10000 |        485 |        575 |        597 |        714 |        752 |        784 |     145919 | ........DC.......1
| lockfr+dr |    2 |           1000 |           10000 |        438 |        582 |        692 |        857 |        890 |        916 |     144727 | ........DC.......1

| mutex     |    2 |           2500 |           25000 |       1005 |       1505 |       1621 |       1847 |       2057 |       2171 |     331428 | .......4CEE751....1
| mut+drain |    2 |           2500 |           25000 |        980 |       1186 |       1257 |       1598 |       1698 |       1801 |     326635 | .......6CEE441....1
| mutex  cb |    2 |           2500 |           25000 |        999 |       1294 |       1433 |       1708 |       1768 |       1839 |     324964 | ........DEE52.....1
| mut+dr cb |    2 |           2500 |           25000 |       1009 |       1227 |       1281 |       1429 |       1549 |       1695 |     324426 | ........DDE42.....1
| spinlock  |    2 |           2500 |           25000 |        893 |       3448 |       4597 |       7295 |       8271 |       9181 |     321655 | .......6EDDDB81...1
| spindrain |    2 |           2500 |           25000 |        380 |        985 |       1294 |       1997 |       2314 |       2519 |     322016 | .......9FDC82.....1
| spin  cb  |    2 |           2500 |           25000 |        416 |        932 |       1159 |       1708 |       1941 |       2110 |     321404 | .......BEDB7......1
| spindr cb |    2 |           2500 |           25000 |        353 |        478 |        558 |        762 |        849 |        943 |     329965 | .......AFB61......1
| lockfree  |    2 |           2500 |           25000 |        451 |        553 |        577 |        638 |        666 |        689 |     318277 | ........FD........1
| lockfr+dr |    2 |           2500 |           25000 |        431 |        523 |        550 |        590 |        597 |        605 |     330348 | ........FC........1

| mutex     |    2 |           5000 |           50000 |       1025 |       1518 |       1610 |       1823 |       2050 |       2159 |     612501 | .......5DFF862...221
| mut+drain |    2 |           5000 |           50000 |        956 |       1191 |       1261 |       1592 |       1717 |       1810 |     608512 | .......7EFF651...221
| mutex  cb |    2 |           5000 |           50000 |        999 |       1239 |       1315 |       1656 |       1732 |       1791 |     757966 | ........EFF63....222
| mut+dr cb |    2 |           5000 |           50000 |        962 |       1194 |       1250 |       1441 |       1636 |       1720 |     609263 | ........EFF532...221
| spinlock  |    2 |           5000 |           50000 |        948 |       3656 |       4954 |       7839 |       9057 |       9981 |     616923 | .......8FEEEC93..221
| spindrain |    2 |           5000 |           50000 |        416 |       1065 |       1355 |       2014 |       2282 |       2471 |     607862 | .......AFED931...221
| spin  cb  |    2 |           5000 |           50000 |        449 |       1011 |       1244 |       1801 |       2077 |       2254 |     614145 | .......BFED922...221
| spindr cb |    2 |           5000 |           50000 |        359 |        485 |        568 |        765 |        840 |        908 |     612831 | .......CGC61.2...221
| lockfree  |    2 |           5000 |           50000 |        449 |        555 |        571 |        597 |        619 |        633 |     613962 | ........GE...2...221
| lockfr+dr |    2 |           5000 |           50000 |        525 |        594 |        610 |        737 |        798 |        830 |     613033 | .......2FF1..2...221

| mutex     |    2 |           7500 |           75000 |       1044 |       1558 |       1636 |       1931 |       2080 |       2221 |     920932 | .......5DFG971..2233
| mut+drain |    2 |           7500 |           75000 |        967 |       1181 |       1249 |       1587 |       1699 |       1797 |     883934 | .......8EGF661..2233
| mutex  cb |    2 |           7500 |           75000 |        988 |       1225 |       1352 |       1674 |       1741 |       1832 |     878829 | ........EFF73...2233
| mut+dr cb |    2 |           7500 |           75000 |        959 |       1165 |       1220 |       1386 |       1596 |       1675 |     879297 | ........FFF43...2233
| spinlock  |    2 |           7500 |           75000 |        915 |       3500 |       4685 |       7448 |       8619 |       9717 |     920251 | .......9FEEED93.2233
| spindrain |    2 |           7500 |           75000 |        436 |       1217 |       1576 |       2464 |       2865 |       3216 |     878712 | .......BGFEB61..2233
| spin  cb  |    2 |           7500 |           75000 |        458 |       1059 |       1331 |       1942 |       2200 |       2384 |     878965 | .......CGFDA2...2233
| spindr cb |    2 |           7500 |           75000 |        424 |        768 |        880 |       1310 |       1429 |       1537 |     885599 | .......BGFB5....2233
| lockfree  |    2 |           7500 |           75000 |        442 |        560 |        581 |        655 |        702 |        735 |     895541 | ........GE......2233
| lockfr+dr |    2 |           7500 |           75000 |        481 |        649 |        713 |        812 |        841 |        863 |     890161 | .......4GF......2233

| mutex     |    2 |          10000 |          100000 |       1050 |       1565 |       1647 |       1978 |       2122 |       2322 |     677821 | .......6DGGA81..2232
| mut+drain |    2 |          10000 |          100000 |        977 |       1190 |       1256 |       1601 |       1723 |       1838 |     633137 | .......7EGG7612..332
| mutex  cb |    2 |          10000 |          100000 |       1000 |       1258 |       1443 |       1682 |       1744 |       1811 |     631795 | ........EGG73.2..332
| mut+dr cb |    2 |          10000 |          100000 |        973 |       1206 |       1266 |       1458 |       1649 |       1720 |    1216983 | .......1FGG532..22342
| spinlock  |    2 |          10000 |          100000 |       1240 |       5415 |       7361 |      12047 |      14407 |      15982 |     617869 | .......8FEFFEC812231
| spindrain |    2 |          10000 |          100000 |        415 |        917 |       1212 |       1871 |       2178 |       2402 |    1191465 | .......CHFDA31..22342
| spin  cb  |    2 |          10000 |          100000 |        399 |        923 |       1166 |       1748 |       2021 |       2230 |    1154728 | .......CGFD91..2.3342
| spindr cb |    2 |          10000 |          100000 |        366 |        464 |        496 |        614 |        674 |        716 |     614293 | .......DHC41.2..2231
| lockfree  |    2 |          10000 |          100000 |        492 |        585 |        604 |        674 |        711 |        742 |     646543 | ........GG.....2.332
| lockfr+dr |    2 |          10000 |          100000 |        527 |        721 |        774 |        850 |        873 |        889 |     632233 | .......4GG2...2.1232

| mutex     |    2 |         100000 |         1000000 |       1017 |       1530 |       1641 |       4476 |       5218 |       6151 |    3882175 | .......9HJJEE523456789
| mut+drain |    2 |         100000 |         1000000 |        994 |       1206 |       1275 |       3608 |       4561 |       5098 |    3811269 | .......BHJJCD523456789
| mutex  cb |    2 |         100000 |         1000000 |        994 |       1217 |       1292 |       1646 |       1724 |       1795 |    4259456 | .......2IJJA62234567894
| mut+dr cb |    2 |         100000 |         1000000 |        975 |       1172 |       1219 |       1461 |       1603 |       1669 |    3786050 | .......3IJJ76323456789
| spinlock  |    2 |         100000 |         1000000 |       1000 |       4246 |       5695 |       9113 |      10752 |      12113 |    4270624 | .......EJIIIHE934567894
| spindrain |    2 |         100000 |         1000000 |        423 |        946 |       1230 |       1939 |       2272 |       2534 |    3816920 | ......3GKIHD7233456789
| spin  cb  |    2 |         100000 |         1000000 |        434 |        970 |       1210 |       1794 |       2077 |       2312 |    4274087 | .......FKJHD51334567894
| spindr cb |    2 |         100000 |         1000000 |        382 |        623 |        694 |        849 |        934 |       1015 |    3797389 | ......1FKIC2.223456789
| lockfree  |    2 |         100000 |         1000000 |        505 |        604 |        643 |        720 |        764 |        794 |    4294309 | ........KJ1222244567895
| lockfr+dr |    2 |         100000 |         1000000 |        591 |        690 |        795 |        909 |        926 |        936 |    3850472 | ........IK444123456789



```
done
