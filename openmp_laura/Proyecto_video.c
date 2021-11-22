//
// Video processing example using FFmpeg
// Written by Ted Burke - last updated 12-2-2017
//

#include <stdio.h>
#include <string.h>
#include <omp.h>

// Video resolution
#define W 854
#define H 480
#define FILENAME "futbol.mp4"

// Allocate a buffer to store one frame
unsigned char frame[H][W][3] = {0};
char message_in[200];
char message_out[200];
char time[200];

void main()
{
    int x, y, count, avg_rgb;

    sprintf(message_in, "%s%s%s", "ffmpeg -i ", FILENAME, " -f image2pipe -vcodec rawvideo -pix_fmt rgb24 -");
    sprintf(message_out, "%s%d%s%d%s", "ffmpeg -y -f rawvideo -vcodec rawvideo -pix_fmt rgb24 -s ", W, "x", H, " -r 25 -i - -f mp4 -q:v 5 -an -vcodec mpeg4 output.mp4" );

    // Open an input pipe from ffmpeg and an output pipe to a second instance of ffmpeg
    FILE *pipein = popen(message_in, "r");
    FILE *pipeout = popen(message_out, "w");

    double start_time = omp_get_wtime();

    //fseek(pipein, H*W*3, SEEK_END);
    // Process video frames
    while(1)
    {
        // Read a frame from the input pipe into the buffer
        count = fread(frame, 1, H*W*3, pipein);

        // If we didn't get a frame of video, we're probably at the end
        if (count != H*W*3) break;


        // Process this frame
        #pragma omp paralell num_threads(32)
        {
            int ID = omp_get_thread_num();
            int num_threads = omp_get_num_threads();
            int start = ID * (H/num_threads);
            int end = start + (H/num_threads);
            for (y=start ; y<end ; ++y) for (x=0 ; x<W ; ++x)
            {
                avg_rgb = (frame[y][x][0] + frame[y][x][1] + frame[y][x][2]) / 3;

                frame[y][x][0] = avg_rgb; // red
                frame[y][x][1] = avg_rgb; // green
                frame[y][x][2] = avg_rgb; // blue
            }
        }

        // Write this frame to the output pipe
        fwrite(frame, 1, H*W*3, pipeout);
    }

    double end_time = omp_get_wtime() - start_time;

    sprintf(time, "%s%f", "Tiempo de ejecucion: ", end_time);

    puts(time);
    // Flush and close input and output pipes
    fflush(pipein);
    pclose(pipein);
    fflush(pipeout);
    pclose(pipeout);
}
