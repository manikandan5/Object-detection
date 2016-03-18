#include "CImg.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <map>
#include <numeric>

using namespace cimg_library;
using namespace std;

class EigenClassifier : public Classifier {
public:
  EigenClassifier(const vector<string> &_class_list) : Classifier(_class_list) {}
  virtual void train(const Dataset &filenames) {
    for(Dataset::const_iterator c_iter=filenames.begin(); c_iter != filenames.end(); ++c_iter) {
      cout << "Processing " << c_iter->first << endl;
      CImg<double> A(filenames.size(),size*size,1);
      unsigned int R = filenames.size() - 1;
      CImg<double> U(R,size * size,1);
      // convert each image to be a row of this "model" image
      // THIS CREATES THE BIG MATRIX
      cout << " Total Number of training images are " << c_iter->second.size() << "\n";
      for(int i=0; i<c_iter->second.size(); i++) {
	CImg<double> e = extract_features(c_iter->second[i].c_str());
	cout<< "EImage w:h of is " << e._width  << " : " << e._height << "\n";
	double tot=0,avg = 0;
	cimg_forY(e,y) {
	  tot += e(0,y,0);
	}
	avg = tot/e._height;
	cimg_forY(e,y) {
	  e(0,y,0) -= avg;
	}
        A.draw_image(i, 0, 0, 0, e);
      }
      CImg<double> At = A.get_transpose();
      CImg<double> L = At * A;

      cout<< " Widht:h of L is " << L._width  << " : " << L._height << "\n";

      CImg<double> eig_vector,eig_values;
      L.symmetric_eigen(eig_values,eig_vector);

      cout<<"Eigen vector size " << eig_vector._width << " : " << eig_vector._height << "\n";
      cout<<"Eigen Value size " << eig_values._width << " : " << eig_values._height << "\n";
      // R = eig_vector.size() - 1;

      for (unsigned int i = 0; i <= R; i++) {
	CImg<double> vi = eig_vector.get_column(i);
	// Could be row transpose or the column - Need to understand it
	// CImg<double> vi = eig_vector.get_row(i).transpose();
	CImg<double> ui = A * vi;
	// Normalize it
	ui.normalize();
	U.draw_image(0, i, 0, 0, ui);
      }
      // cimg_forXY(eig_values,x,y) {
      // 	cout << "Eigen values - "<< x << ":" << y << " -> " << eig_values(x,y) << "\n";
      // }
      U.save_png(("nn_model." + c_iter->first + ".png").c_str());
    }
  }

  virtual string classify(const string &filename)
  {
    CImg<double> test_image = extract_features(filename);
    // figure nearest neighbor
    pair<string, double> best("", 10e100);
    double this_cost;
    return best.first;
  }

  virtual void load_model() {
  }
protected:
  // extract features from an image, which in this case just involves resampling and
  // rearranging into a vector of pixel data.
  CImg<double> extract_features(const string &filename) {
    CImg<double> i(filename.c_str());
    cout << "Size :" << size << "\n";
    i.resize(size,size);
    CImg<double> k(1,size*size,1);
    int yy = 0;
    cimg_forY(i,y) {
      cimg_forX(i,x) {
	k(0,yy) = i(x,y);
	yy++;
      }
    }
    if (yy < size*size)
      cout<<"Warning: Error with flattening image\n";
    return k;
  }

  static const int size=10;  // subsampled image resolution
  map<string, CImg<double> > models; // trained models
};
