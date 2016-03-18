/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Haar.h
 * Author: rakhasan
 *
 * Created on March 11, 2016, 4:53 PM
 */

#ifndef HAAR_H
#define HAAR_H

#include<map>
#include<Classifier.h>
#include<vector>
#include<string>
#include<iostream>
#include<CImg.h>
#include <Vision_3/Utility.h>
#include<ctime>
#include<cmath>
#include <fstream>
using namespace std;

#define POSITIVE true
#define NEGATIVE false
#define USE_OFFSET true
#define NO_OFFSET false
#define X_SKEWED true
#define Y_SKEWED true
#define X_NO_SKEWED false
#define Y_NO_SKEWED false

typedef CImg<double> Image;

enum Position {
    CENTER, LEFT, RIGHT, TOP, BOTTOM, TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT, RANDOM
};

class Filter {
public:

    Filter(int w, int h, bool _p = true, int _x = 0, int _y = 0) : width(w), height(h), x(_x), y(_y), positive(_p) {
    }

    static Filter create_filter(int w, int h, bool _p = true, int _x = 0, int _y = 0) {
        return Filter(w, h, _p, _x, _y);
    }
    int width, height, x, y;
    bool positive;
    vector<Filter> rectangles;

    void place_filter(Filter& rectangle, const Position& position = RANDOM, const bool use_offset = true) {
        //srand(time(NULL));
        int offset_x = 1, offset_y = 1, base_x = 0, base_y = 0;
        switch (position) {
            case CENTER:
                base_x = width / 2 - rectangle.width / 2;
                base_y = height / 2 - rectangle.height / 2;
                offset_x = use_offset ? (base_x / 5) + 1 : 1;
                offset_y = use_offset ? (base_y / 5) + 1 : 1;
                rectangle.x = base_x + (rand() % offset_x * ((rand() % 100) % 2 == 0 ? 1 : -1));
                rectangle.y = base_y + (rand() % offset_y * ((rand() % 100) % 2 == 0 ? 1 : -1));
                break;
            case LEFT:
                offset_x = use_offset ? (width / 2 > rectangle.width ? width / 2 - rectangle.width : width - rectangle.width) : 1;
                rectangle.x = rand() % max(1,offset_x);
                rectangle.y = rand() % max(1, height - rectangle.height);
                break;
            case RIGHT:
                offset_x = use_offset ? (width / 2 > rectangle.width ? width / 2 - rectangle.width : width - rectangle.width) : 1;
                base_x = width - rectangle.width;
                rectangle.x = base_x - rand() % max(1,offset_x);
                rectangle.y = rand() % max(1, height - rectangle.height);
                break;
            case TOP:
                rectangle.x = rand() % max(1, width - rectangle.width);
                offset_y = use_offset ? (height / 2 > rectangle.height ? height / 2 - rectangle.height : height - rectangle.height) : 1;
                rectangle.y = rand() % max(1,offset_y);
                break;
            case BOTTOM:
                rectangle.x = rand() % max(1,width - rectangle.width);
                base_y = height - rectangle.height;
                offset_y = use_offset ? (height / 2 > rectangle.height ? height / 2 - rectangle.height : height - rectangle.height) : 1;
                rectangle.y = base_y - rand() % max(1, offset_y);
                break;
            case TOP_LEFT:
                offset_x = use_offset ? (width / 2 > rectangle.width ? width / 2 - rectangle.width : width - rectangle.width) : 1;
                offset_y = use_offset ? (height / 2 > rectangle.height ? height / 2 - rectangle.height : height - rectangle.height) : 1;
                rectangle.x = rand() % max(1, offset_x);
                rectangle.y = rand() % max(1, offset_y);
                break;
            case BOTTOM_LEFT:
                base_y = height - rectangle.height;
                offset_x = use_offset ? (width / 2 > rectangle.width ? width / 2 - rectangle.width : width - rectangle.width) : 1;
                offset_y = use_offset ? (height / 2 > rectangle.height ? height / 2 - rectangle.height : height - rectangle.height) : 1;
                rectangle.x = rand() % max(1, offset_x);
                rectangle.y = base_y - rand() % max(1, offset_y);
                break;
            case TOP_RIGHT:
                base_x = width - rectangle.width;
                offset_x = use_offset ? (width / 2 > rectangle.width ? width / 2 - rectangle.width : width - rectangle.width) : 1;
                offset_y = use_offset ? (height / 2 > rectangle.height ? height / 2 - rectangle.height : height - rectangle.height) : 1;
                rectangle.x = base_x - rand() % max(1, offset_x);
                rectangle.y = rand() % max(1, offset_y);
                break;
            case BOTTOM_RIGHT:
                base_x = width - rectangle.width;
                base_y = height - rectangle.height;
                offset_x = use_offset ? (width / 2 > rectangle.width ? width / 2 - rectangle.width : width - rectangle.width) : 1;
                offset_y = use_offset ? (height / 2 > rectangle.height ? height / 2 - rectangle.height : height - rectangle.height) : 1;
                rectangle.x = base_x - rand() % max(1, offset_x);
                rectangle.y = base_y - rand() % max(1, offset_y);
                break;
            default:
                rectangle.x = rand() % max(1, width - rectangle.width);
                rectangle.y = rand() % max(1, height - rectangle.height);

        }
    }

    Filter& add_circle(const int radius, const Position& position = RANDOM, const bool use_offset = true,
            const bool sign = NEGATIVE, const bool x_skewed = X_NO_SKEWED, const bool y_skewed = Y_NO_SKEWED) {
        const int size = max(1, static_cast<int> (radius * 0.6));
        const int remainder = (radius - size) / 2;
        Filter rectangle(size, size, sign), dummy(radius, radius);
        place_filter(dummy, position, use_offset);
        rectangle.x = dummy.x + remainder;
        rectangle.y = dummy.y + remainder;
        //cout<<"x:"<<x_skewed<<"\ty:"<<y_skewed<<endl;
        for (int i = 1; i < remainder; ++i) {
            //cout<<"x:"<<rectangle.x-i<<"\ty:"<<rectangle.y+i<<endl;
            if (!y_skewed) {
                rectangles.push_back(Filter(1, rectangle.height - 2 * i, sign, rectangle.x - i, rectangle.y + i));
                rectangles.push_back(Filter(1, rectangle.height - 2 * i, sign, rectangle.x + size + i - 1, rectangle.y + i));
            }
            if (!x_skewed) {
                rectangles.push_back(Filter(rectangle.width - 2 * i, 1, sign, rectangle.x + i, rectangle.y - i));
                rectangles.push_back(Filter(rectangle.width - 2 * i, 1, sign, rectangle.x + i, rectangle.y + size + i - 1));
            }
        }
        rectangles.push_back(rectangle);

        return *this;
    }

    Filter& add_rectangle(Filter rectangle, const Position& position = RANDOM, const bool use_offset = true) {
        place_filter(rectangle, position, use_offset);
        rectangles.push_back(rectangle);
        return *this;
    }

    Filter& add_rectangle(const int width, const int height, bool positive = false,
            const Position& position = RANDOM, const bool use_offset = true) {
        Filter rectangle(width, height, positive);
        place_filter(rectangle, position, use_offset);
        rectangles.push_back(rectangle);
        return *this;
    }
};

class Haar : public Classifier {
public:

    Haar(const vector<string> &_class_list) : Classifier(_class_list) {
        train_model.open("viola-jones-feature-9");
        test_model.open("viola-jones-feature-test-9");
        if (!train_model.is_open() || !test_model.is_open()) {
            cerr << "Failed to create model file\n";
            return;
        }
    }
    
    ~Haar(){train_model.close(); test_model.close();}

    // Nearest neighbor training. All this does is read in all the images, resize
    // them to a common size, convert to greyscale, and dump them as vectors to a file

    virtual void train(const Dataset &filenames) {
        srand(time(NULL));
        //test_integral();return;
        create_filters(); 
        
        int c = 1;
        for (Dataset::const_iterator c_iter = filenames.begin(); c_iter != filenames.end(); ++c_iter) {
            cout << "Processing " << c_iter->first << endl;
            //Utility::CreateDirectory(c_iter->first+"_gray");
            for (int i = 0; i < c_iter->second.size(); i++) {
                write_feature(train_model, c, extract_features(c_iter->second[i]));
//                Image img(c_iter->second[i].c_str());
//                img = img.get_RGBtoHSI().get_channel(2);
//                scale(img);
//                img=img.resize(50,50);
//                string n =c_iter->first+"_gray/"+Utility::NumberToString<int>(i)+".jpg";
//                img.save(n.c_str());
            }
            c++;
        }
        
    }

    void write_feature(ostream& s, int c, const Image& feature_values) {
        s << c << " ";
        for (int k = 0; k < feature_values.width(); ++k) {
            s << k + 1 << ":" << feature_values(k, 0) << " ";
        }
        s << "\n";
    }

    virtual string classify(const string &filename) {
        write_feature(test_model, 0, extract_features(filename));
        return "bagel";
    }

    virtual void load_model() {
        // for (int c = 0; c < class_list.size(); c++)
        //  models[class_list[c] ] = (CImg<double>(("haar_model." + class_list[c] + ".png").c_str()));
        create_filters();
    }



protected:
    // extract features from an image, which in this case just involves resampling and 
    // rearranging into a vector of pixel data.

    CImg<double> extract_features(const string &filename) {
        //scale,rotate and create integral_images
        Image img(filename.c_str());
        img = img.get_RGBtoHSI().get_channel(2);
        //scale(img);		//remove??

        Image rotated_img = img;

        img = img.resize(sampled_image_size, sampled_image_size);

        rotated_img = rotated_img.rotate(90, 2, 0).resize(sampled_image_size, sampled_image_size);
        normalize(img, img.mean(), img.variance());
        normalize(rotated_img, rotated_img.mean(), rotated_img.variance());
        
        Image f1 = apply_filters(get_integral_image(img));

        //const Image& f2 = apply_filters(get_integral_image(img));

        //f1.append(f2);

        return f1.unroll('x');
    }

    Image apply_filters(const Image& integral_img) {
        Image feature_values(1, filters.size());
        for (int i = 0; i < filters.size(); ++i) {
            const Filter& f = filters[i].first;
            const Position& p = filters[i].second;
            int x = 0, y = 0;
            switch (p) {
                case CENTER:
                    x = (integral_img.width() - f.width) / 2;
                    y = (integral_img.height() - f.height) / 2;
                    break;
                case LEFT:
                    x = 0;
                    y = 0;//(integral_img.height() - f.height) / 2;
                    break;
                case RIGHT:
                    x = integral_img.width() - f.width;
                    y = 0;
                    break;
		case TOP:
		    x = 0;//(integral_img.width() - f.width) / 2;
	 	    y = 0;	
		    break;
                case BOTTOM:
                    x = 0;
                    y = integral_img.height() - f.height;
		    break;
            }

            double sum = 0;
            for (int j = 0; j < f.rectangles.size(); ++j) {
                const Filter& r = f.rectangles[j]; //cout<<"r.x:"<<r.x<<"\tr.y:"<< r.y<<endl;
                double s = rectangle_sum(integral_img, make_pair(x + r.x, y + r.y),
                        make_pair( r.x + r.width - 1,  r.y + r.height - 1));
                sum += s; // cout<<"s:"<<s<<endl;
            }
            //cout<<"sum:"<<sum<<"\ttotal:"<<rectangle_sum(integral_img, make_pair(f.x, f.y), 
            //		make_pair(f.x + f.width-1, f.y + f.height-1))<<endl;
            const int sign = f.positive ? 1 : -1;
            feature_values(0, i) = sign *
                    rectangle_sum(integral_img, make_pair(f.x, f.y), make_pair(f.x + f.width - 1, f.y + f.height - 1)) -
                    2 * sign * sum;
        }
        return feature_values;
    }

    void create_filters() {
        cout << "creating filters\n";
        int filter_count = 0;
        add_bagle_filters(sampled_image_size, filters);
        cout << "bagel:" << filters.size() - filter_count << endl;
        filter_count = filters.size();
        //draw_filters(filters, "filters/bagel_filters");
        //filters.clear();
        add_bread_filters(sampled_image_size, filters);
        cout << "bread:" << filters.size() - filter_count << endl;
        filter_count = filters.size();
        //draw_filters(filters, "filters/bread_filters");
        //filters.clear();
        add_brownie_filters(sampled_image_size, filters);
        cout << "brownie:" << filters.size() - filter_count << endl;
        filter_count = filters.size();
        //draw_filters(filters, "filters/brownie_filters");
        //filters.clear();
        add_chiceknuggetnugget_filters(sampled_image_size, filters);
        cout << "chickennugget:" << filters.size() - filter_count << endl;
        filter_count = filters.size();
        //draw_filters(filters, "filters/nugget_filters");
        //filters.clear();
        add_churro_filters(sampled_image_size, filters);
        cout << "churro:" << filters.size() - filter_count << endl;
        filter_count = filters.size();
        //draw_filters(filters, "filters/churro_filters");
        //filters.clear();
        add_croissant_filters(sampled_image_size, filters);
        cout << "croissant:" << filters.size() - filter_count << endl;
        filter_count = filters.size();
        //draw_filters(filters, "filters/croissant_filters");
        //filters.clear();
        add_frenchfry_filters(50, filters);
        cout << "frenchfry:" << filters.size() - filter_count << endl;
        filter_count = filters.size();
        //draw_filters(filters, "filters/frenchfry_filters");
        //filters.clear();
        add_hamburger_filters(sampled_image_size, filters);
        cout << "hamburger:" << filters.size() - filter_count << endl;
        filter_count = filters.size();
        //draw_filters(filters, "filters/hamburger_filters");
        //filters.clear();
        add_hotdog_filters(sampled_image_size, filters);
        cout << "hotdog:" << filters.size() - filter_count << endl;
        filter_count = filters.size();
       // draw_filters(filters, "filters/hotdog_filters");
        //filters.clear();
        
        cout << "total number of filters:" << filters.size() << endl << endl;
        //draw_filters(filters,"all_filters");
    }

    void add_bagle_filters(const int img_size, vector<pair<Filter, Position> >& filters) {
        for (int i = 0; i < 10; ++i) {
            const int outer_size = img_size;
            const int inner_size = static_cast<int> ((float) outer_size * (float) (1.0 - i / 100.0));
            //filters for whole image (black inside white)
            filters.push_back(make_pair(Filter::create_filter(outer_size, outer_size).add_circle(inner_size, CENTER, NO_OFFSET), CENTER));
            filters.push_back(make_pair(Filter::create_filter(outer_size, outer_size).add_circle(inner_size, CENTER), CENTER));
            // (white inside black)
            filters.push_back(make_pair(Filter::create_filter(outer_size, outer_size, NEGATIVE).add_circle(inner_size * .3, CENTER, NO_OFFSET, POSITIVE), CENTER));
            filters.push_back(make_pair(Filter::create_filter(outer_size, outer_size, NEGATIVE).add_circle(inner_size * .3, CENTER, USE_OFFSET, POSITIVE), CENTER));
            //filters for the bagel only (black inside white)
            filters.push_back(make_pair(Filter::create_filter(outer_size * .7, outer_size).add_circle(inner_size * .3, CENTER, NO_OFFSET), CENTER));
            filters.push_back(make_pair(Filter::create_filter(outer_size * .7, outer_size).add_circle(inner_size * .3, CENTER), CENTER));
            // (white inside black)
            filters.push_back(make_pair(Filter::create_filter(outer_size * .7, outer_size, NEGATIVE).add_circle(inner_size * .3, CENTER, NO_OFFSET, POSITIVE), CENTER));
            filters.push_back(make_pair(Filter::create_filter(outer_size * .7, outer_size, NEGATIVE).add_circle(inner_size * .3, CENTER, USE_OFFSET, POSITIVE), CENTER));
            //small filters (black inside white)
            filters.push_back(make_pair(Filter::create_filter(outer_size, outer_size * .5).
                    add_circle(inner_size * .3, TOP_LEFT).
                    add_circle(inner_size * .3, TOP).
                    add_circle(inner_size * .3, TOP_RIGHT), TOP));
            filters.push_back(make_pair(Filter::create_filter(outer_size, outer_size * .5).
                    add_circle(inner_size * .3, BOTTOM_LEFT).
                    add_circle(inner_size * .3, BOTTOM).
                    add_circle(inner_size * .3, BOTTOM_RIGHT), BOTTOM));
        }

    }

    void add_bread_filters(const int img_size, vector<pair<Filter, Position> >& filters) {
        for (int i = 0; i < 10; ++i) {
            //create filters for small, circular buns  (10*6 = 60 filters)     
            int s = static_cast<int> ((float) img_size * (float) (1.0 - i * 2.0 / 100.0));
            filters.push_back(make_pair(Filter::create_filter(s, s).add_circle(s * .6, CENTER), CENTER));
            filters.push_back(make_pair(Filter::create_filter(s, s).add_circle(s * .2, CENTER).add_circle(s * .2, CENTER).add_circle(s * .2, CENTER), CENTER));
            filters.push_back(make_pair(Filter::create_filter(s, s).add_circle(s * .4, LEFT).add_circle(s * .4, RIGHT), CENTER));
            filters.push_back(make_pair(Filter::create_filter(s, s).add_circle(s * .4, TOP).add_circle(s * .4, BOTTOM), CENTER));
            filters.push_back(make_pair(Filter::create_filter(s, s).add_circle(s * .3, TOP_LEFT).add_circle(s * .3, TOP_RIGHT).add_circle(s * .3, BOTTOM_LEFT).add_circle(s * .3, BOTTOM_RIGHT), CENTER));
            filters.push_back(make_pair(Filter::create_filter(s, s).add_circle(s * .3).add_circle(s * .3).add_circle(s * .3).add_circle(s * .3), CENTER));

            //create filters for rectangular slices (10*6 = 60 filters)     
            filters.push_back(make_pair(Filter::create_filter(s, s * .3).add_circle(s * .2, CENTER).add_circle(s * .2, TOP).add_circle(s * .2, BOTTOM), CENTER));
            filters.push_back(make_pair(Filter::create_filter(s, s * .3).add_circle(s * .2, CENTER).add_circle(s * .2, TOP).add_circle(s * .2, BOTTOM), TOP));
            filters.push_back(make_pair(Filter::create_filter(s, s * .3).add_circle(s * .2, CENTER).add_circle(s * .2, TOP).add_circle(s * .2, BOTTOM), BOTTOM));
            filters.push_back(make_pair(Filter::create_filter(s * .3, s).add_circle(s * .2, CENTER).add_circle(s * .2, TOP).add_circle(s * .2, BOTTOM), CENTER));
            filters.push_back(make_pair(Filter::create_filter(s * .3, s).add_circle(s * .2, CENTER).add_circle(s * .2, TOP).add_circle(s * .2, BOTTOM), LEFT));
            filters.push_back(make_pair(Filter::create_filter(s * .3, s).add_circle(s * .2, CENTER).add_circle(s * .2, TOP).add_circle(s * .2, BOTTOM), RIGHT));
        }
    }

    void add_brownie_filters(const int img_size, vector<pair<Filter, Position> >& filters) {
        for (int i = 0; i < 10; ++i) {
            int s = static_cast<int> ((float) img_size * (float) (1.0 - i * 1.0 / 100.0));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).add_rectangle(s, s), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).add_circle(s, CENTER), CENTER));
            for (int j = 1; j <= 5; j++) {
                s = static_cast<int> (s * (float) (1.0 - j * 3.0 / 100.0));
                filters.push_back(make_pair(Filter::create_filter(img_size, img_size).add_circle(s, CENTER), CENTER));
                filters.push_back(make_pair(Filter::create_filter(img_size, img_size).add_circle(s * .7, CENTER).add_circle(s * .7, TOP_LEFT).add_circle(s * .7, TOP_RIGHT).add_circle(s * .7, BOTTOM_LEFT).add_circle(s * .7, BOTTOM_RIGHT), CENTER));
                filters.push_back(make_pair(Filter::create_filter(img_size, img_size).add_circle(s * .7, CENTER).add_circle(s * .7, CENTER).add_circle(s * .7, CENTER).add_circle(s * .7, CENTER), CENTER));
            }
        }
    }

    void add_chiceknuggetnugget_filters(const int img_size, vector<pair<Filter, Position> >& filters) {
        for (int i = 0; i < 5; ++i) {
            int s = static_cast<int> ((float) img_size * (float) (1.0 - i * 3.0 / 100.0));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).add_rectangle(s, s), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).add_circle(s, CENTER), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size, NEGATIVE).add_circle(s, CENTER, USE_OFFSET, POSITIVE), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).add_circle(s, CENTER, USE_OFFSET, NEGATIVE, X_SKEWED, Y_NO_SKEWED), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).add_circle(s, CENTER, USE_OFFSET, NEGATIVE, X_NO_SKEWED, Y_SKEWED), CENTER));

            for (int j = 1; j <= 5; j++) {
                s = static_cast<int> (s * (float) (1.0 - j * 4.0 / 100.0));
                filters.push_back(make_pair(Filter::create_filter(img_size, img_size, POSITIVE)
                        .add_rectangle(s, s * .5, NEGATIVE, CENTER)
                        .add_rectangle(s, s * .5, NEGATIVE, LEFT)
                        .add_rectangle(s, s * .5, NEGATIVE, TOP)
                        .add_rectangle(s, s * .5, NEGATIVE, TOP_LEFT)
                        .add_rectangle(s, s * .5, NEGATIVE, TOP_RIGHT)
                        .add_rectangle(s, s * .5, NEGATIVE, BOTTOM)
                        .add_rectangle(s, s * .5, NEGATIVE, BOTTOM_LEFT)
                        .add_rectangle(s, s * .5, NEGATIVE, BOTTOM_RIGHT)
                        , CENTER));
                filters.push_back(make_pair(Filter::create_filter(img_size, img_size, NEGATIVE)
                        .add_rectangle(s, s * .5, POSITIVE, CENTER)
                        .add_rectangle(s, s * .5, POSITIVE, LEFT)
                        .add_rectangle(s, s * .5, POSITIVE, TOP)
                        .add_rectangle(s, s * .5, POSITIVE, TOP_LEFT)
                        .add_rectangle(s, s * .5, POSITIVE, TOP_RIGHT)
                        .add_rectangle(s, s * .5, POSITIVE, BOTTOM)
                        .add_rectangle(s, s * .5, POSITIVE, BOTTOM_LEFT)
                        .add_rectangle(s, s * .5, POSITIVE, BOTTOM_RIGHT)
                        , CENTER));

            }
            //rest of the filters will be identical to brownie
        }
    }

    void add_churro_filters(const int img_size, vector<pair<Filter, Position> >& filters) {
        for (int i = 0; i < 5; ++i) {
            int s = static_cast<int> ((float) img_size * (float) (1.0 - i * 2.0 / 100.0));
            for (int j = 0; j < 5; ++j) {
                filters.push_back(make_pair(Filter::create_filter(s, s).
                        add_rectangle(s * .8, s * .1, NEGATIVE, CENTER), CENTER));
                filters.push_back(make_pair(Filter::create_filter(s, s).
                        add_rectangle(s * .8, s * .1, NEGATIVE, LEFT).
                        add_rectangle(s * .8, s * .1, NEGATIVE, RIGHT), CENTER));
                filters.push_back(make_pair(Filter::create_filter(s, s).
                        add_rectangle(s * .8, s * .1, NEGATIVE, CENTER).
                        add_rectangle(s * .8, s * .1, NEGATIVE, LEFT).
                        add_rectangle(s * .8, s * .1, NEGATIVE, RIGHT), CENTER));

                filters.push_back(make_pair(Filter::create_filter(s, s).
                        add_rectangle(s * .1, s * .8, NEGATIVE, CENTER), CENTER));
                filters.push_back(make_pair(Filter::create_filter(s, s).
                        add_rectangle(s * .1, s * .8, NEGATIVE, LEFT).
                        add_rectangle(s * .1, s * .8, NEGATIVE, RIGHT), CENTER));
                filters.push_back(make_pair(Filter::create_filter(s, s).
                        add_rectangle(s * .1, s * .8, NEGATIVE, CENTER).
                        add_rectangle(s * .1, s * .8, NEGATIVE, LEFT).
                        add_rectangle(s * .1, s * .8, NEGATIVE, RIGHT), CENTER));
                filters.push_back(make_pair(Filter::create_filter(s, s).
                        add_rectangle(s * .1, s * .8, NEGATIVE, CENTER).
                        add_rectangle(s * .1, s * .8, NEGATIVE, LEFT).
                        add_rectangle(s * .1, s * .8, NEGATIVE, RIGHT).
                        add_rectangle(s * .1, s * .8, NEGATIVE, RIGHT), CENTER));
                filters.push_back(make_pair(Filter::create_filter(s, s).
                        add_rectangle(s * .1, s * .8, NEGATIVE, CENTER).
                        add_rectangle(s * .1, s * .8, NEGATIVE, LEFT).
                        add_rectangle(s * .1, s * .8, NEGATIVE, RIGHT).
                        add_rectangle(s * .1, s * .8, NEGATIVE, LEFT), CENTER));
                filters.push_back(make_pair(Filter::create_filter(s, s).
                        add_rectangle(s * .1, s * .8, NEGATIVE, CENTER).
                        add_rectangle(s * .1, s * .8, NEGATIVE, LEFT).
                        add_rectangle(s * .1, s * .8, NEGATIVE, RIGHT).
                        add_rectangle(s * .1, s * .8, NEGATIVE, RIGHT).
                        add_rectangle(s * .1, s * .8, NEGATIVE, LEFT), CENTER));
            }
        }
    }

    void add_croissant_filters(const int img_size, vector<pair<Filter, Position> >& filters) {
        for (int i = 1; i < 10; ++i) {
            int s = static_cast<int> ((float) img_size * (float) (1.0 - i * 2.0 / 100.0));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).
                    add_circle(s, CENTER, NO_OFFSET, NEGATIVE, X_SKEWED), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).
                    add_circle(s, CENTER, USE_OFFSET, NEGATIVE, X_SKEWED), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).
                    add_circle(s, TOP, USE_OFFSET, NEGATIVE, X_SKEWED), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).
                    add_circle(s, BOTTOM, USE_OFFSET, NEGATIVE, X_SKEWED), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).
                    add_circle(s / 4, TOP_LEFT, USE_OFFSET, NEGATIVE, X_SKEWED).
                    add_circle(s / 4, TOP, USE_OFFSET, NEGATIVE, X_SKEWED).
                    add_circle(s / 4, TOP_RIGHT, USE_OFFSET, NEGATIVE, X_SKEWED).
                    add_circle(s / 4, CENTER, USE_OFFSET, NEGATIVE, X_SKEWED).
                    add_circle(s / 4, BOTTOM_LEFT, USE_OFFSET, NEGATIVE, X_SKEWED).
                    add_circle(s / 4, BOTTOM, USE_OFFSET, NEGATIVE, X_SKEWED).
                    add_circle(s / 4, BOTTOM_RIGHT, USE_OFFSET, NEGATIVE, X_SKEWED), CENTER));

            for (int j = 1; j <= 2; ++j) {
                s = static_cast<int> (s * (float) (1.0 - j * 4.0 / 100.0));
                filters.push_back(make_pair(Filter::create_filter(img_size, img_size / 3).
                        add_rectangle(s / 4, s / 6, NEGATIVE, TOP_LEFT).
                        add_rectangle(s / 4, s / 6, NEGATIVE, TOP, NO_OFFSET).
                        add_rectangle(s / 4, s / 6, NEGATIVE, TOP_RIGHT), TOP));
                filters.push_back(make_pair(Filter::create_filter(img_size, img_size / 3).
                        add_rectangle(s / 4, s / 6, NEGATIVE, TOP_LEFT).
                        add_rectangle(s / 4, s / 6, NEGATIVE, TOP, NO_OFFSET).
                        add_rectangle(s / 4, s / 6, NEGATIVE, TOP_RIGHT), CENTER));
                filters.push_back(make_pair(Filter::create_filter(img_size, img_size / 3).
                        add_rectangle(s / 4, s / 6, NEGATIVE, TOP_LEFT).
                        add_rectangle(s / 4, s / 6, NEGATIVE, TOP, NO_OFFSET).
                        add_rectangle(s / 4, s / 6, NEGATIVE, TOP_RIGHT), BOTTOM));

            }

        }
    }

    void add_frenchfry_filters(const int img_size, vector<pair<Filter, Position> >& filters) 
    {
        for (int i = 1; i < 5; ++i)
        {
            int s = static_cast<int> ((float) img_size * (float) (1.0 - i * 3.0 / 100.0));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).                        
                        add_rectangle(s , s / 2, NEGATIVE, TOP, NO_OFFSET), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).                        
                        add_rectangle(s , s / 2, NEGATIVE, BOTTOM, NO_OFFSET), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).                        
                        add_rectangle(s , s / 2, NEGATIVE, LEFT, NO_OFFSET), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size).                        
                        add_rectangle(s , s / 2, NEGATIVE, RIGHT, NO_OFFSET), CENTER));
            
        }
    }
    
    void add_hamburger_filters(const int img_size, vector<pair<Filter, Position> >& filters) {
        for (int i = 0; i < 5; ++i) {
            int outer_size = img_size;
            int inner_size = static_cast<int> ((float) outer_size * (float) (1.0 - i / 100.0));
            //filters for whole image (black inside white)
            filters.push_back(make_pair(Filter::create_filter(outer_size, outer_size).add_circle(inner_size, CENTER, NO_OFFSET), CENTER));
            filters.push_back(make_pair(Filter::create_filter(outer_size, outer_size).add_circle(inner_size, CENTER), CENTER));
            //horizontal filter for burger middle            
            for (int j = 1; j < 5; j++) {
                outer_size = static_cast<int> ((float) outer_size * (float) (1.0 - j * 2 / 100.0));
                inner_size = static_cast<int> ((float) inner_size * (float) (1.0 - j * 5 / 100.0));
                filters.push_back(make_pair(Filter::create_filter(outer_size, outer_size).add_rectangle(outer_size, inner_size * .5, NEGATIVE, CENTER, NO_OFFSET), CENTER));
                filters.push_back(make_pair(Filter::create_filter(outer_size, outer_size).add_rectangle(outer_size, inner_size * .5, NEGATIVE, CENTER), CENTER));
                filters.push_back(make_pair(Filter::create_filter(outer_size, outer_size).add_circle(inner_size * .7, CENTER, NO_OFFSET, NEGATIVE, X_SKEWED), CENTER));
                filters.push_back(make_pair(Filter::create_filter(outer_size, outer_size).add_circle(inner_size * .7, CENTER, USE_OFFSET, NEGATIVE, X_SKEWED), CENTER));
            }

        }
    }

    void add_hotdog_filters(const int img_size, vector<pair<Filter, Position> >& filters) {
        //const int radius, const Position& position = RANDOM, const bool use_offset = true,
        //const bool sign = NEGATIVE, const bool x_skewed = X_NO_SKEWED, const bool y_skewed = Y_NO_SKEWED)
        for (int i = 0; i < 5; ++i) {
            int s1 = static_cast<int> ((float) img_size * (float) (1.0 - i * 1.0 / 100.0));
            for (int j = 0; j < 4; ++j) {
                //filter  for whole image
                int s2 = s1 * .8;
                filters.push_back(make_pair(Filter::create_filter(s1, s1).
                        add_rectangle(s2, s2 * .5, NEGATIVE, CENTER, NO_OFFSET), CENTER));
                filters.push_back(make_pair(Filter::create_filter(s1, s1).
                        add_rectangle(s2, s2 * .5, NEGATIVE, CENTER, USE_OFFSET), CENTER));
                filters.push_back(make_pair(Filter::create_filter(s1, s1).
                        add_rectangle(s2, s2 * .5, NEGATIVE, TOP, USE_OFFSET).
                        add_rectangle(s2, s2 * .5, NEGATIVE, BOTTOM, USE_OFFSET), CENTER));
                filters.push_back(make_pair(Filter::create_filter(s1, s1).
                        add_circle(s2 * .9, CENTER, USE_OFFSET, NEGATIVE, X_SKEWED), CENTER));
                filters.push_back(make_pair(Filter::create_filter(s1, s1).
                        add_circle(s2 * .9, CENTER, USE_OFFSET, NEGATIVE, X_SKEWED).
                        add_rectangle(s2 * .7, s2 * .4, NEGATIVE, CENTER, NO_OFFSET), CENTER)); // double black in center, remove??

                //filter  for whole hotdog
                s1 *= .8;
                s2 *= .8;
                filters.push_back(make_pair(Filter::create_filter(s1, s1).
                        add_rectangle(s2, s2 * .5, NEGATIVE, CENTER, NO_OFFSET), CENTER)); //horizontal
                filters.push_back(make_pair(Filter::create_filter(s1, s1).
                        add_rectangle(s2, s2 * .5, NEGATIVE, CENTER), CENTER)); //horizontal                
            }
        }
        //some filters for the middle cream white , Remove??
        for (int i = 1; i <= 10; ++i) {
            int s1 = static_cast<int> ((float) img_size * (float) (1.0 - i * 1.0 / 100.0));
            filters.push_back(make_pair(Filter::create_filter(s1, s1 * .5, NEGATIVE).
                    add_rectangle(s1 * .9, s1 * .1, POSITIVE, CENTER, USE_OFFSET), CENTER)); //horizontal

        }
        // draw_filters(filters, "hotdog_filters");
    }

    void add_popcorn_filters(const int img_size, vector<pair<Filter, Position> >& filters) {
        //const int radius, const Position& position = RANDOM, const bool use_offset = true,
        //const bool sign = NEGATIVE, const bool x_skewed = X_NO_SKEWED, const bool y_skewed = Y_NO_SKEWED)
        for (int i = 1; i <= 10; ++i) {
            int s = static_cast<int> ((float) img_size * (float) (1.0 - i * 1.0 / 100.0));
            //filters for whole image (white inside black)
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size, NEGATIVE).
                    add_circle(s, CENTER, NO_OFFSET, POSITIVE), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size, NEGATIVE).
                    add_circle(s, CENTER, USE_OFFSET, POSITIVE), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size, NEGATIVE).
                    add_rectangle(s, s, POSITIVE, CENTER, USE_OFFSET), CENTER));
            filters.push_back(make_pair(Filter::create_filter(img_size, img_size, NEGATIVE).
                    add_rectangle(s, s, POSITIVE, CENTER, NO_OFFSET), CENTER));
        }
    }

    void add_pudding_filters(const int img_size, vector<pair<Filter, Position> >& filters) {
        //filters should be large white rectangle/circle inside black box (similar to popcorn filters)
    }

    void normalize(Image& img, const double mean, const double variance) {
        double sigma = sqrt(variance);
        // cout << "mean:" << mean << "\tstd:" << sigma << endl;
        for (int i = 0; i < img.width(); ++i)
            for (int j = 0; j < img.height(); ++j)
                img(i, j) = (img(i, j) - mean) / sigma;
    }

    void scale(Image& img) {
        for (int i = 0; i < img.width(); ++i)
            for (int j = 0; j < img.height(); ++j)
                img(i, j) *= 255;
    }

    void draw_filters(const vector<Filter>& filters, const string& directory) {
        vector<pair<Filter, Position> > f;
        for (int i = 0; i < filters.size(); ++i)
            f.push_back(make_pair(filters[i], CENTER));
        draw_filters(f, directory);
    }

    void draw_filters(const vector<pair<Filter, Position> >& filters, const string& directory) {
        int a = 0, b = 0;
        Utility::CreateDirectory(directory);
        for (int i = 0; i < filters.size(); ++i) {
            const Filter& f = filters[i].first;
            CImg<int> outer_rect(f.width, f.height, 1, 1, f.positive ? 255 : 0);
            if (f.width == 0 || f.height == 0) {
                cout << "zero filter\n";
                continue;
            }
            for (int j = 0; j < f.rectangles.size(); ++j) {
                const Filter& r = f.rectangles[j];
                if (r.width == 0 || r.height == 0) {
                    cout << "zero rectangle\n";
                    continue;
                }
                CImg<int> inner_rect(r.width, r.height, 1, 1, r.positive ? 255 : 0);
                if (r.x < 0 || r.y < 0) {
                    cout << "bound error:" << r.x << "," << r.y << endl;
                    a++;
                    continue;
                }
                if (r.x + r.width > f.width || r.y + r.height > f.height) {
                    cout << "overflow:" << r.x << "," << r.width << "," << r.y << "," << r.height << ", f:" << f.width << "," << f.height << endl;
                    b++;
                    continue;
                }
                outer_rect.draw_image(r.x, r.y, 0, 0, inner_rect);
            }
            string name = directory + "/" + Utility::NumberToString<int>(i) + ".jpg";
            /*Utility::NumberToString<int>(outer_size)+"_"+ 
            Utility::NumberToString<int>(inner_size)+"_"+
            Utility::NumberToString<int>(j)  +"_"+
            Utility::NumberToString<int>(x_offset)  +"_"+
            Utility::NumberToString<int>(y_offset)  +"_"+".jpg";*/
            outer_rect.save(name.c_str());
        }
        cout << "a:" << a << "\tb:" << b << endl;
    }

    Image get_integral_image(const Image& input_img) {
        Image integral_img = input_img;

        for (int row = 0; row < input_img.height(); ++row) {
            for (int col = 0; col < input_img.width(); ++col) {
                const int top = row <= 0 ? 0 : integral_img(col, row - 1);
                const int left = col <= 0 ? 0 : integral_img(col - 1, row);
                const int t_l = row > 0 && col > 0 ? integral_img(col - 1, row - 1) : 0;
                integral_img(col, row) += top + left - t_l;
            }
        }

        return integral_img;
    }

    double rectangle_sum(const Image& integral_img,
            const pair<int, int>& top_left,
            const pair<int, int>& bottom_right) {
        //cout<<"top left:"<<top_left.first<<","<<top_left.second<<endl<<"bottom_right:"<<bottom_right.first<<","<<bottom_right.second<<endl;
        const double up_value = top_left.second > 0 ?
                integral_img(bottom_right.first, top_left.second - 1) : 0;
        const double left_val = top_left.first > 0 ?
                integral_img(top_left.first - 1, bottom_right.second) : 0;
        const double corner_val = top_left.first > 0 && top_left.second > 0 ?
                integral_img(top_left.first - 1, top_left.second - 1) : 0;
        //cout<<"u:"<<up_value<<"\tl:"<<left_val<<"\tc:"<<corner_val<<endl;
        return integral_img(bottom_right.first, bottom_right.second) -
                up_value -
                left_val +
                corner_val;
    }

    void print(const Image& img, const string& note = " ") {
        cout << "\n\n" << note << endl;
        for (int row = 0; row < img.height(); ++row) {
            for (int col = 0; col < img.width(); ++col) {
                cout << img(col, row) << "\t";
            }
            cout << endl;
        }
        cout << endl << endl;
    }

    void test_integral() {
        Image img(4, 4);
        int p = 1;
        srand(time(NULL));
        cout << img.width() << "\t" << img.height() << endl << endl;

        for (int row = 0; row < img.height(); ++row) {
            for (int col = 0; col < img.width(); ++col) {
                img(col, row) = p; //* ((rand() % 100) % 2 == 0 ? 1 : -1);
                p++;
            }
        }
        print(img, "original image");
        img = get_integral_image(img);
        print(img, "integral image");


//        cout << rectangle_sum(img, make_pair(0, 0), make_pair(0, 0)) << endl << endl;
//        cout << rectangle_sum(img, make_pair(0, 0), make_pair(3, 3)) << endl << endl;
//        cout << rectangle_sum(img, make_pair(3, 3), make_pair(3, 3)) << endl << endl;
//        cout << rectangle_sum(img, make_pair(1, 1), make_pair(3, 3)) << endl << endl;
//        cout << rectangle_sum(img, make_pair(0, 0), make_pair(0, 3)) << endl << endl;
//        cout << rectangle_sum(img, make_pair(1, 1), make_pair(3, 3)) << endl << endl;
//        cout << rectangle_sum(img, make_pair(1, 2), make_pair(3, 2)) << endl << endl;

        Filter f1(4, 4), f2(4, 4, NEGATIVE), f3(3, 3);
        f1.add_rectangle(2, 2, NEGATIVE, TOP_LEFT, NO_OFFSET).add_rectangle(2, 2, NEGATIVE, BOTTOM_LEFT, USE_OFFSET);
        f2.add_rectangle(2, 2, POSITIVE, TOP_LEFT, NO_OFFSET).add_rectangle(2, 2, POSITIVE, BOTTOM_RIGHT, USE_OFFSET);
        f3.add_rectangle(2, 2, NEGATIVE, CENTER, USE_OFFSET);
        
        cout<<"f1-x:"<<f1.rectangles[0].x<<"\tf1-y:"<<f1.rectangles[0].y<<endl;
        cout<<"f1-x:"<<f1.rectangles[1].x<<"\tf1-y:"<<f1.rectangles[1].y<<endl;
        cout<<"f2-x:"<<f2.rectangles[0].x<<"\tf2-y:"<<f2.rectangles[0].y<<endl;
        cout<<"f2-x:"<<f2.rectangles[1].x<<"\tf2-y:"<<f2.rectangles[1].y<<endl;
        cout<<"f3-x:"<<f3.rectangles[0].x<<"\tf3-y:"<<f3.rectangles[0].y<<endl;
        
        filters.push_back(make_pair(f1, CENTER));

        Image feature = apply_filters(img);
        print(feature, "features");

        filters.clear();
        filters.push_back(make_pair(f2, CENTER));
        feature = apply_filters(img);
        print(feature, "features");
        
        filters.clear();
        filters.push_back(make_pair(f3, CENTER));
        feature = apply_filters(img);
        print(feature, "features");


    }

    static const int sampled_image_size = 50; // subsampled image resolution
    map<string, CImg<double> > models; // trained models
    vector<pair<Filter, Position> > filters;
    ofstream train_model, test_model;
};


#endif /* HAAR_H */

