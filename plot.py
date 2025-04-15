import os
import glob
import re
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm
import argparse

suffix = {
    'K': 2**10,
    'M': 2**20,
    'G': 2**30,
    'T': 2**40,
}

parser = argparse.ArgumentParser()
parser.add_argument('--path', type=str, default='./dataset')
args = parser.parse_args()
dataset_dir = args.path

plot_dir = os.path.join(dataset_dir, 'plot')
os.makedirs(plot_dir, exist_ok=True)
file_list = glob.glob(os.path.join(dataset_dir, '*'))

def output(f):
    return os.path.join(plot_dir, f'{f}.png')

targets = []

for filepath in file_list:
    filename = os.path.basename(filepath)
    
    if '.' not in filename:
        continue
    name_part, ext = filename.split('.', 1)

    if ext not in ['sorted', 'unsorted']:
        continue

    match = re.match(r'int(\d+)_(\d+[KMGT]?)_([^_]+)_([^_]+)_([^\.]+)', name_part)
    if not match:
        continue

    output_path = output(filename)
    if os.path.exists(output_path):
        continue

    targets.append({
        'filepath': filepath,
        'filename': filename,
        'name_part': name_part,
        'ext': ext,
        'match': match
    })

for info in tqdm(targets, desc='Generating plots'):
    filepath = info['filepath']
    filename = info['filename']
    name_part = info['name_part']
    ext = info['ext']
    match = info['match']

    nbits_str, size_str, dist, pattern, _ = match.groups()
    nbits = int(nbits_str)
    if size_str.isnumeric():
        size = int(size_str)
    else:
        size_unit = size_str[-1]
        size_number = int(size_str[:-1])
        size = size_number * suffix.get(size_unit, 1)
    byte_size = nbits // 8
    dtype = np.dtype(f'<u{byte_size}')  # little endian

    with open(filepath, 'rb') as f:
        data = np.frombuffer(f.read(size * byte_size), dtype=dtype)

    x = np.arange(size)
    y = data

    plt.figure(figsize=(8, 6))
    plt.scatter(x, y, s=1, c='black')
    plt.xlim(0, size)
    plt.ylim(0, 2**nbits)
    plt.xlabel('Index', fontsize=15)
    plt.ylabel('Data', fontsize=15)
    plt.title(f'{dist}_{pattern} ({ext})', fontsize=17, pad=15)
    plt.savefig(output(filename), dpi=600)
    plt.close()
