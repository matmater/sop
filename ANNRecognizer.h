#ifndef _ANNRECOGNIZER_H_
#define _ANNRECOGNIZER_H_

#include "Common.h"
#include "Recognizer.h"
#include "ANN.h"
#include "SpeechData.h"
#include "ANNTrainData.h"

/*! \brief A speaker recognizer based on Artificial Neural Network.
 */
class ANNRecognizer : public Recognizer
{
public:
	/*! \brief Default constructor
	 */
	ANNRecognizer();
	
	/*! \brief Virtual destructor.
	 */
	virtual ~ANNRecognizer();

	/*! \copydoc Recognizer::Clear()
	 */
	virtual void Clear();
	
	/*! \copydoc Recognizer::Train()
	 */
	void Train(const SpeechData& data);
	
	/*! \copydoc Recognizer::SaveTrainedData()
	 */
	void SaveTrainedData(const std::string& path);
	
	/*! \copydoc Recognizer::LoadTrainedData()
	 */
	void LoadTrainedData(const std::string& path);
	
	/*! \copydoc Recognizer::Test()
	 */
	void Test(const SpeechData& data, std::map<std::string, RecognitionResult>& results);

private:
	std::map<unsigned int, std::string> mNeuronMapping; /*!< Speaker Id, Label */

	ANNTrainData mTrainData;

	std::vector<Real> mResult;

	ANN mNetwork;
};

#endif