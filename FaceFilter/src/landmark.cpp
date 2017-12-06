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

#include "main.h"
#include "view.h"
#include "landmark.h"

//#include <dlib/image_processing/frontal_face_detector.h>
//#include <dlib/image_processing/render_face_detections.h>
//#include <dlib/image_processing.h>
//#include <dlib/image_transforms.h>
//#include <dlib/image_io.h>
#include <ctime>
//#include <fstream>

using namespace dlib;
using namespace std;

// ----------------------------------------------------------------------------------------

std::vector<full_object_detection> face_landmark(camera_preview_data_s* frame, shape_predictor sp, int sticker, camera_detected_face_s *faces, int count)
{  
    try
    {
        // We need a face detector.  We will use this to get bounding boxes for
        // each face in an image.

        //frontal_face_detector detector = get_frontal_face_detector();
    	// We will use camera_start_face_detection api.

        // And we also need a shape_predictor.  This is the tool that will predict face
        // landmark positions given an image and face bounding box.  Here we are just
        // loading the model from the shape_predictor_68_face_landmarks.dat file you gave
        // as a command line argument.
        dlog_print(DLOG_DEBUG, LOG_TAG, "processing image");

        // Now tell the face detector to give us a list of bounding boxes
        // around all the faces in the image.

        /* TODO: camera_detected_face_s faces -> vector<rectangle> faces */
        //std::vector<rectangle> faces = detector(img);
        std::vector<rectangle> v_faces;
        for(int i = 0; i < count; i++)
        {
        	rectangle face;
			face.set_top(faces[i].y);
			face.set_bottom(faces[i].y + faces[i].height);
			face.set_right(faces[i].x + faces[i].width);
			face.set_left(faces[i].x);
			v_faces.push_back(face);
        }



        double_t begin = clock();
        array2d<u_int64_t> img;
        img.set_size(frame->height, frame->width);
        if(frame->data.double_plane.y_size != frame->width * frame->height)
        {
        	//DLOG_PRINT_DEBUG("y_size: %d, width: %d, height: %d", frame->data.double_plane.y_size, frame->width, frame->height);
        	//return NULL;
        }

        for(u_int64_t i = 0; i < frame->data.double_plane.y_size; i++)
        {
        	img[i/frame->width][i%frame->width] = (frame->data.double_plane.y)[i];
        }
        double_t time = (double_t)(clock() - begin) / CLOCKS_PER_SEC; // TM1: 0.411 sec

        // Now we will go ask the shape_predictor to tell us the pose of
        // each face we detected.
        std::vector<full_object_detection> shapes;
        for (unsigned long i = 0; i < count; ++i)
        {
        	begin = clock();
        	full_object_detection shape = sp(img, v_faces[i]);
        	time = (double_t)(clock() - begin) / CLOCKS_PER_SEC; // TM1: 0.1 sec

        	{
        		dlog_print(DLOG_DEBUG, LOG_TAG, "shape predictor: %f", (double)(clock() - begin) / CLOCKS_PER_SEC);
        		dlog_print(DLOG_DEBUG, LOG_TAG, "number of parts: %d", shape.num_parts());
        	}
        	int np = shape.num_parts();
        	int part33_r = shape.part(33)(0);
        	int part33_c = shape.part(33)(1);
        	int part8_r = shape.part(8)(0);
        	int part8_c = shape.part(8)(1);

        	int part0_r = shape.part(0)(0);
        	int part0_c = shape.part(0)(1);

        	//draw_landmark(frame, shape);
        	/*
        	switch(sticker) {
        	case 1:
        		sticker_mustache(shape);
        		break;
        	case 2:
        		sticker_hairband(shape);
        		break;
        	case 3:
        		sticker_ear(shape);
        		break;
        	case 4:
        		sticker_hat(shape);
        		break;
        	case 5:
        		sticker_glasses(shape);
        		break;
        	default:
        		break;
        	}
*/
        	// You get the idea, you can get all the face part locations if
            // you want them.  Here we just store them in shapes so we can
            // put them on the screen.
            shapes[i] = shape;
        }
            
        // We can also extract copies of each face that are cropped, rotated upright,
            // and scaled to a standard size as shown here:
            /*
            dlib::array<array2d<rgb_pixel> > face_chips;
            extract_image_chips(img, get_face_chip_details(shapes), face_chips);
            win_faces.set_image(tile_images(face_chips));
            */

        return shapes;

    }

    catch (exception& e)
    {
    	dlog_print(DLOG_ERROR, LOG_TAG, "\nexception thrown!");
    	dlog_print(DLOG_ERROR, LOG_TAG, e.what());
    }

}

void draw_landmark(camera_preview_data_s* frame, const full_object_detection shape)
{
	for(int i = 0; i < shape.num_parts(); i++)
	{
		int r = shape.part(i)(0);
		int c = shape.part(i)(1);

		frame->data.double_plane.y[c + r*frame->width] *= 2;
	}
}
/*
void sticker_mustache(const full_object_detection shape){
    try{
        assign_image(out_img, in_img);

        array2d<rgb_alpha_pixel> st;
        load_image(st, fname + to_string(idx) + ".png");

        // resize sticker image
        long w = shape.get_rect().right() - shape.get_rect().left();
        int nh = (int)(w/st.nc() * st.nr());
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
    	dlog_print(DLOG_ERROR, LOG_TAG, "\nexception thrown!");
    	dlog_print(DLOG_ERROR, LOG_TAG, e.what());
    }
}

void sticker_hairband(const full_object_detection shape, int idx){
    try{
        assign_image(out_img, in_img);

        long h = ((shape.part(21)+shape.part(22)) - shape.part(33))(1);

        // get position of top of forehead
        dlib::vector<long int, 2l> fore_l = dlib::vector<long int, 2l>((shape.part(19))(0), h);

        dlib::vector<long int, 2l> fore_r = dlib::vector<long int, 2l>((shape.part(24))(0), h);
        
        // load hairband image
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
    	dlog_print(DLOG_ERROR, LOG_TAG, "\nexception thrown!");
    	dlog_print(DLOG_ERROR, LOG_TAG, e.what());
    }
}

void sticker_ear(const full_object_detection shape, int idx)
{
	 try{
	        long h = ((shape.part(21) + shape.part(22)) - shape.part(33))(1);

	        dlib::vector<long int, 2l> fore_l = dlib::vector<long int, 2l>((shape.part(19))(0), h);
	        dlib::vector<long int, 2l> fore_r = dlib::vector<long int, 2l>((shape.part(24))(0), h);

	        int nw = (int)(h/origin_img.nr() * origin_img.nc());
	        resize_img.set_size(h, nw);
	        resize_image(origin_img, resize_img, interpolate_bilinear());

	        int p = (int)fore_l(0);
	        int q = (int)fore_l(1);
	        int sw = (int)resize_img.nr()/2;
	        int sh = (int)resize_img.nc()/2;

	        for(int i = -sw; i < sw; i++) {
	            for(int j = -sh; j < sh; j++) {
	                if(resize_img[i+sw][j+sh].alpha == 0)
	                    continue;
	                cimg[i+q][j+p].red = resize_img[i+sw][j+sh].red;
	                cimg[i+q][j+p].blue = resize_img[i+sw][j+sh].blue;
	                cimg[i+q][j+p].green = resize_img[i+sw][j+sh].green;
	            }
	        }

	        char* new_name;
	        strncpy(new_name, img_name, strlen(img_name));
	        new_name[strlen(new_name) - 5] = 'r';
	        load_image(origin_img, new_name);

	        resize_image(origin_img, resize_img, interpolate_bilinear());
	        p = (int)fore_r(0);
	        q = (int)fore_r(1);

	        for(int i = -sw; i < sw; i++) {
	            for(int j = -sh; j < sh; j++) {
	                if(resize_img[i+sw][j+sh].alpha == 0)
	                    continue;
	                cimg[i+q][j+p].red = resize_img[i+sw][j+sh].red;
	                cimg[i+q][j+p].blue = resize_img[i+sw][j+sh].blue;
	                cimg[i+q][j+p].green = resize_img[i+sw][j+sh].green;
	            }
	        }

	        delete(new_name);
	    }

	    catch (exception &e)
	    {
	    	dlog_print(DLOG_ERROR, LOG_TAG, "\nexception thrown!");
	    	dlog_print(DLOG_ERROR, LOG_TAG, e.what());
	    }

}
void sticker_hat(const full_object_detection shape)
{
	long h =((shape.part(21) + shape.part(22)) - shape.part(33))(1);
	rectangle rect = shape.get_rect();
	long width = rect.right() - rect.left();
	long height = rect.bottom() - rect.top();

	dlib::vector<long int, 21> fore = dlib::vector<long int, 21> ((shape.part(27))(0), h);

	int w = (int)(h/origin_img.nr() * origin_img.nc());
	resize_img.set_size(height, width);
	resize_image(origin_img, resize_img, interpolate_bilinear());

	int p = (int)fore(0);
	int q = (int)fore(1);
	int sw = (int)resize_img.nc()/2;
	int sh = (int)resize_img.nr()/2;

	for(int i = 0; i < resize_img.nc(); i++)
	{
		for(int j = 0; j < resize_img.nr(); j++)
		{
			if(resize_img[i+w][j+h].alpha == 0)
				continue;
			img[i+q-sw][j+p-sh].red = resize_img[i][j].red;
			img[i+q-sw][j+p-sh].blue = resize_img[i][j].blue;
			img[i+q-sw][j+p-sh]].green = resize_img[i][j].green;
		}
	}
}
void sticker_glasses(const full_object_detection shape)
{
	rectangle rect = shape.get_rect();
	long height = (shape.part(41)(1) - shape.part(19)(1))*3; //From left eyebow to bottom of left eye
	long width = rect.right() - rect.left();
	resize_img.set_size(height, width);
	resize_image(origin_img, resize_img, interpolate_bilinear());

	long m_x = shape.part(27)(0);
	long m_y = shape.part(27)(1);

	for(int i = 0; i < resize_img.nr(); i++)
	{
		for(int j = 0; j < resize_img.nc(); j++)
		{
			if(resize_img[i][j].alpha == 0)
				continue;
			img[i - resize_img.nr()/2 + m_y][j - resize_img.nc()/2 + m_x].red = resize_img[i][j].red;
			img[i - resize_img.nr()/2 + m_y][j - resize_img.nc()/2 + m_x].blue = resize_img[i][j].blue;
			img[i - resize_img.nr()/2 + m_y][j - resize_img.nc()/2 + m_x].green = resize_img[i][j].green;
		}
	}
}
*/
// ----------------------------------------------------------------------------------------

