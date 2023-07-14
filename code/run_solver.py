import argparse
import os
import subprocess
import re
import json

parser = argparse.ArgumentParser()

parser.add_argument('-d', '--dir', required=True)
parser.add_argument('-o', '--out_dir', required=True)
parser.add_argument('-e', '--executable', required=True)
parser.add_argument('-r', '--regEx', required=False)
parser.add_argument('-s', '--solver', required=True)
parser.add_argument('--ws', action='store_true', default=False)
parser.add_argument('-t', '--time_limit', required=False)
parser.add_argument('--sizes', nargs='+', type=int)
args = parser.parse_args()

if not os.path.exists(args.out_dir):
    os.mkdir(args.out_dir)

existing_files = [f for f in os.listdir(args.out_dir) if f.endswith('.json')]
if args.regEx:
    filelist = sorted([f for f in os.listdir(args.dir) if f.endswith('.json') and re.search(args.regEx, f)])
else:
    filelist = sorted([f for f in os.listdir(args.dir) if f.endswith('.json')])

for i, file in enumerate(filelist):
    print(str(i+1) + "/" + str(len(filelist)))
    print(file)
    input_file = os.path.join(args.dir, file)

    popen_args = [args.executable,
                  '-f', input_file,
                  '-o', args.out_dir,
                  '--solver', args.solver,
                  '--json',
                  '--plot'
                  ]

    est_filename = file.replace('.json', '_' + args.solver + '.json')

    if est_filename in existing_files:
        continue
        
    if args.sizes:
        with open(input_file) as jsonFile:
            json_file = json.load(jsonFile)
            size = int(json_file['num_vertices'])
            if size < args.sizes[0] or size > args.sizes[1]:
                print("skipped")
                continue
    
    if args.ws:
        popen_args.extend(['--ws'])

    if args.time_limit:
        popen_args.extend(['--timelimit', args.time_limit])

    output_file = os.path.join(args.out_dir, file.replace('.json', '_' + args.solver + '_raw.out'))

    with open(output_file, "w") as f:
        subprocess.Popen(popen_args, stdout=f, stderr=f).communicate()
