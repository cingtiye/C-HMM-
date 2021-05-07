# C++ 实现一阶HMM中文分词

### 1、词库

采用MSR词库，包含训练集，测试集。MSR词库信息如下

| 语料库 | 字符数 | 词语种数 | 总词频 | 平均词长 | 字符数 | 词语种数 | 总词频 | OOV   |
| ------ | ------ | -------- | ------ | -------- | ------ | -------- | ------ | ----- |
| MSR    | 405万  | 9万      | 237万  | 1.7      | 18万   | 1万      | 11万   | 2.65% |
### 2、代码结构

|--- data

|	| --- MSR

|	|	| --- testing       测试集

|	|	| --- training      训练集

|	|	| --- gold            测试集真实结果

|--- include

|	| --- HiddenMarkovModel.hpp	                       马尔科夫模型

|	| --- FirstOrderHiddenMarkovModel.hpp		一阶马尔科夫模型 （继承马尔科夫模型）

|	| --- TransChinese.hpp                                        中文字符转换

|--- src

|	| --- evaluate.py		评估一阶HMM预测结果与测试集真实结果

| --- load_test.cpp		   主函数

### 3、代码执行

#### 3.1 执行 load_test.cpp 训练HMM和得到在MSR上的分词结果

```C++
int main(){

    string train_file = "./data/MSR/training/msr_training.utf8";
    string test_file = "./data/MSR/testing/msr_test.utf8";
    string output_file = "./data/MSR/testing/hmm_output.txt";
    FirstOrderHiddenMarkovModel* trained_FoHMM = new FirstOrderHiddenMarkovModel();
    cout << "Traing HMM ..."<<endl;
    Cut(trained_FoHMM, train_file);
    cout << "Segment ..."<<endl;
    Segment(trained_FoHMM, test_file, output_file);
    cout << "Result has been saved to "<<output_file <<endl;
    return 0;
}
```

#### 3.2 执行 /src/evaluate.py 得到评估结果

### 4、评估结果

| 模型       | P     | R     | F1    | R_{oov} | R_{IV} |
| ---------- | ----- | ----- | ----- | ------- | ------ |
| 一阶 HMM   | 73.42 | 77.43 | 75.37 | 34.54   | 78.60  |
| 最长匹配 * | 91.80 | 95.69 | 93.71 | 2.58    | 98.22  |
| 二元语法 * | 92.38 | 96.70 | 94.49 | 2.58    | 99.26  |

**最长匹配和二元语法参考 pyhanlp库和何晗新书《自然语言处理入门》，最长匹配自己通过python和java实现了一下，和书上有出入，这里是自己跑出的结果，二元语法和书上的结果一致。**

### 5、结论 

一阶HMM F1值相比最长匹配和n-gram较低，但是R_{oov}相比于二者有很大提升。**因此，朴素HMM作为入门模型，不适用于中文分词，但是是非常基础的模型。**