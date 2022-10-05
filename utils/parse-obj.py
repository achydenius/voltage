#!/usr/bin/env python3
import sys

def parse_vertex(line):
    return [float(value) for value in line.split(' ')[1:]]

def parse_face(line):
    return [int(value.split('/')[0]) - 1 for value in line.split(' ')[1:]]

def vertex_to_str(vertex):
    return f'{{{", ".join([str(value) for value in vertex])}}}'

def face_to_str(face):
    return f'{{{", ".join([str(index) for index in face])}}}'

if len(sys.argv) != 3:
    print('Usage: ./parse-obj.py <filename> <output-variable>')
    exit()

[filename, variable] = sys.argv[1:]

vertices = []
faces = []
for line in open(filename, 'r'):
    if line.startswith('v '):
        vertices.append(parse_vertex(line))
    elif line.startswith('f '):
        faces.append(parse_face(line))

output = f'''
Vector3 {variable}Vertices[] = {{
    {', '.join([vertex_to_str(vertex) for vertex in vertices])}
}};
voltage::FaceDefinition {variable}Faces[] = {{
    {', '.join([face_to_str(face) for face in faces])}
}};
voltage::Mesh* {variable} = new voltage::Mesh({variable}Vertices, {len(vertices)}, {variable}Faces, {len(faces)});
'''

print(output.strip())
