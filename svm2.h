#include "CImg.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <vector>

using namespace std;

class SVM2 : public Classifier {
public:
  SVM2(const vector<string> &_class_list) : Classifier(_class_list) {}
  // SVM training
  virtual void train(const Dataset &filenames)
  {
    std::ofstream fout("train_svm.data");
    int classValue=1;
    for(Dataset::const_iterator c_iter=filenames.begin(); c_iter != filenames.end(); ++c_iter) {
      cout<<"SVM Processing " << c_iter->first << endl;
      for(int i=0; i<c_iter->second.size(); ++i) {
	build_svm_data(c_iter->second[i],fout,classValue);
      }
      classValue++;
    }
    fout.close();
    system("./svm_bin/svm_multiclass_learn -c 1 train_svm.data svm_train");
  }

  void build_svm_data(string fname, ofstream &fout,int classValue=1) {
    fout<<classValue;
    CImg<double> feature_vector = extract_features(fname.c_str());
    int i = 0;
    cimg_forXY(feature_vector,x,y) {
      // cout<<" "<<feature_vector(x,y);
      fout<<" "<<(x+1)<<":"<<feature_vector(x,y);
      i++;
    }
    // cout << endl;
    fout<<"\n";
  }

  int get_index(string f,vector<string> class_list) {
    for (int i = 0; i < class_list.size();i++) {
      size_t found = f.find(class_list[i]);
      if (found!=std::string::npos)
	return i;
    }
    return 0;
  }

  virtual string classify(const string &filename) {
    int i = get_index(filename,class_list);
    cout<< " Filename is " << filename << "\n";
    CImg<double> feature_vector = extract_features(filename);
    std::ofstream fout("test_svm_tmp.data");
    build_svm_data(filename,fout,i+1);
    fout.close();
    system("./svm_bin/svm_multiclass_classify test_svm_tmp.data svm_train classify.tmp");
    ifstream cl("classify.tmp");
    string line;
    getline(cl,line);
    int num ;
    stringstream tmp;
    tmp << line;
    tmp >> num;
    cl.close();
    // remove ("test_svm_tmp.data");
    // remove("classify.tmp");
    return class_list[num-1];
  }

  // virtual void test(const Dataset &filenames) {
  //   std::ofstream fout("test_svm.data");
  //   cout<<"\nBuilding SVM Test Data\n";
  //   build_svm_data(filenames, fout, false);
  //   fout.close();
  // }

  virtual void load_model() {
    // for(int c=0; c < class_list.size(); c++)
    //   models[class_list[c] ] = (CImg<double>(("nn_model." + class_list[c] + ".png").c_str()));
  }
protected:
  // extract features from an image, which in this case just involves resampling and
  // rearranging into a vector of pixel data.
  string run_system(string cmd) {
    string f = " > test.txt";
    system((cmd +f).c_str());
    ifstream t("test.txt");
    stringstream buffer;
    buffer << t.rdbuf();
    remove("test.txt");
    return buffer.str();
  }

  CImg<double> extract_features(const string &filename)
  {
    // In grayscale
    CImg<double> img(filename.c_str());
    img._spectrum = 1;
    return img.resize(size,size).unroll('x');
  }

  static const int size=40;  // subsampled image resolution
  map<string, CImg<double> > models; // trained models
};
