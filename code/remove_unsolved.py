import argparse
import os
import json

parser = argparse.ArgumentParser()

parser.add_argument('-d', '--dir', required=True)
args = parser.parse_args()

for file in [f for f in os.listdir(args.dir) if f.endswith('.json')]:
    raw_out = file.replace('.json', '_raw.out')
    plt_out = file.replace('.json', '.plt')
    v_out = file.replace('.json', '.v')
    with open(args.dir + file) as f:
        json_data = json.load(f)
        if len(list(json_data['tour'])) == 0:
            os.remove(args.dir + file)
            os.remove(args.dir + raw_out)
            os.remove(args.dir + plt_out)
            os.remove(args.dir + v_out)
            print(file)
