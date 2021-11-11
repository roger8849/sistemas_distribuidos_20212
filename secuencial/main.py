import imageio
import skimage
import numpy as np

filename = 'input/futbol.mp4'
reader = imageio.get_reader(filename, 'ffmpeg')

for i, im in enumerate(reader):
    image = skimage.img_as_float(im).astype(np.float64)
    imageio.imsave("base/"+str(i) + '.jpg', image)
