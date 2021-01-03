import win32gui
import pyautogui
import time
pyautogui.FAILSAFE = False

import movemouse

import numpy as np
import cv2
from PIL import ImageGrab
import time

screen = 'Counter-Strike: Global Offensive'
hwnd = win32gui.FindWindow(None, screen)
rect = win32gui.GetWindowRect(hwnd)
zoomscale = 1
offsetx = 3
offsety = 3
brx = rect[2] * zoomscale - offsetx
bry = rect[3] * zoomscale - offsety
width = 800
height = 600
rect = (brx - width, bry - height, brx, bry)
# rect[0] = rect[2] - width
# rect[1] = rect[3] - height

print(rect)
# input("here")

last_time = time.time()


import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'    # Suppress TensorFlow logging (1)
import pathlib
import tensorflow as tf

tf.get_logger().setLevel('ERROR')           # Suppress TensorFlow logging (2)

gpus = tf.config.experimental.list_physical_devices('GPU')
for gpu in gpus:
    tf.config.experimental.set_memory_growth(gpu, True)

from object_detection.utils import label_map_util
from object_detection.utils import config_util
from object_detection.utils import visualization_utils as viz_utils
from object_detection.builders import model_builder

base_dir = "C:\\Users\\Gerrard Tai\\Documents\\code\\TensorFlow\\workspace\\training_demo\\"
IMAGE_PATH = base_dir + "exported-models\\imgs\\"
IMAGE_PATHS = os.listdir(IMAGE_PATH)
IMAGE_PATHS = [(IMAGE_PATH + im) for im in IMAGE_PATHS]
PATH_TO_MODEL_DIR = base_dir + "models\\my_ssd_resnet50_v1_fpn"
# PATH_TO_MODEL_DIR = base_dir + "models\\mobilenetv2"
# PATH_TO_MODEL_DIR = base_dir + "models\\centernetresnetv1"

PATH_TO_LABELS = base_dir + "annotations\\label_map.pbtxt"

PATH_TO_CFG = base_dir + "exported-models\\pipeline.config"
# PATH_TO_CFG = PATH_TO_MODEL_DIR + "\\pipeline.config"

PATH_TO_CKPT = base_dir + "exported-models\\checkpoint"
MODEL_SAVE_PATH = base_dir + "exported-models\\1\\"

print('Loading model... ', end='')
start_time = time.time()

# Load pipeline config and build a detection model
configs = config_util.get_configs_from_pipeline_file(PATH_TO_CFG)
model_config = configs['model']
detection_model = model_builder.build(model_config=model_config, is_training=False)

# Restore checkpoint
ckpt = tf.compat.v2.train.Checkpoint(model=detection_model)
ckpt.restore(os.path.join(PATH_TO_CKPT, 'ckpt-26')).expect_partial()

@tf.function
def detect_fn(image):
    """Detect objects in image."""

    image, shapes = detection_model.preprocess(image)
    prediction_dict = detection_model.predict(image, shapes)
    detections = detection_model.postprocess(prediction_dict, shapes)

    return detections

end_time = time.time()
elapsed_time = end_time - start_time
print('Done! Took {} seconds'.format(elapsed_time))


category_index = label_map_util.create_category_index_from_labelmap(PATH_TO_LABELS, use_display_name=True)

import numpy as np
from PIL import Image
import matplotlib.pyplot as plt
import warnings
warnings.filterwarnings('ignore')   # Suppress Matplotlib warnings

def load_image_into_numpy_array(path):
    """Load an image from file into a numpy array.

    Puts image into numpy array to feed into tensorflow graph.
    Note that by convention we put it into a numpy array with shape
    (height, width, channels), where channels=3 for RGB.

    Args:
        path: the file path to the image

    Returns:
        uint8 numpy array with shape (img_height, img_width, 3)
    """
    return np.array(Image.open(path))

# Run model through a dummy image so that variables are created
image_np = load_image_into_numpy_array(IMAGE_PATHS[0])
input_tensor = tf.convert_to_tensor(np.expand_dims(image_np, 0), dtype=tf.float32)
detections = detect_fn(input_tensor)

# tf.saved_model.save(detection_model, MODEL_SAVE_PATH)

cont = True
xhairX, xhairY = pyautogui.position()

while cont:
    try:
        start = time.time()
        image_np = np.array(ImageGrab.grab(rect))
        # Load np array into ml model
        print("Obtained image in " + str(int((time.time() - start) * 1000)) + "ms")

        start = time.time()
        # print('Running inference')

        # image_np = load_image_into_numpy_array(image_path)
        input_tensor = tf.convert_to_tensor(np.expand_dims(image_np, 0), dtype=tf.float32)

        detections = detect_fn(input_tensor)
        print("Inference complete in " + str(int((time.time() - start) * 1000)) + "ms")

        start = time.time()
        # cleanup next 2 lines
        num_detections = int(detections.pop('num_detections'))
        detections = {key: value[0, :num_detections].numpy() for key, value in detections.items()}
        
        if detections['detection_scores'][0] < .30:
            continue

        detectionCoords = detections['detection_boxes'][0]

        detectionCoords[0] *= 600
        detectionCoords[1] *= 800
        detectionCoords[0], detectionCoords[1] = detectionCoords[1], detectionCoords[0]
        detectionCoords[2] *= 600
        detectionCoords[3] *= 800
        detectionCoords[2], detectionCoords[3] = detectionCoords[3], detectionCoords[2]
        # print(coords)
        # xmin, ymin, xmax, ymax
        
        deltaX = (detectionCoords[0] + detectionCoords[2]) / 2 + rect[0] - xhairX
        deltaY = detectionCoords[1] + 0.4 * (detectionCoords[3] - detectionCoords[1]) + rect[1] - xhairY
        print(deltaX, deltaY)
        movemouse.move(int(deltaX), int(deltaY))

        # currentMouseX, currentMouseY = pyautogui.position()
        
        # screenDeltaX = (detectionCoords[0] + detectionCoords[2]) / 2 + rect[0]
        # # toy = (coords[1] + coords[3]) / 2 + rect[1]
        # screenDeltaY = detectionCoords[1] + 0.4 * (detectionCoords[3] - detectionCoords[1]) + rect[1]

        # # print(tox, toy)

        # sens = 1
        # moveX = (screenDeltaX - currentMouseX) / 6.949098 / 2.2 / sens * 100 * 0.95
        # moveY = (screenDeltaY - currentMouseY) / 5.236522 / 2.2 / sens * 100 * 0.65

        # # print(mox, moy)

        # pyautogui.move(moveX, moveY)
        # pyautogui.click()
        # pyautogui.move(mox/2, moy/2)
        print("Shot complete in " + str(int((time.time() - start) * 1000)) + "ms")

    except Exception as e:
        print("error", e)
    
    # input("here")
    time.sleep(2)