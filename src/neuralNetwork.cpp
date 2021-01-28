#include "neuralNetwork.hpp"

NeuralNetwork::~NeuralNetwork(){
    if (theta1)
        delete theta1;
    if (a1)
        delete a1;
    if (a2)
        delete a2;
    if (Y)
        delete Y;
    theta1 = a1 = a2 = Y = nullptr;
}

void NeuralNetwork::setNumberOfTrainingExamples(size_t totalTicks){
    training_set_size = totalTicks % (TICK_CANDLE * 3);
}
