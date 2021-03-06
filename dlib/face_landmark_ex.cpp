// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*

    This example program shows how to find frontal human faces in an image and
    estimate their pose.  The pose takes the form of 68 landmarks.  These are
    points on the face such as the corners of the mouth, along the eyebrows, on
    the eyes, and so forth.  
    


    The face detector we use is made using the classic Histogram of Oriented
    Gradients (HOG) feature combined with a linear classifier, an image pyramid,
    and sliding window detection scheme.  The pose estimator was created by
    using dlib's implementation of the paper:
       One Millisecond Face Alignment with an Ensemble of Regression Trees by
       Vahid Kazemi and Josephine Sullivan, CVPR 2014
    and was trained on the iBUG 300-W face landmark dataset (see
    https://ibug.doc.ic.ac.uk/resources/facial-point-annotations/):  
       C. Sagonas, E. Antonakos, G, Tzimiropoulos, S. Zafeiriou, M. Pantic. 
       300 faces In-the-wild challenge: Database and results. 
       Image and Vision Computing (IMAVIS), Special Issue on Facial Landmark Localisation "In-The-Wild". 2016.
    You can get the trained model file from:
    http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2.
    Note that the license for the iBUG 300-W dataset excludes commercial use.
    So you should contact Imperial College London to find out if it's OK for
    you to use this model file in a commercial product.


    Also, note that you can train your own models using dlib's machine learning
    tools.  See train_shape_predictor_ex.cpp to see an example.

    


    Finally, note that the face detector is fastest when compiled with at least
    SSE2 instructions enabled.  So if you are using a PC with an Intel or AMD
    chip then you should enable at least SSE2 instructions.  If you are using
    cmake to compile this program you can enable them by using one of the
    following commands when you create the build project:
        cmake path_to_dlib_root/examples -DUSE_SSE2_INSTRUCTIONS=ON
        cmake path_to_dlib_root/examples -DUSE_SSE4_INSTRUCTIONS=ON
        cmake path_to_dlib_root/examples -DUSE_AVX_INSTRUCTIONS=ON
    This will set the appropriate compiler options for GCC, clang, Visual
    Studio, or the Intel compiler.  If you are using another compiler then you
    need to consult your compiler's manual to determine how to enable these
    instructions.  Note that AVX is the fastest but requires a CPU from at least
    2011.  SSE4 is the next fastest and is supported by most current machines.  
*/


#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/image_transforms.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <ctime>
#include <iostream>
#include <fstream>

using namespace dlib;
using namespace std;

template <typename image_type>
void landmark_draw(const image_type& in_img, image_type& out_img, const full_object_detection& shape);

template <typename image_type>
void sticker_mustache(const image_type& in_img, image_type& out_img, const full_object_detection& shape ,int idx);

template <typename image_type>
void sticker_hairband(const image_type& in_img, image_type& out_img, const full_object_detection& shape ,int idx);
// ----------------------------------------------------------------------------------------

int main(int argc, char** argv)
{  
    try
    {
        // This example takes in a shape model file and then a list of images to
        // process.  We will take these filenames in as command line arguments.
        // Dlib comes with example images in the examples/faces folder so give
        // those as arguments to this program.
        if (argc == 1)
        {
            cout << "Call this program like this:" << endl;
            cout << "./face_landmark_detection_ex shape_predictor_68_face_landmarks.dat faces/*.jpg" << endl;
            cout << "\nYou can get the shape_predictor_68_face_landmarks.dat file from:\n";
            cout << "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
            return 0;
        }

        // We need a face detector.  We will use this to get bounding boxes for
        // each face in an image.
        frontal_face_detector detector = get_frontal_face_detector();
        // And we also need a shape_predictor.  This is the tool that will predict face
        // landmark positions given an image and face bounding box.  Here we are just
        // loading the model from the shape_predictor_68_face_landmarks.dat file you gave
        // as a command line argument.
        shape_predictor sp;

        clock_t begin = clock();
        deserialize(argv[1]) >> sp;
        cout << ">>>deserialize: " << (double)(clock() - begin) / CLOCKS_PER_SEC << endl;

        image_window win, win_faces;

        // write position of landmarks
        ofstream outFile("result.txt");
        // Loop over all the images provided on the command line.
        // process only one
        for (int i = 2; i < 3; ++i)
        {
            cout << "processing image " << argv[i] << endl;
            array2d<rgb_pixel> img;
            load_image(img, argv[i]);

            // Make the image larger so we can detect small faces.
            // @@@Modified -> maybe no need, due to type of img is selfie
            // pyramid_up(img);

            // Now tell the face detector to give us a list of bounding boxes
            // around all the faces in the image.
            begin = clock();
            // reference about speed up : https://github.com/davisking/dlib/issues/557
            // https://www.learnopencv.com/speeding-up-dlib-facial-landmark-detector/
            std::vector<rectangle> dets = detector(img);
            cout << ">>>frontal face detector: " << (double)(clock() - begin) / CLOCKS_PER_SEC << endl;
            cout << "Number of faces detected: " << dets.size() << endl;

            // Now we will go ask the shape_predictor to tell us the pose of
            // each face we detected.
            std::vector<full_object_detection> shapes;
            for (unsigned long j = 0; j < dets.size(); ++j)
            {
                begin = clock();
                full_object_detection shape = sp(img, dets[j]);
                cout << ">>>shape predictor: " << (double)(clock() - begin) / CLOCKS_PER_SEC << endl;
                cout << "number of parts: "<< shape.num_parts() << endl;               

                // find landmark
                if(true){
                    array2d<rgb_pixel> img_landmark;
                    landmark_draw(img, img_landmark, shape);
                    for(int j=0;j<shape.num_parts();j++){
                        outFile << j << ": " <<  shape.part(j) << endl;
                    }
                    save_png(img_landmark, "readme.png");
                }

                // mustache filter
                if(true){
                    array2d<rgb_pixel> img_origin;
                    assign_image(img_origin, img);
                    
                    for(int idx=0;idx<4;idx++){
                        sticker_mustache(img_origin, img, shape, idx);

                        save_png(img, "img/result_m" + to_string(idx) + ".png");
                        assign_image(img, img_origin);
                    }
                }

                // hairband filter
                if(true){
                    array2d<rgb_pixel> img_origin;
                    assign_image(img_origin, img);
                    
                    for(int idx=0;idx<2;idx++){
                        sticker_hairband(img_origin, img, shape, idx);

                        save_png(img, "img/result_h" + to_string(idx) + ".png");
                        assign_image(img, img_origin);
                    }
                }

                // You get the idea, you can get all the face part locations if
                // you want them.  Here we just store them in shapes so we can
                // put them on the screen.
                shapes.push_back(shape);
            }
            
            /*
            // Now let's view our face poses on the screen.
            win.clear_overlay();
            win.set_image(img);
            win.add_overlay(render_face_detections(shapes));
            */

            // save_png(img, "result.png");

            // We can also extract copies of each face that are cropped, rotated upright,
            // and scaled to a standard size as shown here:
            /*
            dlib::array<array2d<rgb_pixel> > face_chips;
            extract_image_chips(img, get_face_chip_details(shapes), face_chips);
            win_faces.set_image(tile_images(face_chips));
            */

            cout << "Hit enter to process the next image..." << endl;
            cin.get();
        }

        outFile.close();
    }
    catch (exception& e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}

template <typename image_type>
void landmark_draw(const image_type& in_img, image_type& out_img, const full_object_detection& shape){
    assign_image(out_img, in_img);

    for(int j=0;j<shape.num_parts();j++){
        if (j>=0 && j<=16){ // ear to ear
            draw_solid_circle(out_img, shape.part(j), (double)5, rgb_pixel(255,0,0));
        } else if (j>=27 && j<=30){ // line on top of nose
            draw_solid_circle(out_img, shape.part(j), (double)5, rgb_pixel(127,0,0));
        } else if (j>=17 && j<=21){ // left eyebrow
            draw_solid_circle(out_img, shape.part(j), (double)5, rgb_pixel(0,255,0));
        } else if (j>=22 && j<=26){ // right eyebrow
            draw_solid_circle(out_img, shape.part(j), (double)5, rgb_pixel(0,127,0));
        } else if (j>=31 && j<=35){ // bottom part of the nose
            draw_solid_circle(out_img, shape.part(j), (double)5, rgb_pixel(0,0,255));
        } else if (j>=36 && j<=41){ // left eye
            draw_solid_circle(out_img, shape.part(j), (double)5, rgb_pixel(0,0,127));
        } else if (j>=42 && j<=47){ // right eye
            draw_solid_circle(out_img, shape.part(j), (double)5, rgb_pixel(255,255,0));
        } else if (j>=48 && j<=59){ // lips outer part
            draw_solid_circle(out_img, shape.part(j), (double)5, rgb_pixel(0,255,255));
        } else if (j>=60 && j<=67){ // lips inside part
            draw_solid_circle(out_img, shape.part(j), (double)5, rgb_pixel(255,0,255));
        } else {
            draw_solid_circle(out_img, shape.part(j), (double)5, rgb_pixel(0,0,0));
        }
    }

    draw_rectangle(out_img, shape.get_rect(), rgb_pixel(255, 255 ,255));

    int face_width = (int) (shape.get_rect().right() - shape.get_rect().left());
    int face_height = (int) (shape.get_rect().top() - shape.get_rect().bottom());

    cout << "top :" << shape.get_rect().top() << endl;
    cout << "left :" << shape.get_rect().left() << endl;
    cout << "bottom :" << shape.get_rect().bottom() << endl;
    cout << "right :" << shape.get_rect().right() << endl;
}

template <typename image_type>
void sticker_hairband(const image_type& in_img, image_type& out_img, const full_object_detection& shape ,int idx){
    try{
        assign_image(out_img, in_img);

        long h = ((shape.part(21)+shape.part(22)) - shape.part(33))(1);

        // get position of top of forehead
        dlib::vector<long int, 2l> fore_l = dlib::vector<long int, 2l>((shape.part(19))(0), h);

        dlib::vector<long int, 2l> fore_r = dlib::vector<long int, 2l>((shape.part(24))(0), h);
        
        // load hairband image
        string fname = "img/hairband_";
        array2d<rgb_alpha_pixel> st;
        load_image(st, fname + to_string(idx) + "l.png");

        // resize sticker image
        int nw = (int)((float)h/st.nr() * st.nc());
        array2d<rgb_alpha_pixel> stst(h, nw);
        resize_image(st, stst);

        // processing at left side
        int p = (int)fore_l(0);
        int q = (int)fore_l(1);
        int sw = (int)stst.nr()/2;
        int sh = (int)stst.nc()/2;

        for(int x=-sw;x<sw;x++){
            for(int y=-sh;y<sh;y++){
                if(stst[x+sw][y+sh].alpha != 0){
                    out_img[x+q][y+p].red = stst[x+sw][y+sh].red;
                    out_img[x+q][y+p].blue = stst[x+sw][y+sh].blue;
                    out_img[x+q][y+p].green = stst[x+sw][y+sh].green;
                }
            }
        }
        
        // processing at right side
        load_image(st, fname + to_string(idx) + "r.png");
        nw = (int)((float)h/st.nr() * st.nc());
        array2d<rgb_alpha_pixel> ststn(h, nw);
        resize_image(st, ststn);

        p = (int)fore_r(0);
        q = (int)fore_r(1);

        for(int x=-sw;x<sw;x++){
            for(int y=-sh;y<sh;y++){
                if(ststn[x+sw][y+sh].alpha != 0){
                    out_img[x+q][y+p].red = ststn[x+sw][y+sh].red;
                    out_img[x+q][y+p].blue = ststn[x+sw][y+sh].blue;
                    out_img[x+q][y+p].green = ststn[x+sw][y+sh].green;
                }
            }
        }
    }
    catch (exception &e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}

template <typename image_type>
void sticker_mustache(const image_type& in_img, image_type& out_img, const full_object_detection& shape ,int idx){
    try{
        assign_image(out_img, in_img);
        
        string fname = "img/mustache_";
        array2d<rgb_alpha_pixel> st;
        load_image(st, fname + to_string(idx) + ".png");

        // resize sticker image
        long w = (int) (shape.get_rect().right() - shape.get_rect().left());
        int nh = (int)((float)w/st.nc() * st.nr());
        array2d<rgb_alpha_pixel> stst(nh, w);
        resize_image(st, stst);

        int p = (int)(shape.part(30))(0);
        int q = (int)(shape.part(30))(1);
        int sw = (int)stst.nr()/2;
        int sh = (int)stst.nc()/2;

        for(int x=-sw;x<sw;x++){
            for(int y=-sh;y<sh;y++){
                if(stst[x+sw][y+sh].alpha != 0){
                    out_img[x+q][y+p].red = stst[x+sw][y+sh].red;
                    out_img[x+q][y+p].blue = stst[x+sw][y+sh].blue;
                    out_img[x+q][y+p].green = stst[x+sw][y+sh].green;
                }
            }
        }
    }
    catch (exception &e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}

// ----------------------------------------------------------------------------------------

