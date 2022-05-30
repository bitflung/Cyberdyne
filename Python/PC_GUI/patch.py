import numpy as np
from PIL import Image

#returns image or patch as numpy array
def load_image(patch_file_path):
    #print("Loading patch file " + patch_file_path)
    im = Image.open(patch_file_path)
    rgb_im = im.convert('RGB')
    pilarr = np.array(rgb_im)
    pilarr=np.moveaxis(pilarr,2,0)
    imheight = np.shape(pilarr)[1]
    imwidth = np.shape(pilarr)[2]
    padding = np.zeros((imheight*imwidth))
    imgflat = np.ravel(pilarr)
    imgpadded = np.append(imgflat,padding)
    imgformated = np.reshape(imgpadded,(imheight,imwidth,4))
    #swap red and blue
    imgdata_tmp= np.copy(imgformated)
    imgformated[:,:,0] = imgformated[:,:,2]
    imgformated[:,:,2] = imgdata_tmp[:,:,0]
    
    #to -128 to +127
    patch = pilarr - 128

    #debug #save_patch(patch,patch_file_path+"debug.png")
    return patch