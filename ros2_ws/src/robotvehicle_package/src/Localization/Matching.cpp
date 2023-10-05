//
// Created by robot1 on 10/5/23.
//

#include "Matching.h"


Matching::Matching(int capacity)  {
    this->matches = new Matches(capacity);
}

void Matching::update(const PredictionDifferentialDrive *prediction,
                      const MeasurementPrediction *measurementPrediction,
                      const Observations *observations) {

}
