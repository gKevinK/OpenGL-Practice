#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os
import sys

if len(sys.argv) < 2:
    print("Too few parameters.")
    exit(1)

proj_dir = sys.argv[1]
content = ["#ifndef SHADERS_GLSL_HPP\n#define SHADERS_GLSL_HPP", "#include <string>"]
for file_name in os.listdir(proj_dir):
    path = os.path.join(proj_dir, file_name)
    if not file_name.endswith('.glsl'):
        continue
    if os.path.isdir(path):
        continue
    with open(path, "r") as file:
        # print(file_name)
        string = file.read()
        string = 'const std::string ' + file_name.replace('.', '_') \
                 + ' = R"=====(' + string + ')=====";'
        content.append(string)
content.append("#endif")

target_name = "shaders.glsl.hpp" if len(sys.argv) < 3 else sys.argv[2]
target = open(os.path.join(proj_dir, target_name), "w")
target.write('\n\n'.join(content));
target.close()
print('[ GLSL -> hpp ] ', target_name)
exit(0)
