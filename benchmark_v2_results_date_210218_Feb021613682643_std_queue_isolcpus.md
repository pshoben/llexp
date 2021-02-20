
# Running Queue Latency Benchmark #2 (algorithm = std::queue with std::mutex minor tweaks)

All test runs were performed on [Target Architecture](target_architeture.md)

```
Run #1

|num threads| rate - msg/sec | total msg count | latency avg (ns) | latency stddev ns| latency sigma ns | latency 2sigma ns | latency 3sigma ns | hist 0123456789ABCDEFGHIJKLMNOPQRSTUV
|-----------|----------------|-----------------|------------------|------------------|------------------|-------------------|-------------------|--------------------------------------
|threads  4 | rate        10 | count       100 | avg         1121 | dev         2076 | sig         3197 | 2sig         5273 | 3sig         7349 | hist .......1454....
|threads  4 | rate        50 | count       500 | avg         1009 | dev         1390 | sig         2400 | 2sig         3791 | 3sig         5182 | hist .......3677321.
|threads  4 | rate       100 | count      1000 | avg          918 | dev          773 | sig         1691 | 2sig         2464 | 3sig         3237 | hist .......3788221
|threads  4 | rate       250 | count      2500 | avg          930 | dev          642 | sig         1573 | 2sig         2216 | 3sig         2858 | hist .......68A9432
|threads  4 | rate       500 | count      5000 | avg    261561529 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......79AA741.................
|threads  4 | rate      1000 | count     10000 | avg          939 | dev          735 | sig         1674 | 2sig         2409 | 3sig         3145 | hist .......8ACB742..
|threads  4 | rate      2500 | count     25000 | avg     52520493 | dev     13856039 | sig     66376532 | 2sig     80232572 | 3sig     94088611 | hist .......4CDD841..................
|threads  4 | rate      5000 | count     50000 | avg     26317515 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......9DEDB72..1..............
|threads  4 | rate     10000 | count    100000 | avg     13190693 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......AEFEA611..22..........
|threads  4 | rate    100000 | count    100000 | avg     13228872 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......7EFFB81.22342............
|threads  4 | rate   1000000 | count   1000000 | avg      1325469 | dev       882817 | sig      2208287 | 2sig      3091105 | 3sig      3973922 | hist .......EIIIEA.2.234564.........

|threads  2 | rate        10 | count       100 | avg          859 | dev         1519 | sig         2379 | 2sig         3898 | 3sig         5418 | hist .......35521..
|threads  2 | rate        50 | count       500 | avg          909 | dev          945 | sig         1854 | 2sig         2800 | 3sig         3745 | hist .......3776331
|threads  2 | rate       100 | count      1000 | avg          733 | dev          664 | sig         1397 | 2sig         2061 | 3sig         2725 | hist .......688743.
|threads  2 | rate       250 | count      2500 | avg          835 | dev          655 | sig         1490 | 2sig         2146 | 3sig         2801 | hist .......6999531
|threads  2 | rate       500 | count      5000 | avg          856 | dev          847 | sig         1703 | 2sig         2550 | 3sig         3398 | hist .......7ABA64...
|threads  2 | rate      1000 | count     10000 | avg          951 | dev         1095 | sig         2046 | 2sig         3141 | 3sig         4236 | hist ........BBB76....
|threads  2 | rate      2500 | count     25000 | avg          941 | dev         1978 | sig         2920 | 2sig         4899 | 3sig         6877 | hist .......4CDD97......
|threads  2 | rate      5000 | count     50000 | avg          742 | dev         3154 | sig         3896 | 2sig         7050 | 3sig        10204 | hist .......CEED97..1..1.
|threads  2 | rate     10000 | count    100000 | avg          827 | dev         3127 | sig         3955 | 2sig         7082 | 3sig        10210 | hist .......DDFEB7....12.
|threads  2 | rate    100000 | count    100000 | avg          715 | dev         4202 | sig         4918 | 2sig         9120 | 3sig        13323 | hist ......4DFFDA7..1123.
|threads  2 | rate   1000000 | count   1000000 | avg         1086 | dev        25595 | sig        26682 | 2sig        52278 | 3sig        77874 | hist ......1HHIHFB21.223455


Run #2

|num threads| rate - msg/sec | total msg count | latency avg (ns) | latency stddev ns| latency sigma ns | latency 2sigma ns | latency 3sigma ns | hist 0123456789ABCDEFGHIJKLMNOPQRSTUV
|-----------|----------------|-----------------|------------------|------------------|------------------|-------------------|-------------------|--------------------------------------
|threads  4 | rate        10 | count       100 | avg  14078837826 | dev    241883137 | sig  14320720964 | 2sig  14562604102 | 3sig  14804487240 | hist ........4541....................
|threads  4 | rate        50 | count       500 | avg          920 | dev          726 | sig         1647 | 2sig         2374 | 3sig         3101 | hist .......367722.
|threads  4 | rate       100 | count      1000 | avg   1412892008 | dev     74761949 | sig   1487653958 | 2sig   1562415908 | 3sig   1637177858 | hist ........788532..................
|threads  4 | rate       250 | count      2500 | avg          930 | dev          629 | sig         1559 | 2sig         2189 | 3sig         2819 | hist .......5999631
|threads  4 | rate       500 | count      5000 | avg          944 | dev          552 | sig         1496 | 2sig         2048 | 3sig         2600 | hist .......2AAA631
|threads  4 | rate      1000 | count     10000 | avg          935 | dev          644 | sig         1579 | 2sig         2224 | 3sig         2868 | hist .......7ACB741..
|threads  4 | rate      2500 | count     25000 | avg     56926788 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......ACCD94.1................
|threads  4 | rate      5000 | count     50000 | avg     28520847 | dev      9859838 | sig     38380685 | 2sig     48240523 | 3sig     58100362 | hist .......ADEEB71.2..........
|threads  4 | rate     10000 | count    100000 | avg     14291887 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......AEFEA6111.23.............
|threads  4 | rate    100000 | count    100000 | avg     14330458 | dev      5948401 | sig     20278859 | 2sig     26227261 | 3sig     32175663 | hist .......BEFEB7..12242............
|threads  4 | rate   1000000 | count   1000000 | avg      1435770 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist ......6EHIIFA.2.234565..........

|threads  2 | rate        10 | count       100 | avg          953 | dev         1975 | sig         2928 | 2sig         4904 | 3sig         6879 | hist .......345211..
|threads  2 | rate        50 | count       500 | avg          759 | dev          840 | sig         1600 | 2sig         2440 | 3sig         3281 | hist .......577542.
|threads  2 | rate       100 | count      1000 | avg          822 | dev          700 | sig         1522 | 2sig         2223 | 3sig         2923 | hist .......488743.
|threads  2 | rate       250 | count      2500 | avg          713 | dev          638 | sig         1351 | 2sig         1989 | 3sig         2627 | hist .......8998531
|threads  2 | rate       500 | count      5000 | avg          787 | dev          896 | sig         1683 | 2sig         2580 | 3sig         3476 | hist .......8AA974...
|threads  2 | rate      1000 | count     10000 | avg          917 | dev         1236 | sig         2154 | 2sig         3391 | 3sig         4628 | hist ........BBB85....
|threads  2 | rate      2500 | count     25000 | avg          887 | dev         1842 | sig         2729 | 2sig         4572 | 3sig         6414 | hist .......ABDC96......
|threads  2 | rate      5000 | count     50000 | avg          738 | dev         3180 | sig         3918 | 2sig         7098 | 3sig        10278 | hist .......CEECA6..1..1.
|threads  2 | rate     10000 | count    100000 | avg          872 | dev         8214 | sig         9087 | 2sig        17302 | 3sig        25517 | hist .......BFFDB7...1113.
|threads  2 | rate    100000 | count    100000 | avg          959 | dev         4804 | sig         5764 | 2sig        10568 | 3sig        15373 | hist ........EFEB8..11231
|threads  2 | rate   1000000 | count   1000000 | avg         1033 | dev        25551 | sig        26584 | 2sig        52135 | 3sig        77687 | hist .......FIIHEA.1.223455

```
