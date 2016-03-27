#include "CImg.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <map>
#include <numeric>

using namespace cimg_library;
using namespace std;
// Instructions from http://blog.manfredas.com/eigenfaces-tutorial
class EigenClassifier : public Classifier {
public:
  EigenClassifier(const vector<string> &_class_list) : Classifier(_class_list) {}
  virtual void train(const Dataset &filenames) {
    int classvalue = 1;
    std::ofstream fout("train_eigen_svm.data");
    for(Dataset::const_iterator c_iter=filenames.begin(); c_iter != filenames.end(); ++c_iter) {
      double M = c_iter->second.size();
      CImg<double> A(M,size*size,1);
      // convert each image to be a row of this "model" image
      // THIS CREATES THE BIG MATRIX
      cout << " Total Number of training images are " << M << "\n";
      // Get average value among all images
      CImg<double> total(extract_features(c_iter->second[0].c_str()));
      for(int i=1; i< M; i++) {
	// cout << "Adding image : " << c_iter->second[i] << "\n";
	CImg<double> e = extract_features(c_iter->second[i].c_str());
	// cimg_forXYC(total,x,y,C) total(x,y) = C + e(x,y);
	total += e;
      }
      // cimg_forXYC(total,x,y,C) total(x,y) = C/M;
      total /= M;
      cout << " Size of Total is " << total._width << "-" << total._height << "\n";
      get_unfolded(total).save_jpeg(("traintotal/nn_"+c_iter->first + ".jpg").c_str());
      for(int i=0; i< M; i++) {
	CImg<double> e = extract_features(c_iter->second[i].c_str());
	e -= total;
	// double max=-1000000 , min=1000000;
	// cimg_forXY(e,x,y) {
	//   double C = e(x,y);
	//   if (C > max) max = C;
	//   if (C < min) min = C;
	// }
	// cout<<"Average image - max "<<max << " _ min "<< min <<"\n" ;
	// TEST - Show the face after removing average
	ostringstream convert;   // stream used for the conversion
	convert << i;      // insert the textual representation of 'Number' in the characters in the stream
	get_unfolded(e).save_jpeg(("trainaverage/nn_e"+c_iter->first + convert.str() + ".jpg").c_str());
	A.draw_image(i, 0, 0, 0, e);
      }
      // Save the created A
      A.save_jpeg(("train_a/"+c_iter->first + ".jpg").c_str());
      A.get_transpose().save_jpeg(("train_b/"+c_iter->first + ".jpg").c_str());

      CImg<double> At = A.get_transpose();
      CImg<double> L = At * A;

      cout<< " Widht:h of L is " << L._width  << " : " << L._height << " Where A has size " << A._width << ":" << A._height << "\n";

      CImg<double> eig_vector,eig_values,dummy;
      // L.symmetric_eigen(eig_values,eig_vector);
      L.SVD(eig_vector,eig_values,dummy);
      // What are the eigen values possible ?
      cout<<"Eigen vector size " << eig_vector._width << " : " << eig_vector._height << "\n";
      cout<<"Eigen Value size " << eig_values._width << " : " << eig_values._height << "\n";

      CImg<double> eigenValuesPowerNHalf = CImg<double>(eig_values,false);
      cimg_forXY(eigenValuesPowerNHalf,x,y) {
	if (eigenValuesPowerNHalf(x,y) != 0)
	  eigenValuesPowerNHalf(x,y) = 1/sqrt(eigenValuesPowerNHalf(x,y));
      }
      // cout << "Eigen values ";
      // cimg_forY(eig_values,y)
      // 	cout << eig_values(0,y) << ",";
      // cout << endl;
      // cout << "Eigen Vectors ";
      // cimg_forY(eig_vector,y) {
      // 	cimg_forX(eig_vector,x)
      // 	  cout << eig_vector(x,y) << " ";
      // 	cout << endl;
      //}

      // R = eig_vector.size() - 1;
      unsigned int R = 10;
      CImg<double> U(R,size * size,1);
      for (unsigned int i = 0; i < R; i++) {
	CImg<double> vi = eig_vector.get_row(i).transpose();
	// Could be row transpose or the column - Need to understand it
	// CImg<double> vi = eig_vector.get_row(i).transpose();
	CImg<double> ui = A * vi;
	ui /= eigenValuesPowerNHalf[i];
	// Normalize it
	// ui.normalize(0,255);
	// FOR TESTING
	ostringstream convert;   // stream used for the conversion
	convert << i;      // insert the textual representation of 'Number' in the characters in the stream
	get_unfolded(ui).save_jpeg(("traintest/"+ convert.str() + (string)"s." + c_iter->first + ".jpg").c_str());
	U.draw_image(0, i, 0, 0, ui);
      }
      // cimg_forXY(eig_values,x,y) {
      // 	cout << "Eigen values - "<< x << ":" << y << " -> " << eig_values(x,y) << "\n";
      // }

      string eigenfname = "model_eigen." + c_iter->first + ".jpg";
      U.save_jpeg((eigenfname).c_str());
      build_svm_data(U.transpose(),fout,classvalue);
      classvalue++;
    }
    fout.close();
    system("./svm_bin/svm_multiclass_learn -c 1 train_eigen_svm.data svm_eigen_train");
  }

  void build_svm_data(string fname, ofstream &fout,int classValue=1) {
    CImg<double> feature_vector = extract_features(fname.c_str());
    build_svm_data(feature_vector,fout,classValue);
  }

  void build_svm_data(CImg<double> feature_vector, ofstream &fout,int classValue=1) {
    // int i = 0;
    feature_vector.normalize(0,255);
    cimg_forY(feature_vector,y) {
      fout<<classValue << " ";
      cimg_forX(feature_vector,x) {
	fout<<(x+1)<<":"<<feature_vector(x,y)<<" ";
	// i++;
      }
      fout<<"\n";
    }
  }

  CImg<double> get_unfolded(CImg<double> i) {
    double k = (int)sqrt(i._height);
    CImg<double> ret(k,k);
    int x = 0 , y = 0;
    cimg_forC(i,c) {
      ret(x,y) = c;
      if (x >= k) {
	y++;
	x = 0;
      } else {
	x++;
      }
    }
    return ret;
  }

  virtual CImg<double> get_mean_adjusted_image(CImg<double> img) {
    CImg<double> e(img);
    cout<< "Get_mean_adjusted_mage w:h of is " << e._width  << " : " << e._height << "\n";
    double tot=0,avg = 0;
    double count = 0;
    cimg_forXY(e,x,y) {
      tot += e(x,y);
      count++;
    }
    if (count > 0)
      avg = tot/count;
    else avg = 0;
    cimg_forXY(e,x,y) {
      e(x,y) -= avg;
    }
    return e;
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
    CImg<double> test_image = extract_features(filename);
    // Now let get Ut * (Meaned out test_image)
    // phi is meaned out test_image
    // CImg<double> phi = test_image - test_image.mean();
    // cout<< " Test image is "<< filename << "\n";
    // cout<< " Test image after subtracting average is "<< phi.mean() << "\n";
    // double threshold = 0.05;
    // string ret = "";
    // double min_minval = 100;
    // Should be using an SVM here
    int i = get_index(filename,class_list);
    ofstream fout("test_svm_tmp.data");
    build_svm_data(test_image.transpose(),fout,i+1);
    fout.close();
    system("./svm_bin/svm_multiclass_classify test_svm_tmp.data svm_eigen_train classify.tmp");
    ifstream cl("classify.tmp");
    string line;
    getline(cl,line);
    int num ;
    stringstream tmp;
    tmp << line;
    tmp >> num;
    cl.close();
    remove ("test_svm_tmp.data");
    remove("classify.tmp");
    return class_list[num-1];
  }

  virtual void load_model() {
    // Get all the images from U
    // for(int c=0; c < class_list.size(); c++)
    //   models[class_list[c] ] = (CImg<double>(("nn_model." + class_list[c] + ".png").c_str()));
  }
protected:
  // extract features from an image, which in this case just involves resampling and
  // rearranging into a vector of pixel data.
  CImg<double> extract_features(const string &filename) {
    CImg<double> i(filename.c_str());
    string f = filename;
    replace(f.begin(),f.end(),'/','_');
    i._spectrum = 1;
    i.resize(size,size);
    get_unfolded(i.get_vector()).save_jpeg(("trainextract/"+f).c_str());

    return i.get_vector();

    // OK ISSUES
    CImg<double> k(1,size*size,1);
    int yy = 0;
    cimg_forY(i,y) {
      cimg_forX(i,x) {
	k(0,yy) = i(x,y);
	yy++;
      }
    }
    // TEST - Show the face after removing average

    get_unfolded(k).save_jpeg(("trainextract/nn_"+f).c_str());

    if (yy < size*size)
      cout<<"Warning: Error with flattening image\n";
    return k;
  }

  static const int size=40;  // subsampled image resolution
  map<string, CImg<double> > models; // trained models
};
