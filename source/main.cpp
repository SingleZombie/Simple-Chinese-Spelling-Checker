#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include "LanguageModel.h"
#include "NLPAlgorithm.h"
#include "WcharLoader.h"
#include "WcharWriter.h"

NLP::LanguageModel languageModel;

void printTimeInfo(unsigned& beginTime, const char* str)
{
	printf("It takes %ums to %s\n", clock() - beginTime, str);
	beginTime = clock();
}



int main()
{
	unsigned beginTime = clock();

	//读取分词语料 成语词典
	languageModel.insertDictionary("TrainDataCht.txt");
	languageModel.insertWordsToDictionary("idiomDictionary.txt");
	printTimeInfo(beginTime, "load the dictionary");

	//读取原始混淆集
	languageModel.insertCSShape("SimilarShape.txt");
	languageModel.insertCSPronunciation("SimilarPronunciation.txt");
	printTimeInfo(beginTime, "load the confusion set");

	//预处理1：从文本中找出候选混淆对
	/*languageModel.outputConfusionPair("ChatData.txt", "ChatDataConfusionSet.txt");
	printTimeInfo(beginTime, "calculate the confusion set");*/

	//预处理2：筛选混淆对并计算cos值
	/*languageModel.calPotentialConfusionPair("ChatDataConfusionSet.txt", "ConfusionInfo.txt");
	printTimeInfo(beginTime, "calculte the confusion pairs");*/

	//预处理3：根据混淆对出现次数、cos值，计算每个字出错的概率
	languageModel.insertConfusionPairSet("ConfusionInfo.txt");
	printTimeInfo(beginTime, "insert confusion pairs");

	// 用两个测试集进行测试
	languageModel.cnSpellingChecker("FinalTest_SubTask1.txt", "FinalTest_SubTask1_Result.txt");
	printTimeInfo(beginTime, "correct the test file1");
	languageModel.cnSpellingChecker("FinalTest.txt", "FinalTest_Result.txt");
	printTimeInfo(beginTime, "correct the test file2");

	// 评估结果
	NLP::cnSpellingDetectEvaluator("FinalTest_SubTask1.txt", "FinalTest_SubTask1_Result.txt", "FinalTest_SubTask1_Truth.txt");
	NLP::cnSpellingCorrectEvaluator("FinalTest.txt", "FinalTest_Result.txt", "FinalTest_Truth.txt");

	system("pause");
	return 0;
}

