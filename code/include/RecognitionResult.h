#ifndef _RECOGNITIONRESULT_H_
#define _RECOGNITIONRESULT_H_

#include "Common.h"

/*! \brief The result of a speaker recognition task.
 */
class RecognitionResult
{
public:
	enum class Type
	{
		RECOGNIZED = 0,
		UNRECOGNIZED,
		UNKNOWN
	};

public:
	/*! \brief Constructor with parameters.
	 *
	 *  \param type The recognition type.
	 *  \param speaker The string identifier of the speaker (if recognized).
	 */
	/*
	RecognitionResult(Type type = Type::UNKNOWN, const std::string& speaker = "", bool placeholder = false);
	*/
	
	/*! \brief Constructor with parameters.
	 */
	RecognitionResult(const bool known = false, const std::string& speaker = "");

	/*! \brief Virtual destructor.
	 */
	~RecognitionResult();
	
	/*! \brief Returns the type of the recognition.
	 */
	Type GetType() const;
	
	/*! \brief Returns the string identifier of the speaker.
	 */
	const std::string& GetSpeaker() const;

	/*! A flag that tells if the speaker existed in database.
	 */
	bool GetKnown() const;

private:
	bool mKnown;

	Type mType;

	std::string mSpeaker;
};

#endif