
# Running Queue Latency Microbenchmark : side-by-side compare algorithms

All test runs performed on [Target Hardware](target_architecture.md)

```

| name ---- | cpus | tot msg |targ m/s |actl m/s | lat med ns | lat 90p ns | lat 95p ns | lat 99p ns |lat 99.5p ns|lat 99.7p ns|lat 99.9p ns| lat max ns | throughput msg/sec log2 hist --- | latency log2 hist
| ----------|------|---------|---------|---------|------------|------------|------------|------------|------------|------------|------------|------------|----------------------------------|----------------------------
| mutex     |    4 |     100 |      10 |      10 |       1146 |       2423 |       6126 |      35510 |      35510 |      35510 |      35510 |      35510 | .145..........4....0.0           | ........155310.1                
| mut+drain |    4 |     100 |      10 |      10 |       1306 |       2192 |       6309 |      21558 |      21558 |      21558 |      21558 |      21558 | .036...........4....00           | ........1452201                 
| mutex  cb |    4 |     100 |      10 |      10 |       1143 |       2573 |       5284 |      21576 |      21576 |      21576 |      21576 |      21576 | .235...........4.....1           | ........25532.1                 
| mut+dr cb |    4 |     100 |      10 |      11 |        907 |       1794 |       2566 |      27092 |      27092 |      27092 |      27092 |      27092 | .135...........4.0...0           | ........4542001                 
| spinlock  |    4 |     100 |      10 |      11 |       1143 |       4732 |       8365 |      24164 |      24164 |      24164 |      24164 |      24164 | .135..........24...0.0           | ........4444211                 
| spindrain |    4 |     100 |      10 |      10 |       1299 |       3400 |       6066 |      21298 |      21298 |      21298 |      21298 |      21298 | 0135...........4...00            | ........34442.1                 
| spin  cb  |    4 |     100 |      10 |      10 |       1223 |       2987 |       3690 |      23520 |      23520 |      23520 |      23520 |      23520 | .235...........4...1             | ........35541.1                 
| spindr cb |    4 |     100 |      10 |      10 |        628 |       1073 |       1599 |      28467 |      28467 |      28467 |      28467 |      28467 | .235...........4....1            | ........4530..1                 
| lockfree  |    4 |     100 |      10 |      11 |        592 |        705 |        752 |      21356 |      21356 |      21356 |      21356 |      21356 | .235...........4......1          | ........46....1                 
| lockfr+dr |    4 |     100 |      10 |      10 |        578 |        730 |        776 |      20755 |      20755 |      20755 |      20755 |      20755 | .235...........4.....00          | ........46....1                 

| mutex     |    4 |    1000 |     100 |     101 |       1186 |       2178 |       2794 |       5654 |       7552 |      18188 |      30858 |      30858 | ....379.......71...0.0           | ........5886401                 
| mut+drain |    4 |    1000 |     100 |     101 |       1150 |       2089 |       2310 |       5570 |       6853 |      12752 |      27502 |      27502 | ....379.......67.....1           | ........6886401                 
| mutex  cb |    4 |    1000 |     100 |     102 |        991 |       1923 |       2525 |       5605 |       7272 |       7380 |      27360 |      27360 | ....379.......67...0.0           | ........68854.1                 
| mut+dr cb |    4 |    1000 |     100 |     100 |        938 |       1737 |       2096 |       4782 |       6034 |       7140 |      30299 |      30299 | ....379.......71....00           | ........68853.1                 
| spinlock  |    4 |    1000 |     100 |     101 |       1822 |       5768 |       7571 |      12084 |      14974 |      17983 |      29370 |      29370 | ...0279.......76...00            | ........5788751                 
| spindrain |    4 |    1000 |     100 |     104 |       1160 |       3149 |       4191 |       6809 |       8079 |       8581 |      29435 |      29435 | ....379.......75...0..0          | ........6887511                 
| spin  cb  |    4 |    1000 |     100 |     100 |       1098 |       3078 |       3884 |       6105 |       7466 |       9185 |      30240 |      30240 | ....379.......74....1            | ........5887501                 
| spindr cb |    4 |    1000 |     100 |     103 |        691 |       1506 |       1766 |       2552 |       3113 |       4340 |      29353 |      29353 | ...0379.......72...0..0          | .......079740.1                 
| lockfree  |    4 |    1000 |     100 |     105 |        623 |        703 |        727 |        782 |        823 |        860 |      31748 |      31748 | ....479.......8......00          | ........69....1                 
| lockfr+dr |    4 |    1000 |     100 |     108 |        558 |        678 |        704 |        745 |        781 |        845 |      28848 |      28848 | ..01369.......8......00          | ........89....1                 

| mutex     |    4 |   10000 |    1000 |    1018 |       1146 |       2140 |       2762 |       5590 |       5981 |       6507 |       9623 |      90935 | ......16ABB..B......00           | ........8BC984..1               
| mut+drain |    4 |   10000 |    1000 |    1001 |       1103 |       1948 |       2325 |       5242 |       5760 |       6277 |       9225 |      94899 | ......16ABB..B......00           | ........9BC973..1               
| mutex  cb |    4 |   10000 |    1000 |    1000 |       1035 |       1948 |       2502 |       5380 |       5915 |       6194 |       8668 |      95779 | .......6ABB..B.....0.0           | .......09CC983..1               
| mut+dr cb |    4 |   10000 |    1000 |    1011 |        959 |       1767 |       2154 |       4837 |       5371 |       5791 |       7548 |      88377 | .......7ABB..B.....0.0           | .......0ACB871..1               
| spinlock  |    4 |   10000 |    1000 |    1032 |       2069 |       6137 |       7946 |      12071 |      14048 |      15219 |      18973 |      97527 | ......05ABB..B.....00            | ........8ABBA84.1               
| spindrain |    4 |   10000 |    1000 |    1023 |       1341 |       3727 |       4787 |       7576 |       8595 |       9590 |      10929 |      96413 | ......06ABB..B.....0.0           | .......19BBB95..1               
| spin  cb  |    4 |   10000 |    1000 |    1012 |       1240 |       3444 |       4414 |       6545 |       7413 |       7972 |       9583 |      92511 | .......6ABB..B.....0.0           | .......09BBB94..1               
| spindr cb |    4 |   10000 |    1000 |    1006 |        680 |       1311 |       1598 |       2275 |       2571 |       2737 |       3504 |      93903 | ......06ABB..B.....0.0           | .......2ACA70...1               
| lockfree  |    4 |   10000 |    1000 |    1045 |        540 |        648 |        676 |        734 |        767 |        780 |        810 |      90407 | ......06ABB..B.......1           | ........BC......1               
| lockfr+dr |    4 |   10000 |    1000 |    1084 |        564 |        658 |        689 |        743 |        764 |        776 |        830 |      91680 | ......379BB..B.......1           | ........BC......1               

| mutex     |    4 |  100000 |   10000 |   10073 |       1166 |       2143 |       2827 |       5612 |       6213 |       6974 |       9477 |     650140 | .........07CEEF....0.0           | .......1CFFCB7.0.111            
| mut+drain |    4 |  100000 |   10000 |   10005 |       1102 |       1952 |       2328 |       5096 |       5467 |       5769 |       8160 |     620783 | .........07CEF4E3...00           | .......2CFFCB60.0011            
| mutex  cb |    4 |  100000 |   10000 |   10035 |        999 |       1882 |       2308 |       5095 |       5568 |       5984 |       8335 |     626190 | ..........7CEFCE....00           | .......4CFFCB60.0011            
| mut+dr cb |    4 |  100000 |   10000 |   10090 |        938 |       1705 |       2015 |       4475 |       5046 |       5332 |       6123 |     628125 | ..........7CEFEB.....1           | .......5DFFBA40.0011            
| spinlock  |    4 |  100000 |   10000 |   10401 |       1960 |       5772 |       7423 |      11178 |      13066 |      14184 |      16785 |     628547 | ..........7CEFEC3..1             | .......1CEEEEB6.0011            
| spindrain |    4 |  100000 |   10000 |   10125 |       1231 |       3252 |       4150 |       6264 |       7253 |       7976 |       9516 |     626691 | ..........7CEFCE...0..0          | .......3DEFEC71.0011            
| spin  cb  |    4 |  100000 |   10000 |   10266 |       1076 |       2913 |       3740 |       5647 |       6497 |       7130 |       8581 |     636049 | ..........7CEEF2...0.0           | .......0CFEEB6.0.111            
| spindr cb |    4 |  100000 |   10000 |   10019 |        788 |       1611 |       1954 |       2775 |       3112 |       3353 |       3941 |     632881 | ..........7CEFE....0.0           | .......4DFEB5..0.111            
| lockfree  |    4 |  100000 |   10000 |   10309 |        530 |        626 |        652 |        697 |        716 |        730 |        765 |     617630 | .........18CDF.E.....1           | ........FF....0.0011            
| lockfr+dr |    4 |  100000 |   10000 |   10653 |        545 |        661 |        694 |        745 |        766 |        785 |        823 |     607915 | ........269BDF..E....1           | ........FF..0...0011            

| mutex     |    4 | 1000000 |  100000 |  100058 |       1182 |       2371 |       4383 |       6437 |       7462 |       8410 |      10706 |    2726594 | ............09EHIGGGFDCBAA7754.E | .......5FIIGFB42234566          
| mut+drain |    4 | 1000000 |  100000 |  100325 |       1113 |       1951 |       2368 |       5286 |       5926 |       6851 |       9578 |    2368756 | ............09EHIHHGDBA8773322.D | .......6GIIFEA21234564          
| mutex  cb |    4 | 1000000 |  100000 |  100448 |        989 |       1936 |       2551 |       5467 |       6123 |       6981 |       9338 |    2353418 | .............8EHIGHGDBA8875432.D | .......AGIIFFA22234564          
| mut+dr cb |    4 | 1000000 |  100000 |  100563 |        935 |       1715 |       2049 |       4812 |       5344 |       5766 |       7552 |    2868066 | .............9EHIGHGECA8775431.D | .......AGIIEE821234566          
| spinlock  |    4 | 1000000 |  100000 |  100228 |       2022 |       5670 |       7224 |      10945 |      12592 |      13831 |      16746 |    2363931 | ............19FHIHGFEDCAAA6543.G | .......3EHIIHE92234564          
| spindrain |    4 | 1000000 |  100000 |  101486 |       1233 |       3416 |       4416 |       7007 |       8111 |       8936 |      10723 |    2181557 | .............8EHIIGDCA97674211.E | .......7GIIHFC41234563          
| spin  cb  |    4 | 1000000 |  100000 |  102139 |       1165 |       3207 |       4159 |       6516 |       7572 |       8325 |      10158 |    2358575 | .............8EHIIGECB97773220.E | .......6GIIHFB31234564          
| spindr cb |    4 | 1000000 |  100000 |  102508 |        759 |       1575 |       1956 |       2862 |       3216 |       3473 |       4077 |    2925447 | .............8EHIDEHGDB9895421.E | .......4HJHF9.11234566          
| lockfree  |    4 | 1000000 |  100000 |  105655 |        621 |        707 |        732 |        770 |        784 |        794 |        815 |    2672379 | .............8DHIII.00           | ........GJ.10111234565          
| lockfr+dr |    4 | 1000000 |  100000 |  100880 |        676 |        754 |        767 |        793 |        803 |        810 |        827 |    2875760 | .............7DHIHH.0.0          | ........9J.01102234566          

| mutex     |    4 | 1000000 | 1000000 | 1000722 |      16885 |      47132 |      60942 |     135328 |     359206 |     739502 |    1777882 |    2417261 | ............0.3AEGGHGGGFHCA987.G | .......3ADEGHHIHECAAA8          
| mut+drain |    4 | 1000000 | 1000000 | 1008232 |       1654 |       4072 |       5639 |       9209 |      11411 |      14319 |    1383565 |    2372788 | ............0.4BEGGGGGGFICA987.G | .......AFHIIGD756789A8          
| mutex  cb |    4 | 1000000 | 1000000 |  525573 |       2553 |       6178 |       7556 |      10926 |      12750 |      14544 |     181781 |    1000141 | 0..............8DGGHGGHFHCAA98.G | .......8EGHIHF968889            
| mut+dr cb |    4 | 1000000 | 1000000 | 1453719 |       1172 |       2627 |       4053 |       6748 |       8095 |       9357 |      17690 |    1001901 | ..............3AEGGGGGHFHCBA98.G | .......DGIIHFB556788            
| spinlock  |    4 | 1000000 | 1000000 |  452211 |       4576 |      11559 |      14590 |      26010 |     520124 |     841507 |    2152304 |    2893078 | 0...........018DFGGFFHHGD32001.G | .......19EHIIHE868ABAA          
| spindrain |    4 | 1000000 | 1000000 |  501586 |       1801 |       5191 |       6867 |      11565 |      14612 |      23441 |    1909812 |    2899081 | 0...........0.7CEGGGFHIGD86543.G | .......8EHIIHE956789A9          
| spin  cb  |    4 | 1000000 | 1000000 |  598853 |       4110 |      11001 |      14251 |      24499 |      29990 |      34488 |     255955 |    1000352 | 0.............2BEGGGGHHGB65332.H | ........BFHIIHEA8889            
| spindr cb |    4 | 1000000 | 1000000 |  650847 |       1310 |       3856 |       5021 |       7854 |       9195 |      10367 |      21291 |    1001504 | 0.............3AEGGGGHHGC97765.H | .......AGIIHGC656788            
| lockfree  |    4 | 1000000 | 1000000 | 1011643 |        827 |       1175 |       1297 |       1578 |       1794 |     186605 |    1358566 |    2169616 | ................09FIIGEBCD9865.E | ........EJH735667899A6          
| lockfr+dr |    4 | 1000000 | 1000000 | 1004211 |       1033 |       1661 |       2002 |       2940 |       3600 |     157760 |    1532507 |    2380744 | ................1AFIIGDAAB7654.D | ........DIIF94567899A8          



```
done