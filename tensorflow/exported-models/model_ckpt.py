import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'    # Suppress TensorFlow logging (1)
import pathlib
import tensorflow as tf

tf.get_logger().setLevel('ERROR')           # Suppress TensorFlow logging (2)

gpus = tf.config.experimental.list_physical_devices('GPU')
for gpu in gpus:
    tf.config.experimental.set_memory_growth(gpu, True)

import time
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

end_time = time.time()
elapsed_time = end_time - start_time
print('Done! Took {} seconds'.format(elapsed_time))


# tf.saved_model.save(detection_model, MODEL_SAVE_PATH)

for count, image_path in enumerate(IMAGE_PATHS):
    # start = time.process_time()
    start = time.time()
    print('Running inference for {}... '.format(image_path), end='')

    image_np = load_image_into_numpy_array(image_path)
    input_tensor = tf.convert_to_tensor(np.expand_dims(image_np, 0), dtype=tf.float32)

    detections = detect_fn(input_tensor)
    print(time.time() - start)
    # All outputs are batches tensors.
    # Convert to numpy arrays, and take index [0] to remove the batch dimension.
    # We're only interested in the first num_detections.
    num_detections = int(detections.pop('num_detections'))
    detections = {key: value[0, :num_detections].numpy() for key, value in detections.items()}
    detections['num_detections'] = num_detections

    # detection_classes should be ints.
    detections['detection_classes'] = detections['detection_classes'].astype(np.int64)

    label_id_offset = 1
    image_np_with_detections = image_np.copy()

    viz_utils.visualize_boxes_and_labels_on_image_array(
        image_np_with_detections,
        detections['detection_boxes'],
        detections['detection_classes']+label_id_offset,
        detections['detection_scores'],
        category_index,
        use_normalized_coordinates=True,
        max_boxes_to_draw=200,
        min_score_thresh=.30,
        agnostic_mode=False)

    plt.figure()
    plt.imshow(image_np_with_detections)

    print('Done')
    # print(time.process_time() - start)
    savefilename = "./exported-models/results/results" + str(count)
    plt.savefig("./" + savefilename + ".png")
plt.show()

# sphinx_gallery_thumbnail_number = 2