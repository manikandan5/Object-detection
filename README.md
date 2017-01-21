# Object-Detection

This code was written for the course Computer Vision (CSCI-B 657) at Indiana University handled by Professor David Crandall. Skeleton code was provided by the Professor to get us started with the assignment.


**What does the program do?** <br/>
* The program tries to find the type of food item from the given image.
* The program does the same by using multiple feature extraction and the results can be used to compare how well these work for the application.
* The program uses Support Vector Machine for all the feature extraction algorithms.
* The same code can be tweaked to work for different scenarios as well.

**How does it find it?** <br/>

* The program uses three feature extraction algorithms - Eigen, HAAR and Bag of Words.
* Also it tries to classify using Convolutional Neural Networks.

Detailed explanation about how the code works and the reason why we chose this implementation could be found [here](https://github.com/manikandan5/Object-detection/blob/master/A3.pdf).

**How to run the program?** 

This command compiles the program: <br>
    * make 

To run the baseline algorithm: <br>
    * ./a3 train baseline 
      ./a3 test baseline
  
To run the Eigen faces algorithm: <br>
    * ./a3 train eigen 
      ./a3 test eigen

To run the HAAR algorithm: <br>
    * ./a3 train haar 
      ./a3 test haar

To run the Bag of Words algorithm: <br>
    * ./a3 train bow 
      ./a3 test bow

To run the Convolutional Neural Networks: <br>
    * make init
      ./a3 train deep 
      ./a3 test deep
          
