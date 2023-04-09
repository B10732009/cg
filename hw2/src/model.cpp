#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <glm/vec3.hpp>

Model* Model::fromObjectFile(const char* obj_file) {
  Model* m = new Model();

  std::ifstream ObjFile(obj_file);

  if (!ObjFile.is_open()) {
    std::cout << "Can't open File !" << std::endl;
    return NULL;
  }

  /* TODO#1: Load model data from OBJ file
   *         You only need to handle v, vt, vn, f
   *         Other fields you can directly ignore
   *         Fill data into m->positions, m->texcoords m->normals and m->numVertex
   *         Data format:
   *           For positions and normals
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   | 11   | ...
   *         | face 1                                                       | face 2               ...
   *         | v1x  | v1y  | v1z  | v2x  | v2y  | v2z  | v3x  | v3y  | v3z  | v1x  | v1y  | v1z  | ...
   *         | vn1x | vn1y | vn1z | vn2x | vn2y | vn2z | vn3x | vn3y | vn3z | vn1x | vn1y | vn1z | ...
   *           For texcoords
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | ...
   *         | face 1                                  | face 2        ...
   *         | v1x  | v1y  | v2x  | v2y  | v3x  | v3y  | v1x  | v1y  | ...
   * Note:
   *        OBJ File Format (https://en.wikipedia.org/wiki/Wavefront_.obj_file)
   *        For simplification, all faces from OBJ file are triangle (vertex per face = 3)
   */

  std::string line = "";
  std::string prefix = "";
  std::stringstream ss;
  std::vector<float> v, vt, vn;

  while (getline(ObjFile, line)) {
    ss.clear();
    ss.str(line);
    ss >> prefix;
    if (prefix == "v") {
      float vx, vy, vz;
      ss >> vx >> vy >> vz;
      v.push_back(vx);
      v.push_back(vy);
      v.push_back(vz);
    } else if (prefix == "vt") {
      float vtx, vty;
      ss >> vtx >> vty;
      vt.push_back(vtx);
      vt.push_back(vty);
    } else if (prefix == "vn") {
      float vnx, vny, vnz;
      ss >> vnx >> vny >> vnz;
      vn.push_back(vnx);
      vn.push_back(vny);
      vn.push_back(vnz);
    } else if (prefix == "f") {
      for (int i = 0; i < 3; i++) {
        std::string p;
        ss >> p;
        int vi, vti, vni;
        face_parser(p, vi, vti, vni);
        m->positions.push_back(v[(vi - 1) * 3]);
        m->positions.push_back(v[(vi - 1) * 3 + 1]);
        m->positions.push_back(v[(vi - 1) * 3 + 2]);
        m->texcoords.push_back(vt[(vti - 1) * 2]);
        m->texcoords.push_back(vt[(vti - 1) * 2 + 1]);
        m->normals.push_back(vn[(vni - 1) * 3]);
        m->normals.push_back(vn[(vni - 1) * 3 + 1]);
        m->normals.push_back(vn[(vni - 1) * 3 + 2]);
        m->numVertex++;
      }
    }
  }

  if (ObjFile.is_open()) ObjFile.close();

  return m;
}

void face_parser(std::string str, int& v, int& vt, int& vn) {
  v = 0;
  vt = 0;
  vn = 0;
  int i = 0;
  for (; i < str.size() && str[i] != '/'; i++) {
    v = v * 10 + (int)(str[i] - '0');
  }
  i++;
  for (; i < str.size() && str[i] != '/'; i++) {
    vt = vt * 10 + (int)(str[i] - '0');
  }
  i++;
  for (; i < str.size() && str[i] != '/'; i++) {
    vn = vn * 10 + (int)(str[i] - '0');
  }
}
