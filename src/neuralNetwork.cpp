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

void NeuralNetwork::loadData(std::vector<Luno::Trade>* ticks){
    containsY = true;
    if (training_set_size == 0)
        // Error, first set the number of trainning examples
        return;
    if (a1)
        delete a1;
    if (Y)
        delete Y;
    a1 = new Eigen::MatrixXd(feature_size +1, training_set_size);
    Y = new Eigen::MatrixXd(training_set_size, 3); // (buy, hold, sell)
    
    for (int i = 0; i < training_set_size; i++){
        // column-wise loading of features
        (*a1)(0, i) = 1; // bias unit, defaults to 1
        
        // FORM CANDLE 1
        size_t pos = i * TICK_CANDLE * 3; // for ticks vector
        (*a1)(1, i) = (*ticks)[pos].price; // unit 1, opening price
        
        double high = INT_MAX, low = INT_MAX;
        for (int index = 0; index < TICK_CANDLE; index++){
            if ( high < (*ticks)[pos + index].price )
                high =  (*ticks)[pos + index].price;
            if ( low > (*ticks)[pos + index].price )
                low = (*ticks)[pos + index].price;
        }
        (*a1)(2, i) = high; // unit 2, high price
        (*a1)(3, i) = low; // unit 3, low price
        (*a1)(4, i) = (*ticks)[pos + TICK_CANDLE - 1].price; // unit 4, closeing price
        
        // FORM CANDLE 2
        pos += TICK_CANDLE;
        (*a1)(5, i) = (*ticks)[pos].price; // unit 1, opening price
        
        high = INT_MAX, low = INT_MAX;
        for (int index = 0; index < TICK_CANDLE; index++){
            if ( high < (*ticks)[pos + index].price )
                high =  (*ticks)[pos + index].price;
            if ( low > (*ticks)[pos + index].price )
                low = (*ticks)[pos + index].price;
        }
        (*a1)(6, i) = high; // unit 2, high price
        (*a1)(7, i) = low; // unit 3, low price
        (*a1)(8, i) = (*ticks)[pos + TICK_CANDLE - 1].price; // unit 4, closeing price
        
        // Label
        // FROM CANDLE 3 - no need to form
        // row-wise loading of labels
        pos += TICK_CANDLE;
        double close = (*ticks)[pos + TICK_CANDLE - 1].price;
        if ( close > (*a1)(8, i) * 1.002 ){
            (*Y)(i, 0) = 1; // Buy
            (*Y)(i, 1) = 0;
            (*Y)(i, 2) = 0;
        }
        else if ( close >= (*a1)(8, i) ){
            (*Y)(i, 0) = 0;
            (*Y)(i, 1) = 1; // Hold
            (*Y)(i, 2) = 0;
        }
        else {
            (*Y)(i, 0) = 0;
            (*Y)(i, 1) = 0;
            (*Y)(i, 2) = 1; // Sell
        }
    }
}

// only one step to forward propagation for architecture with no hidden layer, as it is only multi class logistic regression
Eigen::MatrixXd* NeuralNetwork::hypothesis (){
    if (!theta1){
        // no trained data
        return nullptr;
    }
    // h(z2) = 1 / [1 + e ^ -(theta1 * a1) ]
    (*a2) = -( theta1->transpose() * (*a1) ).transpose();
    (*a2) = 1 / (1 + a2->array().exp() );
    return a2;
}
