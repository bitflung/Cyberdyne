import numpy as np
from PIL import Image

def old_mask_generation(mask_type='rectangle', patch=None, image_size=(128,128,4)):
    applied_patch = np.zeros(image_size)
    if mask_type == 'rectangle':
        # patch rotation
#         rotation_angle = np.random.choice(4)
#         for i in range(patch.shape[0]):
#             patch[i] = np.rot90(patch[i], rotation_angle)  # The actual rotation angle is rotation_angle * 90
        # patch location
        x_location, y_location = np.random.randint(low=0, high=image_size[1]-patch.shape[1]), np.random.randint(low=0, high=image_size[2]-patch.shape[2])
        for i in range(patch.shape[2]):
            applied_patch[ x_location:x_location + patch.shape[1], y_location:y_location + patch.shape[2],:] = patch
    mask = applied_patch.copy()
    mask[mask != 0] = 1.0
    return applied_patch, mask, x_location, y_location

def mask_generation(mask_type='rectangle', patch=None, image_size=(3, 224, 224)):
    applied_patch = np.zeros(image_size)
    patch=np.moveaxis(patch,2,0)
    if mask_type == 'rectangle':
        # patch rotation
        rotation_angle = np.random.choice(4)
        for i in range(patch.shape[0]):
            patch[i] = np.rot90(patch[i], rotation_angle)  # The actual rotation angle is rotation_angle * 90
        # patch location
        x_location, y_location = (0,0)#np.random.randint(low=0, high=image_size[1]-patch.shape[1]), np.random.randint(low=0, high=image_size[2]-patch.shape[2])
        for i in range(patch.shape[0]):
            applied_patch[:, x_location:x_location + patch.shape[1], y_location:y_location + patch.shape[2]] = patch
    mask = applied_patch.copy()
    mask[mask != 0] = 1.0
    applied_patch=np.moveaxis(applied_patch,0,2)
    mask=np.moveaxis(mask,0,2)
    return applied_patch, mask, x_location, y_location

def hack(imgpath, patchpath):
    nativeimg = Image.open(imgpath, 'r')
    patchimg = Image.open(patchpath, 'r')
    print("patching")
    nw,nh = nativeimg.size
    pw,ph = patchimg.size

    # todo: pick random valid offset values
    offset = 0,0#((nw - pw) // 2, (nw - pw) // 2)

    nativeimg.paste(patchimg, offset)
    nativeimg.save('tmp_patched.png')

def insert_patch(mask,applied_patch,image):
    perturbated_image = np.mul(mask.astype(np.uint8), applied_patch.astype(np.uint8)) + np.mul((1-mask.astype(np.uint8)), image.astype(np.uint8))
    return perturbated_image

def make_patched_img(img_file,patch_file):
    im = Image.open(img_file)
    rgb_im = im.convert('RGB')
    imgarr = np.array(rgb_im,dtype=np.uint8)
    patchimg = Image.open(patch_file)
    patched_rgb = im.convert('RGB')
    patcharr = np.array(patched_rgb,dtype=np.uint8)
    applied_patch, mask, x_location, y_location = mask_generation('rectangle', patcharr, (128,128,4))
    patched_image = insert_patch(mask, applied_patch, imgarr)
    return patched_image


#returns image or patch as numpy array
def load_image(patch_file_path):
    #print("Loading patch file " + patch_file_path)
    im = Image.open(patch_file_path)
    rgb_im = im.convert('RGB')
    pilarr = np.array(rgb_im,dtype=np.uint8)
    #pdb.set_trace()
    imheight = np.shape(pilarr)[0]
    imwidth = np.shape(pilarr)[1]
    imgconvert = pilarr.astype(np.int16)
    imgconvert -= 128
    #pilarr=np.moveaxis(pilarr,2,0)
    imgformatted = np.zeros((imheight,imwidth,4),dtype=np.int16)
#     imgformatted[:,:,0] = imgconvert[:,:,2]
#     imgformatted[:,:,1] = imgconvert[:,:,1]
#     imgformatted[:,:,2] = imgconvert[:,:,0]
    
    imgformatted[:,:,1] = imgconvert[:,:,2]
    imgformatted[:,:,2] = imgconvert[:,:,1]
    imgformatted[:,:,3] = imgconvert[:,:,0]
    
    
    img8bit = imgformatted.astype(np.uint8)
    
    # padding = np.zeros((imheight*imwidth))
    #imgflat = np.ravel(pilarr)
    
    # imgpadded = np.concatenate((pilarr,padding),axis=2)
    # imgformated = np.reshape(imgpadded,(imheight,imwidth,4)).astype(np.int8)
    #swap red and blue
    
    # imgdata_tmp= np.copy(imgformated)
    # imgformated[:,:,0] = imgformated[:,:,2]
    # imgformated[:,:,2] = imgdata_tmp[:,:,0]


    #debug #save_patch(patch,patch_file_path+"debug.png")
    return img8bit