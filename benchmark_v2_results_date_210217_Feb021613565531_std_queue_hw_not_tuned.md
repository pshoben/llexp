
# Running Queue Latency Benchmark #2 (algorithm = std::queue with std::mutex minor tweaks)

Architecture:        x86_64
CPU op-mode(s):      32-bit, 64-bit
Byte Order:          Little Endian
CPU(s):              20
On-line CPU(s) list: 0-19
Thread(s) per core:  2
Core(s) per socket:  10
Socket(s):           1
NUMA node(s):        1
Vendor ID:           GenuineIntel
CPU family:          6
Model:               85
Model name:          Intel(R) Xeon(R) Silver 4210R CPU @ 2.40GHz
Stepping:            7
CPU MHz:             1018.466
CPU max MHz:         3200.0000
CPU min MHz:         1000.0000
BogoMIPS:            4800.00
Virtualization:      VT-x
L1d cache:           32K
L1i cache:           32K
L2 cache:            1024K
L3 cache:            14080K
NUMA node0 CPU(s):   0-19
Flags:               fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx smx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid dca sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb cat_l3 cdp_l3 invpcid_single intel_ppin ssbd mba ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 hle avx2 smep bmi2 erms invpcid cqm mpx rdt_a avx512f avx512dq rdseed adx smap clflushopt clwb intel_pt avx512cd avx512bw avx512vl xsaveopt xsavec xgetbv1 xsaves cqm_llc cqm_occup_llc cqm_mbm_total cqm_mbm_local dtherm ida arat pln pts pku ospke avx512_vnni md_clear flush_l1d arch_capabilities

## Run #1

|num threads| rate - msg/sec | total msg count | latency avg (ns) | latency stddev ns| latency sigma ns | latency 2sigma ns | latency 3sigma ns | hist 0123456789ABCDEFGHIJKLMNOPQRSTUV
|-----------|----------------|-----------------|------------------|------------------|------------------|-------------------|-------------------|--------------------------------------
|threads  4 | rate        10 | count       100 | avg         3852 | dev         5170 | sig         9022 | 2sig        14192 | 3sig        19363 | hist .......13344422.
|threads  4 | rate        50 | count       500 | avg         4997 | dev         5627 | sig        10624 | 2sig        16252 | 3sig        21879 | hist .......14667655
|threads  4 | rate       100 | count      1000 | avg   6214466918 | dev     83702457 | sig   6298169375 | 2sig   6381871833 | 3sig   6465574291 | hist .......267777551................
|threads  4 | rate       250 | count      2500 | avg   2486798467 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......58898856.................
|threads  4 | rate       500 | count      5000 | avg   1243915466 | dev     21289797 | sig   1265205264 | 2sig   1286495062 | 3sig   1307784860 | hist .......489AA987..............
|threads  4 | rate      1000 | count     10000 | avg    622218791 | dev     29916044 | sig    652134835 | 2sig    682050879 | 3sig    711966923 | hist .......7BCB541..................
|threads  4 | rate      2500 | count     25000 | avg    248993453 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......5CDC852.................
|threads  4 | rate      5000 | count     50000 | avg    124554209 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......6DEEB82.11...............
|threads  4 | rate     10000 | count    100000 | avg     62308593 | dev      8244831 | sig     70553425 | 2sig     78798257 | 3sig     87043088 | hist .......5EFFA7.1.113.............
|threads  4 | rate    100000 | count    100000 | avg     62346913 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......BEFFB7.111341............
|threads  4 | rate   1000000 | count   1000000 | avg      6237310 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......DHIIFB3.1234564..........

|threads  2 | rate        10 | count       100 | avg         4685 | dev         4216 | sig         8902 | 2sig        13118 | 3sig        17335 | hist ........1.35422
|threads  2 | rate        50 | count       500 | avg         4896 | dev         4249 | sig         9146 | 2sig        13395 | 3sig        17645 | hist ........1247745
|threads  2 | rate       100 | count      1000 | avg         4419 | dev         3971 | sig         8390 | 2sig        12362 | 3sig        16333 | hist .........468855
|threads  2 | rate       250 | count      2500 | avg         4551 | dev         4136 | sig         8687 | 2sig        12823 | 3sig        16959 | hist ........358A977
|threads  2 | rate       500 | count      5000 | avg         4462 | dev         4172 | sig         8634 | 2sig        12806 | 3sig        16979 | hist .......4569BA87.
|threads  2 | rate      1000 | count     10000 | avg          894 | dev         1143 | sig         2038 | 2sig         3181 | 3sig         4325 | hist ........BBB85....
|threads  2 | rate      2500 | count     25000 | avg          631 | dev         1713 | sig         2345 | 2sig         4059 | 3sig         5773 | hist .......CCDA95.....
|threads  2 | rate      5000 | count     50000 | avg          732 | dev         3025 | sig         3757 | 2sig         6782 | 3sig         9808 | hist .......CEDDA5.1...1.
|threads  2 | rate     10000 | count    100000 | avg          729 | dev         3301 | sig         4030 | 2sig         7331 | 3sig        10633 | hist ......6EEFCB7....12.
|threads  2 | rate    100000 | count    100000 | avg          767 | dev         4182 | sig         4950 | 2sig         9132 | 3sig        13315 | hist .......CFFE96211123.
|threads  2 | rate   1000000 | count   1000000 | avg         1095 | dev        25225 | sig        26321 | 2sig        51546 | 3sig        76772 | hist .......GHIHF922.123455


## Run #2

|num threads| rate - msg/sec | total msg count | latency avg (ns) | latency stddev ns| latency sigma ns | latency 2sigma ns | latency 3sigma ns | hist 0123456789ABCDEFGHIJKLMNOPQRSTUV
|-----------|----------------|-----------------|------------------|------------------|------------------|-------------------|-------------------|--------------------------------------
|threads  4 | rate        10 | count       100 | avg         3483 | dev         4653 | sig         8137 | 2sig        12791 | 3sig        17445 | hist ........2444412
|threads  4 | rate        50 | count       500 | avg  12647817377 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......25676554................
|threads  4 | rate       100 | count      1000 | avg         3527 | dev         4269 | sig         7797 | 2sig        12067 | 3sig        16337 | hist .......26787755
|threads  4 | rate       250 | count      2500 | avg   2531575260 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......58898856.................
|threads  4 | rate       500 | count      5000 | avg   1266305348 | dev     39184491 | sig   1305489839 | 2sig   1344674331 | 3sig   1383858823 | hist .......599AA9871..............
|threads  4 | rate      1000 | count     10000 | avg    633416848 | dev     10944395 | sig    644361244 | 2sig    655305639 | 3sig    666250034 | hist .......7ABC62...................
|threads  4 | rate      2500 | count     25000 | avg    253472485 | dev      6598542 | sig    260071028 | 2sig    266669570 | 3sig    273268113 | hist .......8CDC84..................
|threads  4 | rate      5000 | count     50000 | avg    126793500 | dev     12918342 | sig    139711843 | 2sig    152630186 | 3sig    165548528 | hist .......9DEEB7.2..1..........
|threads  4 | rate     10000 | count    100000 | avg     63428279 | dev      6704168 | sig     70132448 | 2sig     76836616 | 3sig     83540785 | hist .......AEFEA72...23.............
|threads  4 | rate    100000 | count    100000 | avg     63466717 | dev      8529734 | sig     71996451 | 2sig     80526185 | 3sig     89055919 | hist .......7EFFB8.212341............
|threads  4 | rate   1000000 | count   1000000 | avg      6349239 | dev         -nan | sig         -nan | 2sig         -nan | 3sig         -nan | hist .......DHIIFB32.333464......

|threads  2 | rate        10 | count       100 | avg         4321 | dev         4604 | sig         8925 | 2sig        13530 | 3sig        18134 | hist .......11335422
|threads  2 | rate        50 | count       500 | avg         4870 | dev         4667 | sig         9537 | 2sig        14205 | 3sig        18873 | hist .......21357654
|threads  2 | rate       100 | count      1000 | avg         4531 | dev         4410 | sig         8941 | 2sig        13351 | 3sig        17761 | hist .......1.478765
|threads  2 | rate       250 | count      2500 | avg         4741 | dev         4328 | sig         9069 | 2sig        13397 | 3sig        17725 | hist .......2147A977
|threads  2 | rate       500 | count      5000 | avg         4441 | dev         4244 | sig         8686 | 2sig        12930 | 3sig        17175 | hist .......4469BA87.
|threads  2 | rate      1000 | count     10000 | avg          892 | dev         1142 | sig         2035 | 2sig         3177 | 3sig         4320 | hist ........BBB84....
|threads  2 | rate      2500 | count     25000 | avg          637 | dev         1777 | sig         2415 | 2sig         4192 | 3sig         5970 | hist .......CCD994......
|threads  2 | rate      5000 | count     50000 | avg          807 | dev         3108 | sig         3915 | 2sig         7023 | 3sig        10131 | hist .......AEEDA6..1..1.
|threads  2 | rate     10000 | count    100000 | avg          917 | dev         3112 | sig         4030 | 2sig         7142 | 3sig        10254 | hist .......2FFEB7....12.
|threads  2 | rate    100000 | count    100000 | avg         1005 | dev        12532 | sig        13537 | 2sig        26070 | 3sig        38603 | hist .......CEFDB62.111341
|threads  2 | rate   1000000 | count   1000000 | avg         1110 | dev        27145 | sig        28255 | 2sig        55401 | 3sig        82546 | hist .......GHIHFA32.123456

done
