#ifndef CONFUSION_SET_H
#define CONFUSION_SET_H

#include <unordered_map>
#include <unordered_set>
#include "WcharWriter.h"
#include "Dictionary.h"

namespace NLP
{
	class ConfusionSet
	{
	public:
		ConfusionSet();

		void insert(wchar_t key, wchar_t value, double initialP);
		void insertConfusionPair(wchar_t correctChar, wchar_t confusionChar);
		void calProbability();
		bool characterExists(wchar_t ch) const;
		const std::unordered_map<wchar_t, double>& getCharConfustionSet(wchar_t ch) const;
	private:
		std::unordered_map<wchar_t, std::unordered_map<wchar_t, unsigned>> _charConfusionCounter;
		std::unordered_map<wchar_t, std::unordered_map<wchar_t, double>> _charConfusionSet;
	};

}

namespace std {
	template <>
	class hash< std::pair<wchar_t, double >> {
	public:
		size_t operator()(std::pair<wchar_t, double > pr) const
		{
			return hash<wchar_t>()(pr.first) * hash<double>()(pr.second);
		}
	};

	template <>
	class hash< std::pair<wchar_t, unsigned >> {
	public:
		size_t operator()(std::pair<wchar_t, double > pr) const
		{
			return hash<wchar_t>()(pr.first) * hash<unsigned>()(pr.second);
		}
	};
};

#endif