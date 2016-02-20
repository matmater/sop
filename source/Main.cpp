#include "Common.h"

#include "VQRecognizer.h"
#include "GMMRecognizer.h"

#include "TestEngine.h"

int main(int argc, char** argv)
{
    // !!!!!!!!
    // INDEXFIX macro in Common.h !
    // !!!!!!!!

    /* tests.txt
    %ubm 295 30 1 5

    %test0 rec
          samples      vq       225 50 1 5   225 10 6 11   2         -o 128
          samples      gmm      225 50 1 5   225 10 6 11   2         -o 128

    %test1 ver
          samples      vq       225 100 1 5   240 5 10 3   5  5 1  275 20   -w -o 128 -ubm -zt
        n_samples      vq       225 100 1 5   240 5 10 3   5  5 1  275 20   -w -o 128 -ubm -zt
          samples      gmm      225 100 1 5   240 5 10 3   5  5 1  275 20   -o 128 -ubm -zt
    */
    TestEngine engine;

    engine.Run("tests.txt");

    system("pause");

    //// Use scopes to seal test cases.
    //{
    //    auto trainData = std::make_shared<SpeechData>();
    //    auto ubmData = std::make_shared<SpeechData>();
    //    
    //    // Load speaker data.
    //    LoadTextSamples("samples", trainData, 225     , 100, 1, 5, true);
    //    // Load ubm data from a different set of speakers.
    //    LoadTextSamples("samples", ubmData,   225 + 70, 30,  1, 5, true);
    //    
    //    // Copy, paste & modify to do the same procedure for the GMM.
    //    {
    //        auto vq = std::make_shared<VQRecognizer>();
    //    
    //        // Order
    //        vq->SetOrder(256);
    //        // VQ weighting
    //        vq->SetWeightingEnabled(true);
    //        // Normalization
    //        vq->SetScoreNormalizationType(ScoreNormalizationType::ZERO_TEST);
    //        // Background model
    //        vq->SetBackgroundModelEnabled(true);
    //        vq->SetBackgroundModelTrainingEnabled(true);
    //        // MAP
    //        vq->SetAdaptationIterations(2); // 1-3
    //        vq->SetRelevanceFactor(12.0f);  // And maybe 16.0f for GMM.
    //        // Data
    //        vq->SetSpeakerData(trainData);
    //        vq->SetBackgroundModelData(ubmData);

    //        // No need to call Train() here.

    //        // "samples" is the folder that contains the coefficients.
    //        // Alternatively, the folder can be named "mfcc", for instance.

    //        // Creates a "samples_rec_[identifiers].txt" file.
    //        vq->SetBackgroundModelEnabled(false);
    //        vq->SetScoreNormalizationType(ScoreNormalizationType::NONE);
    //        engine.Recognize(
    //            "samples", vq,
    //            240,      // sf
    //            5,        // gf
    //            10,       // sl
    //            3,        // gl
    //            4         // cycles
    //        );
    //    
    //        // Creates a "samples_ver_[identifiers].txt" file.
    //        vq->SetBackgroundModelEnabled(true);
    //        vq->SetScoreNormalizationType(ScoreNormalizationType::ZERO_TEST);
    //        engine.Verify(
    //            "samples", vq,
    //            240,      // sf
    //            5,        // gf
    //            10,       // sl
    //            3,        // gl
    //            5,        // cycles
    //            5,        // incorrectClaimed
    //            1,        // correctClaimed
    //            225 + 50, // si (start-impostor)
    //            20        // gi (get-impostor)
    //        );
    //    }
    //}
}