#include<fstream>
#include<iostream>

class SVM : public Classifier
{
public:
    SVM(const vector<string> &_class_list) : Classifier(_class_list) {}
    
    // SVM training
    virtual void train(const Dataset &filenames)
    {
        std::ofstream fout("train_svm.data");
        build_svm_data(filenames,fout);
        fout.close();
    }
    
    void build_svm_data(const Dataset &filenames, ofstream &fout, bool isTrain=true)
    {
        int classValue=1;
        
        for(Dataset::const_iterator c_iter=filenames.begin(); c_iter != filenames.end(); ++c_iter)
        {
            cout<<"SVM Processing " << c_iter->first << endl;
            
            for(int i=0; i<c_iter->second.size(); ++i)
            {
                fout<<classValue;
                
                CImg<double> feature_vector = extract_features(c_iter->second[i].c_str());
                
                for(int j=0; j<feature_vector.width(); ++j)
                {
                    fout<<"\t"<<(j+1)<<":"<<feature_vector(j,0);
                }
                
                fout<<"\n";
                
            }
            
            classValue++;
        }
    }
    
    virtual string classify(const string &filename)
    {
        return 0;
    }
    
    virtual void test(const Dataset &filenames)
    {
        std::ofstream fout("test_svm.data");
        cout<<"\nBuilding SVM Test Data\n";
        build_svm_data(filenames, fout, false);
        fout.close();
    }
    
    virtual void load_model()
    {
        for(int c=0; c < class_list.size(); c++)
            models[class_list[c] ] = (CImg<double>(("nn_model." + class_list[c] + ".png").c_str()));
    }
protected:
    // extract features from an image, which in this case just involves resampling and
    // rearranging into a vector of pixel data.
    CImg<double> extract_features(const string &filename)
    {
        return (CImg<double>(filename.c_str())).resize(size,size,1,3).unroll('x');
    }
    
    static const int size=40;  // subsampled image resolution
    map<string, CImg<double> > models; // trained models
};