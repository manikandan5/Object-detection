#include "CImg.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <vector>

using namespace std;
const string overfeat = "./overfeat/bin/linux_64/overfeat ";
const string overfeat_b = "./overfeat/bin/linux_64/overfeat_batch ";
class Neural : public Classifier {
public:
  Neural(const vector<string> &_class_list) : Classifier(_class_list) {}
  // SVM training
  virtual void train(const Dataset &filenames)
  {
    int classValue=1;
    ofstream fout("train_n_svm.data");
    for(Dataset::const_iterator c_iter=filenames.begin(); c_iter != filenames.end(); ++c_iter) {
      int k = 0;
      for(int i=0; i<c_iter->second.size(); ++i) {
      	string fname = c_iter->second[i];
	string f = fname;
	replace(f.begin(),f.end(),'/','_');
	f = "./n/"+f;
      	get_file(fname).save(f.c_str());
	cout << " Processing file " << f << endl;
      	string out = run_system(overfeat+" -L 15 "+f);
      	//remove("overtmp.jpg");
	build_svm_n(out,fout,classValue);
      }
      classValue++;
    }
    fout.close();
    system("./svm_bin/svm_multiclass_learn -c 1 train_n_svm.data svm_neural_train");
  }

  void build_svm_n(string out,ofstream &fout,int classValue=1) {
    stringstream tmp;
    tmp<< out ;
    int n,l,h;
    tmp >> n; tmp>> l; tmp >> h;
    int i =  1;
    fout << classValue << " ";
    while(n>=0) {
      double t;
      tmp >> t;
      fout << i << ":"<< t << " ";
      n--;
      i++;
    }
    fout << endl;
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
    ofstream fout("test_n_tmp.data");
    string f = filename;
    replace(f.begin(),f.end(),'/','_');
    f = "./n/"+f;
    get_file(filename).save(f.c_str());
    int i = get_index(filename,class_list);
    string out = run_system((overfeat+" -f " + f).c_str());
    //    remove("overtmp.jpg");
    build_svm_n(out,fout,i+1);
    fout.close();
    system("./svm_bin/svm_multiclass_classify test_n_tmp.data svm_neural_train classify.tmp");
    ifstream cl("classify.tmp");
    string line;
    getline(cl,line);
    int num ;
    stringstream tmp;
    tmp << line;
    tmp >> num;
    cl.close();
    remove ("test_n_tmp.data");
    remove("classify.tmp");
    return class_list[num-1];
  }

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

  CImg<double> get_file(const string &filename) {
    CImg<double> img(filename.c_str());
    return img.resize(size,size);
  }

  CImg<double> extract_features(const string &filename)
  {
    // In grayscale
    CImg<double> img(filename.c_str());
    // img._spectrum = 1;
    return img.resize(size,size).unroll('x');
  }

  static const int size=231;  // subsampled image resolution
  map<string, CImg<double> > models; // trained models
};
