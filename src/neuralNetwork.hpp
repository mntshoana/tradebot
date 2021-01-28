#ifndef neuralNetwork_hpp
#define neuralNetwork_hpp

#include "Eigen/Eigen"
#define TICK_CANDLE 100

class NeuralNetwork {
    size_t feature_size;
    size_t training_set_size;
    bool containsY;
public:
    Eigen::MatrixXd* theta1;
    Eigen::MatrixXd* a1; // activation level 1 (or X)
    Eigen::MatrixXd* a2; // activation level 2 --- 3 units
    
    Eigen::MatrixXd* Y; // (buy, hold, sell)
public:
    NeuralNetwork() : feature_size(0), theta1(nullptr), a1(nullptr), a2(nullptr), Y(nullptr) { }
    ~NeuralNetwork();
    
    void setNumberOfTrainingExamples(size_t totalTicks);
};

#endif /* neuralNetwork_hpp */
