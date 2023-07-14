import argparse
import os
import cv2
import numpy as np
import socket
import time
import json

parser = argparse.ArgumentParser()
parser.add_argument('-f', '--file', required=True)
parser.add_argument('-o', '--out_dir', required=False)
parser.add_argument('-n', type=int, required=False)
parser.add_argument('--batch', nargs='+', type=int)
parser.add_argument('-c', '--count', type=int, default=1)
parser.add_argument('--type', default='brightness', help="Available:\nbrightness\nedge")
parser.add_argument('--json', action='store_true', default=False)
parser.add_argument('--image', action='store_true', default=False)
args = parser.parse_args()

if not os.path.exists(args.out_dir):
    os.mkdir(args.out_dir)

img = cv2.imread(args.file, 0)
if str(args.type).lower() == 'brightness':
    pass
elif str(args.type).lower() == 'edge':
    img = cv2.Canny(img, 100, 200)
else:
    print("No available generator chosen. Available:\nbrightness\nedge")
    exit(1)
img[img < 20] = 0
img = img / img.sum()

img = np.asarray(img)
choices = np.prod(img.shape)
if args.batch:
    start, end, step = args.batch
else:
    start = args.n
    end = args.n
    step = args.n
for n in range(start, end + 1, step):
    pointsets = []
    _i = 0
    while _i < args.count:
        try:
            index = np.random.choice(choices, size=n, p=img.ravel(), replace=False)
            points = np.column_stack(np.unravel_index(index, shape=img.shape))
            sorted_idx = np.lexsort((points[:, 1], points[:, 0]))
            points = points[sorted_idx]
            # if any([np.array_equal(existing, points) for existing in pointsets]):
            #    print("duplicate point set")
            #    pass
            # else:
            pointsets.append(points)
            _i += 1
        except ValueError:
            print("Not enough distinct pixels available. Choose a smaller size or another type.")
            exit(1)

    fn = os.path.splitext(os.path.basename(args.file))[0] + "_" + str(n)

    for i, points in enumerate(pointsets):
        instance_fn = fn + "_" + str(i)
        if args.image:
            out_img = np.full(shape=img.shape, fill_value=255)
            for p in points:
                out_img[p[0], p[1]] = 0
            cv2.imwrite(os.path.join(args.out_dir, instance_fn + ".png"), out_img)
        if args.json:
            js = {
                'host': socket.gethostname(),
                'id': instance_fn,
                'points': [{"x": int(p[0]), "y": int(p[1])} for p in points],
                'size': n,
                'time': round(time.time() * 1000),
                'type': 'image_' + args.type
            }
            instance_fn += '.json'
            fw = open(os.path.join(args.out_dir, instance_fn), 'w')
            json.dump(js, fw, indent=4)
            fw.close()
            print(instance_fn)
        else:
            instance_fn += ".instance"
            fw = open(os.path.join(args.out_dir, instance_fn), 'w')
            for p in points:
                fw.write('{} {}\n'.format(p[0], p[1]))
            fw.close()
