#include "ConfusionSet.h"
#include "WcharLoader.h"
#include "WcharWriter.h"
#include <cassert>

namespace NLP
{
	ConfusionSet::ConfusionSet() : _charConfusionCounter()
	{
		
	}

	void ConfusionSet::insert(wchar_t key, wchar_t value, double pro)
	{
		if (!_charConfusionCounter.count(key))
		{
			_charConfusionCounter[key].insert({ key, 0 });
			_charConfusionSet[key].insert({ key, 1.0 });
		}
		_charConfusionCounter[key].insert({ value, 0 });
		_charConfusionSet[key][value] = std::max(_charConfusionSet[key][value], pro);
	}

	void ConfusionSet::insertConfusionPair(wchar_t correctChar, wchar_t confusionChar)
	{
		_charConfusionCounter[correctChar][confusionChar]++;
		_charConfusionCounter[correctChar][correctChar]++; // total count
	}

	void ConfusionSet::calProbability()
	{
		for (auto& pr1 : _charConfusionCounter)
		{
			auto& currentConfusionSet = pr1.second;
			wchar_t wch1 = pr1.first;
			unsigned totalCnt = currentConfusionSet[wch1];
			unsigned setSize = currentConfusionSet.size();
			for (auto& pr2 : currentConfusionSet)
			{
				wchar_t wch2 = pr2.first;
				if (wch2 == wch1)
				{
					continue;
				}
				// Êý¾ÝÆ½»¬
				_charConfusionSet[wch1][wch2] *= double(pr2.second + 1) / (totalCnt + setSize);
			}
		}
	}

	bool ConfusionSet::characterExists(wchar_t ch) const
	{
		return _charConfusionCounter.count(ch);
	}
	const std::unordered_map<wchar_t, double>& ConfusionSet::getCharConfustionSet(wchar_t ch) const
	{
		return _charConfusionSet.at(ch);
	}

}