# Simple-Chinese-Spelling-Checker

A program which correct the spelling error in Chinese text. The performance of program is poor due to insufficient training data.

The algorithm of program is based on \[3\] and the most training and test data come from \[1\]\[2\]. The confusing set is trained with https://github.com/zake7749/Gossiping-Chinese-Corpus. The language model is trained with People's Daily Corpus and https://github.com/pwxcoo/chinese-xinhua.

If you understand Chinese, you can visit my blog for more information: https://zhouyifan.net/2020/02/09/Chinese-Spelling-Checker/.

# Reference

[1].  Shih-Hung Wu, Chao-Lin Liu, and Lung-Hao Lee (2013). Chinese Spelling Check Evaluation at SIGHAN Bake-off 2013. Proceedings of the 7th SIGHAN Workshop on Chinese Language Processing (SIGHAN'13), Nagoya, Japan, 14 October, 2013, pp. 35-42.

[2].  Chao-Lin Liu, Min-Hua Lai, Kan-Wen Tien, Yi-Hsuan Chuang, Shih-Hung Wu, and Chia-Ying Lee. Visually and Phonologically similar characters in incorrect Chinese Words: analyses, Identification, and Applications. ACM Transactions on Asian Language Information Processing, 10(2), 10:1-39.

[3].  Hsieh Y M, Bai M H, Huang S L, et al. Correcting Chinese spelling errors with word lattice decoding[J]. ACM Transactions on Asian and Low-Resource Language Information Processing (TALLIP), 2015, 14(4): 18.