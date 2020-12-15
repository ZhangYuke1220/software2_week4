# ナップサック問題

ナップサック問題を貪欲法で解いてみました．

## 貪欲法

貪欲法とは、選べるものの中から、コストパフォマンスの最も良いものを毎回選ぶことによって局所最優解を求め、近似的に最適解を求めるアルゴリズムです．実装が簡単で、効率も良いので、最も基本的な近似アルゴリズムの一つとして知られております．今回はナップサック問題を貪欲法で解いてみました．

## 実装のポイント

- 品物の価値重量比（コスパのようなもの）を求める
- 品物を価値重量比の高い順（もしくは低い順）に並べる
- 価値重量比の高いものから順にナップサックに詰める
- 入れなくなるまで詰める

一見すごく単純な発想ですが、良い近似解が求められます．

## 実装コード

```c
typedef struct item
{
    int arrange;
    double ratio;
    double value;
    double weight;
} Item;
```
まずは計算の便利上、```item```のメンバーを少し変えました．```ratio```は```value / weight```で求められ、初期化の際に計算されます．品物を```ratio```順に並べるともとの順番がわからなくなりますので、区別のため```label```で品物に名前をつけます．

```c
void quick_sort(Itemset *itemset, int low, int high)
```
これはどのソートアルゴリズムを使ってもよいですが、安定性を求めているか、速さを求めているかによって選べます．最悪バブルソートを使っても計算量が高々```O(n * n)```なので今の計算機のスピードだとそれほどの影響は出ないと思います（ほんとうか？）．今週の講義でせっかく再帰をやりましたのでクィックソートを使いました．

```c
for (int i = list->number-1; i>=0; --i)
{
    if (list->item[i].weight < capacity)
    {
        choosed[i] = list->item[i].label;
        capacity -= list->item[i].weight;
        sum_w += list->item[i].weight;
        sum_v += list->item[i].value;
    }
    else
        continue;
}
```
こちら本番になりますが、特に難しいことをやっていません．```quick_sort```で品物を```ratio```の小さい順に並べましたので、リストの最後尾から品物を選んでいきます．残っている重量が最後尾の品物の重さより大きければ入れると判断し、残っている重量が入れた品物の分だけ減り、```choosed```で選ばれた品物のラベルを記録します．ナップサック内の重さと価値が入れた品物分だけ大きくなります．

そして選ばれた品物を```flag```に反映されましたが、特に理由がありません（ただ答えの構造体をそのまま使いたかっただけなので…）．

## 実行結果

### 貪欲法
```c
max capacity: W = 200, # of items: 35

best solution:
value: 341.1
11111111110110011010001110010110101

real    0m0.007s
user    0m0.000s
sys     0m0.000s
```

### 全探索（枝刈り）
```c
max capacity: W = 200, # of items: 35

best solution:
value: 341.1
11111111110110011010001110010110101

real    9m49.197s
user    9m48.672s
sys     0m0.078s
```
ソートアルゴリズムにもよりますが、最悪でも計算量が```O(n * n)```くらいで抑えられます．nが大きいとき、全探索の```O(2 ^ n)```よりだいぶ良くなります．