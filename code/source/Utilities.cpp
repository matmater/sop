#include "Utilities.h"

bool Utilities::IsSameSpeaker(const std::string& first, const std::string& second)
{
	return (first.compare(0, 3, second) == 0);
}