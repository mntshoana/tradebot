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

void NeuralNetwork::setNumberOfTrainingExamples(unsigned long long totalTicks){
    training_set_size = totalTicks / (TICK_CANDLE * 3);
}

void NeuralNetwork::setOutputStream(QTextEdit* stream){
    outputStream = stream;
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
    a1 = new Eigen::MatrixXd(training_set_size, feature_size);
    Y = new Eigen::MatrixXd(3, training_set_size); // (buy, hold, sell)
    
    for (unsigned long long i = 0; i < training_set_size; i++){
        // x1, x2, x3, x4, x5 .... -> test example1
        // x1, x2, x3, x4, s5 .... -> test example2
        // ...
        
        (*a1)(i, 0) = 1; // bias unit, defaults to 1
        double max = 0; // to regularize
        // FORM CANDLE 1
        unsigned long long pos = i * TICK_CANDLE * 3; // for ticks vector
        double high = INT_MIN, low = INT_MAX;
        for (int index = 0; index < TICK_CANDLE; index++){
            if ( high < (*ticks)[pos + index].price )
                high =  (*ticks)[pos + index].price;
            if ( low > (*ticks)[pos + index].price )
                low = (*ticks)[pos + index].price;
        }
        (*a1)(i, 1) = (*ticks)[pos].price; // unit 1, opening price
        (*a1)(i, 2) = high; // unit 2, high price
        (*a1)(i, 3) = low; // unit 3, low price
        (*a1)(i, 4) = (*ticks)[pos + TICK_CANDLE - 1].price; // unit 4, closeing price
        max = high;
        // FORM CANDLE 2
        pos += TICK_CANDLE;
        high = INT_MIN, low = INT_MAX;
        for (int index = 0; index < TICK_CANDLE; index++){
            if ( high < (*ticks)[pos + index].price )
                high =  (*ticks)[pos + index].price;
            if ( low > (*ticks)[pos + index].price )
                low = (*ticks)[pos + index].price;
        }
        (*a1)(i, 5) = (*ticks)[pos].price; // unit 1, opening price
        (*a1)(i, 6) = high; // unit 2, high price
        (*a1)(i, 7) = low; // unit 3, low price
        (*a1)(i, 8) = (*ticks)[pos + TICK_CANDLE - 1].price; // unit 4, closeing price
        if (max < high)
            max = high;
        // Label
        // FROM CANDLE 3 - heighst value
        // row-wise loading of labels
        pos += TICK_CANDLE;
        high = INT_MIN;
        for (int index = 0; index < TICK_CANDLE; index++){
            if ( high < (*ticks)[pos + index].price )
                high =  (*ticks)[pos + index].price;
        }
        
        if ( high > (*a1)(i, 8) * 1.002 ){
            (*Y)(0, i) = 1; // Buy
            (*Y)(1, i) = 0;
            (*Y)(2, i) = 0;
        }
        else if ( high >= (*a1)(i, 8) ){
            (*Y)(0, i) = 0;
            (*Y)(1, i) = 1; // Hold
            (*Y)(2, i) = 0;
        }
        else {
            (*Y)(0, i) = 0;
            (*Y)(1, i) = 0;
            (*Y)(2, i) = 1; // Sell
        }
        
        // Regularize
        (*a1)(i, 1) = (*a1)(i, 1) / max;
        (*a1)(i, 2) = (*a1)(i, 2) / max;
        (*a1)(i, 3) = (*a1)(i, 3) / max;
        (*a1)(i, 4) = (*a1)(i, 4) / max;
        (*a1)(i, 5) = (*a1)(i, 1) / max;
        (*a1)(i, 6) = (*a1)(i, 2) / max;
        (*a1)(i, 7) = (*a1)(i, 3) / max;
        (*a1)(i, 8) = (*a1)(i, 4) / max;
    }
    /*
    if (outputStream) {
        std::stringstream out;
        out << "Training set:\n" << std::endl;
        for (unsigned long long i = 0; i < training_set_size; i++){
            for (size_t j = 0; j < feature_size; j++){
                out << (*a1)(i,j) << " ";
            }
            out << std::endl;
        }
        outputStream->append(out.str().c_str());
    }
    */
}

// only one step to forward propagation for architecture with no hidden layer, as it is only multi class logistic regression
Eigen::MatrixXd* NeuralNetwork::hypothesis (){
    if (!theta1){
        // no trained data
        return nullptr;
    }
    // h(z2) = 1 / [1 + e ^ -(theta1 * a1) ]
    (*a2) = -( *theta1 * a1->transpose() );
    (*a2) = 1 / (1 + a2->array().exp() );
    
    return a2;
}

void NeuralNetwork::cost() {
    // More or less similar to:
    // cost = 1/m [∑i=1..n ( -log( h(xi) )     , if yi = 1
    //                  -log( 1 - h(xi) ) , if yi = 0  ]
    //      =-1/m [∑i=1..n (yi*log( h(xi) ) + (1-yi)*log( 1 - h(xi)) ]
    // but for multiclass, will do
    #define ones Eigen::MatrixXd::Ones(1, training_set_size)
    Eigen::MatrixXd cost(3,1);
    for (int i = 0; i < 3; i++) {
        cost.row(i) = (  Y->row(i) * (Eigen::MatrixXd) a2->row(i).transpose().array().log()  );
        cost.row(i) += (ones - Y->row(i))
                * (Eigen::MatrixXd) (ones - a2->row(i) ).transpose().array().log() ;
    }
    cost /= training_set_size;
    cost = -cost;
    
    // OUTPUT COST FUNCTION
    if (outputStream) {
        std::stringstream out;
        out << "Cost function: "
        << cost(0,0) << " "
        << cost(1,0) << " "
        << cost(2,0) << " ";
        outputStream->append(out.str().c_str());
    }
}

void NeuralNetwork::minimize(){
    // gradient desc:
    // thetaj = thetaj - alpha 1/n ∑i=1..n  ( h(xi) - yi) * Xj
    double alpha = 0.3;
    for (int i = 0; i < 2000; i++){
        hypothesis();
        // ∑ h - y * x[i] / total n
        Eigen::MatrixXd derivative(3, feature_size);
        derivative = (*a2 - *Y) * (*a1);
        derivative /= training_set_size;
        
        /*
        // OUTPUT derivative
        if (outputStream && i % 10 == 0) {
            std::stringstream out;
            out << "derivative vs theta\n";
            out << "theta1\n";
            for (int ind = 0; ind < 3; ind++){
                for (int j = 0; j < feature_size; j++)
                    out << (*theta1)(ind, j) << " ";
                out << std::endl;
            }
            out << "derivative\n";
            for (int ind = 0; ind < 3; ind++){
                for (int j = 0; j < feature_size; j++)
                    out << derivative(ind, j) << " ";
                out << std::endl;
            }
            outputStream->append(out.str().c_str());
        }
         */
        *theta1 = *theta1 - (alpha * derivative);
        // print cost function to see if converged
        if (containsY && i % 100 == 0)
            cost(); // this should print the cost
    }
}

void NeuralNetwork::train(){
    if (!theta1)
        theta1 = new Eigen::MatrixXd(3, feature_size);
    for (int i = 0; i < feature_size ; i++)
        (*theta1)(0, i) = (*theta1)(1, i) = (*theta1)(2, i) = 0;
    
    if (a2)
        delete a2;
    a2 = new Eigen::MatrixXd(3, training_set_size);
    minimize();
    
    // OUTPUT PREDICTION VS Y
    if (outputStream) {
        std::stringstream out;
        out << "prediction vs Y:\n";
        hypothesis();
        for (int i = 0; i < training_set_size; i++)
        out << "P: "
            << (*a2)(0, i) << ", "
            << (*a2)(1, i) << ", "
            << (*a2)(2, i) << " "
            << " Y: "
            << (*Y)(0, i) << ", "
            << (*Y)(1, i) << ", "
            << (*Y)(2, i) << "\n";
        outputStream->append(out.str().c_str());
    }
}

void NeuralNetwork::predict(){
    if (a2) // new prediction, possible new size to predict
        delete a2;
    a2 = new Eigen::MatrixXd(3, training_set_size);
    hypothesis();
}
