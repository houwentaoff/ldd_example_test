# perf 使用
## riscv集成perf  
* 需要在内核enable如下菜单  
```
[root@xxxxE:~]# zcat /proc/config.gz |grep PMU
# CONFIG_I2C_MUX_GPMUX is not set
CONFIG_RISCV_PMU=y
CONFIG_RISCV_PMU_LEGACY=y
CONFIG_RISCV_PMU_SBI=y

```
## perf 范例
```
[root@MSV100_MODULE:~]# ./perf stat ls
a.data
a.log
a.unfold
ld-linux-riscv64-lp64d.so.1
ld-linux-riscv64-lp64d_andes-45-series.so.1
libasm-0.178.so
libasm.so
libc.so.6
libdw-0.178.so
libdw.so.1
libelf-0.178.so
libelf.so.1
libm.so.6
libz.so
libz.so.1
perf
readme.md

 Performance counter stats for 'ls':

              3.81 msec task-clock                       #    0.716 CPUs utilized          
                 2      context-switches                 #  524.285 /sec                   
                 0      cpu-migrations                   #    0.000 /sec                   
                59      page-faults                      #   15.466 K/sec                  
           4535022      cycles                           #    1.189 GHz                    
           1727645      instructions                     #    0.38  insn per cycle         
            195321      branches                         #   51.202 M/sec                  
             43994      branch-misses                    #   22.52% of all branches        

       0.005328400 seconds time elapsed

       0.005650000 seconds user
       0.000000000 seconds sys

```
```
./perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-prefetch-misses,L1-dcache-prefetches,L1-dcache-store-misses,L1-dcache-stores,L1-icache-load-misses,L1-icache-loads,L1-icache-prefetch-misses    -p 370
./perf list
./perf top -p 228 -K
./perf top -p 221 -U
```
``` 结果如下
   PerfTop:    3585 irqs/sec  kernel:99.7%  exact:  0.0% lost: 0/0 drop: 0/0 [1000Hz cycles],  (all, 4 CPUs)
-------------------------------------------------------------------------------

     8.66%  perf           [.] rb_next
     3.66%  perf           [.] map__process_kallsym_symbol
     3.65%  [kernel]       [k] format_decode
     3.17%  libc.so.6      [.] strchr
     2.68%  [kernel]       [k] vsnprintf
     2.67%  perf           [.] kallsyms__parse
     2.50%  [kernel]       [k] number
     2.49%  [kernel]       [k] finish_task_switch.isra.0
     2.37%  libc.so.6      [.] _int_malloc
     2.20%  libc.so.6      [.] memset
     2.17%  perf           [.] __symbols__insert
     1.98%  [kernel]       [k] kallsyms_expand_symbol.constprop.0
     1.84%  libc.so.6      [.] _int_free
     1.67%  libc.so.6      [.] __libc_calloc
     1.52%  [kernel]       [k] _raw_spin_unlock_irq
     1.50%  libc.so.6      [.] memcpy
     1.50%  libc.so.6      [.] strlen
     1.49%  perf           [.] rb_insert_color
     1.34%  [kernel]       [k] rt_spin_unlock
     1.31%  perf           [.] symbol__new

```
```
./perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-prefetch-misses,L1-dcache-prefetches,L1-dcache-store-misses,L1-dcache-stres,L1-icache-load-misses,L1-icache-loads,L1-icache-prefetch-misses    -p 502
^C
 Performance counter stats for process id '502':

           2665454      L1-dcache-load-misses            #    0.40% of all L1-dcache accesses  (11.22%)
         671112131      L1-dcache-loads                                                      (22.34%)
     <not counted>      L1-dcache-prefetch-misses                                     (0.00%)
     <not counted>      L1-dcache-prefetches                                          (0.00%)
           2559853      L1-dcache-store-misses                                               (11.06%)
         412702214      L1-dcache-stores                                                     (22.29%)
          52654233      L1-icache-load-misses            #    2.09% of all L1-icache accesses  (33.35%)
        2513851128      L1-icache-loads                                                      (44.42%)
     <not counted>      L1-icache-prefetch-misses                                     (0.00%)

       5.691408000 seconds time elapsed

```
## 使用perf产生火焰图
### Target:  
`./perf record  -p 283 -a -g -o a.data  -- sleep 10`  #10s monitor  
`./perf script -i a.data  &> a.unfold`  

### PC:  
```
sudo apt-get install c++filt
git clone https://github.com/brendangregg/FlameGraph.git
cd FlameGraph
./stackcollapse-perf.pl a.unfold |c++filt &> a.folded
./flamegraph.pl a.folded  > a.svg  ## 最后生成文件
```
### 说明
* 经过上述命令后使用浏览器打开a.svg便可用浏览器打开火焰图文件a.svg，火焰图平顶部分是性能的关键所在
 火焰图 如下
 
