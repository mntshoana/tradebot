#ifndef neuralNetwork_hpp
#define neuralNetwork_hpp

#include "Eigen/Eigen"

#include "lunoTypes.hpp"
#define TICK_CANDLE 100

// not really a neural network, no hidden layer yet
class NeuralNetwork {
    size_t feature_size;
    size_t training_set_size;
    bool containsY;
public:
    Eigen::MatrixXd* theta1;
    Eigen::MatrixXd* a1; // activation level 1 (or X)
    Eigen::MatrixXd* a2;  // activation level 2 --- 3 hypothesis units (buy, hold, sell)
    
    Eigen::MatrixXd* Y;
public:
    NeuralNetwork() : feature_size(0), training_set_size(0), theta1(nullptr), a1(nullptr), a2(nullptr), Y(nullptr) { }
    ~NeuralNetwork();
    
    void setNumberOfTrainingExamples(size_t totalTicks);
    void loadData(std::vector<Luno::Trade>* ticks);
    Eigen::MatrixXd* hypothesis ();
    void cost();
    void minimize();
    void train();
    void predict();
};

#endif /* neuralNetwork_hpp */
