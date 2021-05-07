#-*- coding: utf-8 -*-
import re
import time
from collections import Counter
import os

def prf(gold: str, pred: str, dic) -> tuple:
        """
        计算P、R、F1
        :param gold: 标准答案文件，比如“商品 和 服务”
        :param pred: 分词结果文件，比如“商品 和服 务”
        :param dic: 词典
        :return: (P, R, F1, OOV_R, IV_R)
        """
        def to_region(segmentation: str) -> list:
            """
            将分词结果转换为区间
            :param segmentation: 商品 和 服务
            :return: [(0, 2), (2, 3), (3, 5)]
            """
            region = []
            start = 0
            for word in re.compile("\\s+").split(segmentation.strip()):
                end = start + len(word)
                region.append((start, end))
                start = end
            return region

        A_size, B_size, A_cap_B_size, OOV, IV, OOV_R, IV_R = 0, 0, 0, 0, 0, 0, 0
        with open(gold, encoding='utf-8') as gd, open(pred, encoding='utf-8') as pd:
            for g, p in zip(gd, pd):
                A, B = set(to_region(g)), set(to_region(p))
                A_size += len(A)
                B_size += len(B)
                A_cap_B_size += len(A & B)
                text = re.sub("\\s+", "", g)
                for (start, end) in A:
                    word = text[start: end]
                    if word in dic:
                        IV += 1
                    else:
                        OOV += 1

                for (start, end) in A & B:
                    word = text[start: end]
                    if word in dic:
                        IV_R += 1
                    else:
                        OOV_R += 1
        p, r = A_cap_B_size / B_size * 100, A_cap_B_size / A_size * 100
        return p, r, 2 * p * r / (p + r), OOV_R / OOV * 100, IV_R / IV * 100

if __name__ == '__main__':
    data = 'MSR'
    corpus_path = os.path.join('./', 'data/MSR')
    train_file = os.path.join('training', 'msr_training.utf8')
    test_file = os.path.join('testing', 'msr_test.utf8')
    model_name = 'HMM_model'

    test_gold_file = os.path.join('gold', 'msr_test_gold.utf8')
    test_gold_path = os.path.join(corpus_path, test_gold_file)

    msr_output = os.path.join(corpus_path, 'testing', 'hmm_output.txt')


    print('开始测试 HMM 分词:  ' + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
    msr_dict_path = os.path.join(corpus_path, 'gold', 'msr_training_words.utf8')
    msr_dict = set()
    with open(msr_dict_path, 'r', encoding='utf-8') as f:
        for word in f:
            msr_dict.add(word.strip())
    f.close()
    
    msr_eval = prf(test_gold_path, msr_output, msr_dict)
    print("P:%.2f R:%.2f F1:%.2f OOV-R:%.2f IV-R:%.2f" % msr_eval)
    print('测试结束:    ' + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
    print('\n')


