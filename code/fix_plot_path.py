import argparse
import os

parser = argparse.ArgumentParser()

parser.add_argument('-d', '--dir', required=True)
args = parser.parse_args()

for file in [f for f in os.listdir(args.dir) if f.endswith('.plt')]:
    file_processor = open(args.dir + file, "r")
    lines = file_processor.readlines()
    file_processor = open(args.dir + file, "w")
    for line in lines:
        if '/' in line:
            file_processor.write(line.replace(line[line.find('/'):line.rfind('/')+1],''))
        else:
            file_processor.write(line)
    file_processor.close()
