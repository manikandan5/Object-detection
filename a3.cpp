// B657 assignment 3 skeleton code, D. Crandall
//
// Compile with: "make"
//
// This skeleton code implements nearest-neighbor classification
// using just matching on raw pixel values, but on subsampled "tiny images" of
// e.g. 20x20 pixels.
//
// It defines an abstract Classifier class, so that all you have to do
// :) to write a new algorithm is to derive a new class from
// Classifier containing your algorithm-specific code
// (i.e. load_model(), train(), and classify() methods) -- see
// NearestNeighbor.h for a prototype.  So in theory, you really
// shouldn't have to modify the code below or the code in Classifier.h
// at all, besides adding an #include and updating the "if" statement
// that checks "algo" below.
//
// See assignment handout for command line and project specifications.
//
#include "CImg.h"
#include <ctime>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string>
#include <vector>
#include <Sift.h>
#include <sys/types.h>
#include <dirent.h>
#include <map>
#include <numeric>
#include <opencv2/core.hpp>


typedef CImg<double> Image;

//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;
using namespace cv;

// Dataset data structure, set up so that e.g. dataset["bagel"][3] is
// filename of 4th bagel image in the dataset

typedef map<string, vector<string> > Dataset;


#include <Classifier.h>
#include <NearestNeighbor.h>
#include <EigenClassifier.h>
#include <Haar.h>
#include <SVM.h>
#include <neural.h>
#include <svm2.h>
#include <BOW.h>

// Figure out a list of files in a given directory.
//
vector<string> files_in_directory(const string &directory, bool prepend_directory = false)
{
    vector<string> file_list;
    DIR *dir = opendir(directory.c_str());
    if(!dir)
        throw std::string("Can't find directory " + directory);
    
    struct dirent *dirent;
    while ((dirent = readdir(dir)))
        if(dirent->d_name[0] != '.')
            file_list.push_back((prepend_directory?(directory+"/"):"")+dirent->d_name);
    
    closedir(dir);
    return file_list;
}

int main(int argc, char **argv)
{
    try {
        string buildData = "true";
        
        if(argc < 3)
            throw string("Insufficent number of arguments");
        else if(argc>3)
            buildData = argv[3];
        
        string mode = argv[1];
        string algo = argv[2];
        
        // Scan through the "train" or "test" directory (depending on the
        //  mode) and builds a data structure of the image filenames for each class.
        Dataset filenames;
        vector<string> class_list = files_in_directory(mode);
        for(vector<string>::const_iterator c = class_list.begin(); c != class_list.end(); ++c)
            filenames[*c] = files_in_directory(mode + "/" + *c, true);
        
        // set up the classifier based on the requested algo
        Classifier *classifier=0;
        if(algo == "nn")
            classifier = new NearestNeighbor(class_list);
        else if(algo == "haar")
            classifier = new Haar(class_list);
        else if(algo == "baseline")
            classifier = new SVM(class_list);
        else if(algo == "svm2")
            classifier = new SVM2(class_list);
        else if(algo == "eigen")
            classifier = new EigenClassifier(class_list);
        else if(algo == "deep")
            classifier = new Neural(class_list);
        else if(algo == "bow")
            classifier = new BOW(class_list);
        else
            throw std::string("unknown classifier " + algo);
        
        // now train or test!
        if(mode == "train")
        {
            if(algo!="bow")
            {
                classifier->train(filenames);
            }
            else
            {
                if(buildData=="true")
                {
                    classifier->train(filenames);
                    system("./svm_multiclass_learn -c 0.1 bow-train-features bow-model ");
                }
                else
                {
                    system("./svm_multiclass_learn -c 0.1 bow-train-features bow-model ");
                }
            }
        }
        else if(mode == "test")
        {
            if(algo!="bow")
            {
                classifier->test(filenames);
            }
            else
            {
                if(buildData=="true")
                {
                    classifier->test(filenames);
                    system("./svm_multiclass_classify bow-test-features bow-model bow-predictions");
                    system("python score.py bow-predictions bow-test-features");
                }
                else
                {
                    system("./svm_multiclass_classify bow-test-features bow-model bow-predictions");
                    system("python score.py bow-predictions bow-test-features");
                }
                
            }
        }
        else
            throw std::string("unknown mode!");
    }
    catch(const string &err) {
        cerr << "Error: " << err << endl;
    }
}








