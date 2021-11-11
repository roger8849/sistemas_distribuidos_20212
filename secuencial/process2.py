import os
import cv2
import glob
import imageio
import skimage
import numpy as np
from tqdm import tqdm

from decorators import profile

VIDEO = "futbol.mp4"
SCALE_PERCENT = 50
PATH_ORIGIN_FRAMES = "video/base"
BASE_PATH = "video"
FRAME_FORMAT = ".jpg"
ABSOLUTE_PATH = os.path.abspath("video/")


def extract_frames():
    print("==== 1. ===")
    print("[STAR] EXTRACT FRAMES")
    reader = imageio.get_reader(VIDEO, 'ffmpeg')
    for i, im in tqdm(enumerate(reader)):
        image = skimage.img_as_float(im).astype(np.float64)
        imageio.imsave(f"{BASE_PATH}/base/{str(i)}{FRAME_FORMAT}", image)
    print("[DONE] EXTRACT FRAMES")


def numpy_array_frames():
    print("==== 2. ===")
    print("[STAR] GET NUMPY ARRAY FROM FRAMES")
    frames = [
        cv2.imread(file) for file in glob.glob(f"{PATH_ORIGIN_FRAMES}/*.jpg")
    ]
    print(
        "Frame 0: ", type(frames[0]),
        "Frame List: ", type(frames),
        "Total Frames: ", len(frames)
    )
    print("[DONE] GET NUMPY ARRAY FROM FRAMES")
    return frames


def scale_frames(frames):
    print("==== 3. ===")
    print("[STAR] RESIZING FRAMES")
    folder = "scale"
    for frame in tqdm(range(frames)):
        src = cv2.imread(f"{PATH_ORIGIN_FRAMES}/{frame}.jpg", cv2.IMREAD_UNCHANGED)
        scale_percent = SCALE_PERCENT
        width = int(src.shape[1] * scale_percent / 100)
        height = int(src.shape[0] * scale_percent / 100)
        dsize = (width, height)
        output = cv2.resize(src, dsize)
        cv2.imwrite(f"{BASE_PATH}/{folder}/{frame}.jpg", output)
    print("[DONE] RESIZING FRAMES")

def transform_grayscale_frame(frames):
    print("==== 4. ===")
    print("[STAR] APPLY FILTER GRAYSCALE TO FRAMES")
    folder = "gray"
    for frame in tqdm(range(frames)):
        src = cv2.imread(f"{BASE_PATH}/scale/{frame}.jpg", cv2.IMREAD_UNCHANGED)
        output = cv2.cvtColor(src, cv2.COLOR_BGR2GRAY)
        cv2.imwrite(f"{BASE_PATH}/{folder}/{frame}.jpg", output)
    print("[DONE] APPLY FILTER GRAYSCALE TO FRAMES")


def convertToVideo():
    print("==== 5. ===")
    print("[STAR] BUILD VIDEO...")
    out_video = "out.mp4"
    framerate_video = "25"
    os.system(f"cd {ABSOLUTE_PATH}/gray && ffmpeg -framerate {framerate_video} -pattern_type glob -i '*{FRAME_FORMAT}' -pix_fmt yuv420p -qp 0 {ABSOLUTE_PATH}/{out_video} -y")
    print("[DONE] BUILD VIDEO")

@profile(sort_by='cumulative', lines_to_show=10, strip_dirs=True)
def init_process():
    extract_frames()
    frames = len(numpy_array_frames())
    scale_frames(frames)
    transform_grayscale_frame(frames)
    convertToVideo()

if __name__ == '__main__':
    init_process()
