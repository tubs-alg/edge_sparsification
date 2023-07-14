import argparse
import os
import subprocess
from tqdm import tqdm

parser = argparse.ArgumentParser()

parser.add_argument('-d', '--dir', required=True)
parser.add_argument('-o', '--out-dir', required=True)
parser.add_argument('-e', '--executable', required=True)
parser.add_argument('-a', '--algorithm', required=True)
parser.add_argument('-k', required=False)
parser.add_argument('-c', required=False)
parser.add_argument('-t', required=False)
parser.add_argument('-v', required=False)
parser.add_argument('--sep', required=False)
parser.add_argument('-r', '--regEx', required=False)
parser.add_argument('--sizes', nargs='+', type=int)

args = parser.parse_args()

if not os.path.exists(args.out_dir):
    os.mkdir(args.out_dir)

existing_files = [f for f in os.listdir(args.out_dir) if f.endswith('.json')]
if args.regEx:
    filelist = sorted([f for f in os.listdir(args.dir) if f.endswith('.json') and args.regEx in f])
else:
    filelist = sorted([f for f in os.listdir(args.dir) if f.endswith('.json')])

print(args.algorithm)

for file in tqdm(filelist):
    substr = file[:file.rfind('_')]
    size = int(substr[1+substr.rfind('_'):])
    if args.sizes:
        if size < args.sizes[0] or size > args.sizes[1]:
            continue
            
    input_file = os.path.join(args.dir, file)

    popen_args = [args.executable,
                  '-f', input_file,
                  '-o', args.out_dir,
                  '-a', args.algorithm,
                  '-d',
                  '--json'
                  ]

    est_filename = file.replace('.json', '_' + args.algorithm)
    algorithm = args.algorithm
    if args.k and args.c:
        # theta and yao
        est_filename += args.k + "-" + args.c
        algorithm += args.k + "-" + args.c
        popen_args.extend(['-k', args.k, '-c', args.c])
    elif args.t:
        # wspd greedy
        est_filename += args.t
        algorithm += args.t
        popen_args.extend(['-t', args.t])
        if args.sep:
            est_filename += "-" + args.sep
            algorithm += "-" + args.sep
            popen_args.extend(['--sep', args.sep])
    if args.v:
        # wspd onion
        est_filename += "-" + args.v
        algorithm += "-" + args.v
        popen_args.extend(['-v', args.v])
        if args.k:
            # onion
            est_filename += "-" + args.k
            algorithm += "-" + args.k
            popen_args.extend(['-k', args.k])
    est_filename += '.json'

    if est_filename in existing_files:
        continue

    output_file = os.path.join(args.out_dir, file.replace('.json', '_' + algorithm + '_raw.out'))

    with open(output_file, "w") as f:
        subprocess.Popen(popen_args, stdout=f, stderr=f).communicate()
