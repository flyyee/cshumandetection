import os
import glob
import pandas as pd
import io
import xml.etree.ElementTree as ET

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'    # Suppress TensorFlow logging (1)
import tensorflow.compat.v1 as tf
from PIL import Image
from object_detection.utils import dataset_util, label_map_util
from collections import namedtuple

import csv

xml_dir = "C:\\Users\\Gerrard Tai\\Documents\\code\\cshumandetection\\ImageGeneration\\ImageGeneration\\imgs_12-30_3-21pm"
labels_path = "C:\\Users\\Gerrard Tai\\Documents\\code\\cshumandetection\\ImageGeneration\\ImageGeneration\\annotations\\label_map.pbtxt"
output_path = "C:\\Users\\Gerrard Tai\\Documents\\code\\cshumandetection\\ImageGeneration\\ImageGeneration\\annotations\\train.record"

image_dir = xml_dir

label_map = label_map_util.load_labelmap(labels_path)
label_map_dict = label_map_util.get_label_map_dict(label_map)

def xml_to_csv(path):
    """Iterates through all .xml files (generated by labelImg) in a given directory and combines
    them in a single Pandas dataframe.

    Parameters:
    ----------
    path : str
        The path containing the .xml files
    Returns
    -------
    Pandas DataFrame
        The produced dataframe
    """

    xml_list = []
    for xml_file in glob.glob(path + '/*.xml'):
        tree = ET.parse(xml_file)
        root = tree.getroot()
        for member in root.findall('object'):
            value = (root.find('filename').text,
                     int(root.find('size')[0].text),
                     int(root.find('size')[1].text),
                     member[0].text,
                     int(member[4][0].text),
                     int(member[4][1].text),
                     int(member[4][2].text),
                     int(member[4][3].text)
                     )
            xml_list.append(value)
    column_name = ['filename', 'width', 'height',
                   'class', 'xmin', 'ymin', 'xmax', 'ymax']
    xml_df = pd.DataFrame(xml_list, columns=column_name)
    return xml_df


def xml_to_csv2(path):
    xml_list = []
    for xml_file in glob.glob(path + '/*.xml'):
        tree = ET.parse(xml_file)
        root = tree.getroot()
        for member in root.findall('object'):
            value = (root.find('filename').text,  # filename
                     int(root.find('size')[0].text),  # image width
                     int(root.find('size')[1].text),  # image height
                     member[0].text,  # class
                     int(member[4][0].text),  # xmin
                     int(member[4][1].text),  # ymin
                     int(member[4][2].text),  # xmax
                     int(member[4][3].text)  # ymax
                     )
            xml_list.append(value)
    column_name = ['filename', 'width', 'height',
                   'class', 'xmin', 'ymin', 'xmax', 'ymax']
    xml_df = pd.DataFrame(xml_list, columns=column_name)
    return xml_df

def xml_to_csv3(path, offset):
    IMGWIDTH = 800
    IMGHEIGHT = 600
    IMGCLASS = "Bot"
    with open(path, newline='') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
        xml_list = []
        for count, row in enumerate(spamreader):
            value = (str(count + offset) + ".jpg",  # filename
                     IMGWIDTH,  # image width
                     IMGHEIGHT,  # image height
                     IMGCLASS,  # class
                     int(float(row[0])),  # xmin
                     int(float(row[1])),  # ymin
                     int(float(row[2])),  # xmax
                     int(float(row[3]))  # ymax
                     )
            if int(float(row[0])) == 0 or int(float(row[1])) == 0 or int(float(row[2])) == 0 or int(float(row[3])) == 0:
                continue
            if int(float(row[0])) > 800 or int(float(row[1])) > 600 or int(float(row[2])) < 0 or int(float(row[3])) < 0:
                continue
            xml_list.append(value)
    column_name = ['filename', 'width', 'height',
                   'class', 'xmin', 'ymin', 'xmax', 'ymax']
    xml_df = pd.DataFrame(xml_list, columns=column_name)
    return xml_df


def class_text_to_int(row_label):
    return label_map_dict[row_label]


def split(df, group):
    data = namedtuple('data', ['filename', 'object'])
    gb = df.groupby(group)
    return [data(filename, gb.get_group(x)) for filename, x in zip(gb.groups.keys(), gb.groups)]


def create_tf_example(group, path):
    with tf.gfile.GFile(os.path.join(path, '{}'.format(group.filename)), 'rb') as fid:
        encoded_jpg = fid.read()
    encoded_jpg_io = io.BytesIO(encoded_jpg)
    image = Image.open(encoded_jpg_io)
    width, height = image.size

    filename = group.filename.encode('utf8')
    image_format = b'jpg'
    xmins = []
    xmaxs = []
    ymins = []
    ymaxs = []
    classes_text = []
    classes = []

    for index, row in group.object.iterrows():
        xmins.append(row['xmin'] / width)
        xmaxs.append(row['xmax'] / width)
        ymins.append(row['ymin'] / height)
        ymaxs.append(row['ymax'] / height)
        classes_text.append(row['class'].encode('utf8'))
        classes.append(class_text_to_int(row['class']))

    tf_example = tf.train.Example(features=tf.train.Features(feature={
        'image/height': dataset_util.int64_feature(height),
        'image/width': dataset_util.int64_feature(width),
        'image/filename': dataset_util.bytes_feature(filename),
        'image/source_id': dataset_util.bytes_feature(filename),
        'image/encoded': dataset_util.bytes_feature(encoded_jpg),
        'image/format': dataset_util.bytes_feature(image_format),
        'image/object/bbox/xmin': dataset_util.float_list_feature(xmins),
        'image/object/bbox/xmax': dataset_util.float_list_feature(xmaxs),
        'image/object/bbox/ymin': dataset_util.float_list_feature(ymins),
        'image/object/bbox/ymax': dataset_util.float_list_feature(ymaxs),
        'image/object/class/text': dataset_util.bytes_list_feature(classes_text),
        'image/object/class/label': dataset_util.int64_list_feature(classes),
    }))
    return tf_example


writer = tf.python_io.TFRecordWriter(output_path)
path = os.path.join(image_dir)

for chunkcount in range(1,5):
# for chunkcount in range(1):
    # TODO: had to manually remove empty new line at the end of each bb txt file. Check if necessary.
    chunksize = 5000 # TODO: chunksize not always 5000. Last files have smaller size.
    bb_path = "C:\\Users\\Gerrard Tai\\Documents\\code\\cshumandetection\\ImageGeneration\\ImageGeneration\\boundingboxes_12-30_3-21pm\\"
    examples = xml_to_csv3(bb_path + str(chunkcount + 1) + ".txt", int(chunksize * chunkcount))
    # print(examples)
    grouped = split(examples, 'filename')
    for group in grouped:
        # print(group)
        # print("hello")
        tf_example = create_tf_example(group, path)
        writer.write(tf_example.SerializeToString())
        # os.system("pause")
writer.close()
print('Successfully created the TFRecord file: {}'.format(output_path))