#ifndef neuralNetwork_hpp
#define neuralNetwork_hpp

#include <QTextEdit>

#include "Eigen/Eigen"

#include "lunoTypes.hpp"
#define TICK_CANDLE 100

// not really a neural network, no hidden layer yet
class NeuralNetwork {
    size_t feature_size;
    size_t training_set_size;
    bool containsY;
    
    QTextEdit* outputStream;
public:
    Eigen::MatrixXd* theta1;
    Eigen::MatrixXd* a1; // activation level 1 (or X)
    Eigen::MatrixXd* a2;  // activation level 2 --- 3 hypothesis units (buy, hold, sell)
    
    Eigen::MatrixXd* Y;
public:
    NeuralNetwork() : feature_size(8 + 1), training_set_size(0),
                        outputStream(nullptr), theta1(nullptr), a1(nullptr), a2(nullptr), Y(nullptr) { }
    ~NeuralNetwork();
    
    void setNumberOfTrainingExamples(unsigned long long totalTicks);
    void setOutputStream(QTextEdit* stream);
    void loadData(std::vector<Luno::Trade>* ticks);
    Eigen::MatrixXd* hypothesis ();
    void cost();
    void minimize();
    void train();
    void predict();
};

#endif /* neuralNetwork_hpp */
