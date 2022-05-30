import numpy as np
from PIL import Image


#returns image or patch as numpy array
def load_image(patch_file_path):
    #print("Loading patch file " + patch_file_path)
    im = Image.open(patch_file_path)
    rgb_im = im.convert('RGB')
    pilarr = np.array(rgb_im)
    imheight = np.shape(pilarr)[0]
    imwidth = np.shape(pilarr)[1]
    #pilarr=np.moveaxis(pilarr,2,0)
    imgformatted = np.zeros((imheight,imwidth,4),dtype=np.int8)
    imgformatted[:,:,0] = pilarr[:,:,2]
    imgformatted[:,:,1] = pilarr[:,:,1]
    imgformatted[:,:,2] = pilarr[:,:,0]
    
    # padding = np.zeros((imheight*imwidth))
    #imgflat = np.ravel(pilarr)
    
    # imgpadded = np.concatenate((pilarr,padding),axis=2)
    # imgformated = np.reshape(imgpadded,(imheight,imwidth,4)).astype(np.int8)
    #swap red and blue
    
    # imgdata_tmp= np.copy(imgformated)
    # imgformated[:,:,0] = imgformated[:,:,2]
    # imgformated[:,:,2] = imgdata_tmp[:,:,0]


    #debug #save_patch(patch,patch_file_path+"debug.png")
    return imgformatted