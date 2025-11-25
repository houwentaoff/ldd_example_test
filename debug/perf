# perf 使用
## riscv集成perf

## 使用perf产生火焰图
Target:
`./perf record  -p 283 -a -g -o a.data  -- sleep 10`
`./perf script -i a.data  &> a.unfold`
PC:
```
sudo apt-get install c++filt
git clone https://github.com/brendangregg/FlameGraph.git
cd FlameGraph
./stackcollapse-perf.pl a.unfold |c++filt &> a.folded
./flamegraph.pl a.folded  > a.svg
```
* 经过上述命令后使用浏览器打开a.svg便可以浏览火焰图
 火焰图 如下
 
