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

	//��ȡ�ִ����� ����ʵ�
	languageModel.insertDictionary("TrainDataCht.txt");
	languageModel.insertWordsToDictionary("idiomDictionary.txt");
	printTimeInfo(beginTime, "load the dictionary");

	//��ȡԭʼ������
	languageModel.insertCSShape("SimilarShape.txt");
	languageModel.insertCSPronunciation("SimilarPronunciation.txt");
	printTimeInfo(beginTime, "load the confusion set");

	//Ԥ����1�����ı����ҳ���ѡ������
	/*languageModel.outputConfusionPair("ChatData.txt", "ChatDataConfusionSet.txt");
	printTimeInfo(beginTime, "calculate the confusion set");*/

	//Ԥ����2��ɸѡ�����Բ�����cosֵ
	/*languageModel.calPotentialConfusionPair("ChatDataConfusionSet.txt", "ConfusionInfo.txt");
	printTimeInfo(beginTime, "calculte the confusion pairs");*/

	//Ԥ����3�����ݻ����Գ��ִ�����cosֵ������ÿ���ֳ���ĸ���
	languageModel.insertConfusionPairSet("ConfusionInfo.txt");
	printTimeInfo(beginTime, "insert confusion pairs");

	// ���������Լ����в���
	languageModel.cnSpellingChecker("FinalTest_SubTask1.txt", "FinalTest_SubTask1_Result.txt");
	printTimeInfo(beginTime, "correct the test file1");
	languageModel.cnSpellingChecker("FinalTest.txt", "FinalTest_Result.txt");
	printTimeInfo(beginTime, "correct the test file2");

	// �������
	NLP::cnSpellingDetectEvaluator("FinalTest_SubTask1.txt", "FinalTest_SubTask1_Result.txt", "FinalTest_SubTask1_Truth.txt");
	NLP::cnSpellingCorrectEvaluator("FinalTest.txt", "FinalTest_Result.txt", "FinalTest_Truth.txt");

	system("pause");
	return 0;
}

