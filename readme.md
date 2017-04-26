# 一个半成型但是离能用还差得远的求解器
## 最后几天原来的代码怎么改都没有进展，深感绝望，决定写一个求解器试一试。

网上找了好几份单纯形代码，发现每一份都不太一样，而且看不懂，最后无奈只好全部推倒，自己重写。还好有大佬指点一二，不然有很多地方搞不懂.

# 实现了：
1. 对偶单纯形（稀疏矩阵实现）
2. [Gomory's 割平面](https://en.wikipedia.org/wiki/Cutting-plane_method)
3. 分支定界（伪）

# 坑点：
### 数值精度
由于对于矩阵不断的进行行操作，误差会逐渐累积，产生错误。解决方法有两个：
1. 使用更高精度的浮点数(long double)。好处是不需要修改代码，坏处是拖慢正常运行速度(不过还是比2快多了)，治标不治本，迭代足够多的话还是出错。
2. 重新计算所有矩阵(RSM_Model::eliminateError)，此方法坑点在于需要计算非稀疏矩阵的乘法，速度太慢，优点在于，能“完全”重置误差。

### Gomory's 割平面(RSM_Model::cut)
为啥课本里的例子都能算出来，一到最简单的图里割半天也割不出整数解啊。。。

### 分支定界(RSM_Model::BnB)
这不是真的分支定界，没有内存储存状态了，我也不清楚能否从单纯形表里删除约束，我也很绝望啊，GG。



##这是我的最后一次比赛了，就这样了，有问题联系我就好545976176@qq.com。
>非常精彩的比赛。
