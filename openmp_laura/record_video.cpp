#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "opencv2/opencv.hpp" //Include OpenCV header file
//#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

string int2string(int number){ //Function to convert an integer number to a string
  std::stringstream ss;
  ss << number;
  return ss.str();
}

int main() //char *argv[]
{
  string source, filename;
  bool finish = false;
  int key = 0;


  Mat frame;
  Mat frameg;
  Mat frame2g;
  int y;

  source = "futbol.mp4";
  filename = "output.mp4";

  VideoCapture cap(source); //Create a VideoCapture object
  VideoWriter writer; //Create a VideoWriter object, not initialized yet

  //Check if videocapture opened successfully
  if(!cap.isOpened())
  {
    cout << "Could not open the video capture: " << source << endl;
    return -1;
  }

  //Getting the video properties
  int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
  Size frame_size(frame_width, frame_height);
  double fps = cap.get(CAP_PROP_FPS); //after open the capture obj
  int total_frames = cap.get(cv::CAP_PROP_FRAME_COUNT);

  cout << "Source info:\n Size:" << frame_size << endl;
  cout << " Frames per seconds:" << fps << endl;
  cout << " Total frames: " << total_frames << endl;

  //Define the output video fps
  int FPS = 60; //Frames per second
  double fontScale = 2; //Size of the font

  //Define the video codec by FOURCC, recoding method, fourcc integer
  int fcc = cv::VideoWriter::fourcc('A','V','C','1');
  //'X','V','I','D' open source codec
  //'M','J','P','G' Motion JPEG video
  //'X','2','6','4' open source H.264 implementation (compressed)

  //Initialize the VideoWriter object
  writer = VideoWriter(filename,fcc,FPS,frame_size,true);


  if(!writer.isOpened()){
    cout << "Could not open the video writer" << endl;
    return -1;	//Exit
  }

  //Creating the video window
  //string window_name = "Video window: "+source;
  //namedWindow(window_name, WINDOW_NORMAL);

  double start_time = omp_get_wtime();

  //while(!finish) //while finish == false
  //{
    cout << "Starting video" << endl;

    cap.set(cv::CAP_PROP_POS_FRAMES, 0); //Set the initial videocapture position on 0
    #pragma omp paralell num_threads(4)
    {
        int ID = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        int start = ID * (total_frames/num_threads);
        int end_h = start + (total_frames/num_threads);
        for (y=start ; y<end_h ; ++y)

    //while(cap.get(cv::CAP_PROP_POS_FRAMES) <= total_frames-1)//Compare if the video has not finished
        {

      //Capture frame-by-frame
            cap.read(frame); //cap >> frame;

      //If the frame is empty, break immediately
            if(frame.empty())
            {
                cout << "Error: frame empty" << endl;
                y=end_h+1;
            }

      //Getting the frame position
            int frame_pos = cap.get(cv::CAP_PROP_POS_FRAMES);
            string pos_frame = int2string(frame_pos);

            int width2 = int(frame_width * 50 / 100);
            int height2 = int(frame_height * 50 / 100);

            cv::resize(frame,frameg,Size(width2, height2));

            cvtColor(frameg,frame2g,cv::COLOR_BGR2GRAY);

      //double time_seg = cap.get(CV_CAP_PROP_POS_AVI_RATIO)*total_frames/fps;
            double time_seg = (double)(frame_pos/fps);
      //cout << "time [s]: " << time_seg << endl;

      //Getting the time
            int time_s = (int)(time_seg); //Truncate the time
            string t_s = int2string(time_s);
      //putText(frame,"time [s] = "+t_s, Point(10,150),2,fontScale,Scalar(0,0,0),1.5,1);


      //Write the frame into the filename
            writer.write(frame); //writer << frame;
     //}
      //putText(frame,"REC",Point(10,400),2,4*fontScale,Scalar(0,0,255),4,1);

            //imshow( window_name, frame2g ); //Display the resulting frame

    //     key = waitKey(8);
      // Press ESC on keyboard to exit

      }
    }

    double end_time = omp_get_wtime() - start_time;

    cout << "Tiempo de ejecucion: " << end_time<< endl;


      //if(key == 27)
      //{
      //    finish = true; //This will break the first while
      //    break; //Break this loop (comparison of the frames)
      //}

    //finish = true;
    //break;
  //}

  //}

  //Before going, release the video capture and write object
  cap.release();
  writer.release();

  //imshow(window_name,writer.);

  return 0;
}
